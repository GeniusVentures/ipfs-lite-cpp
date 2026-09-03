#include "peer_context.hpp"

#include <chrono>
#include <libp2p/connection/stream.hpp>
#include <fmt/chrono.h>

#include "inbound_endpoint.hpp"
#include "message_queue.hpp"
#include "message_reader.hpp"
#include "outbound_endpoint.hpp"

namespace sgns::ipfs_lite::ipfs::graphsync
{
    namespace
    {
        std::string makeStringRepr( const PeerId &peer_id )
        {
            return peer_id.toBase58().substr( 46 );
        }

        /// Needed for sets and maps
        bool less( const PeerId &a, const PeerId &b )
        {
            // N.B. toVector returns const std::vector&, i.e. it is fast
            return a.toVector() < b.toVector();
        }
    }

    class PeerContext::StateLock
    {
    public:
        explicit StateLock( const PeerContext &self ) : self_( self ), lock_( self.state_mutex_ ) {}

        ~StateLock()
        {
            if ( self_.pending_feedbacks_.empty() )
            {
                return;
            }
            // Copy out before unlocking: a callback may destroy the context.
            auto pending   = std::move( self_.pending_feedbacks_ );
            auto feedbacks = self_.graphsync_feedbacks_;
            auto peer      = self_.peer;
            lock_.unlock();
            for ( auto &fn : pending )
            {
                for ( const auto &wfb : feedbacks )
                {
                    if ( auto fb = wfb.lock() )
                    {
                        fn( *fb, peer );
                    }
                }
            }
        }

        StateLock( const StateLock & )            = delete;
        StateLock &operator=( const StateLock & ) = delete;

    private:
        const PeerContext            &self_;
        std::unique_lock<StateMutex> lock_;
    };

    void PeerContext::deferFeedback( Feedback fn )
    {
        pending_feedbacks_.emplace_back( std::move( fn ) );
    }

    PeerContext::PeerContext( PeerId                                               peer_id,
                              std::vector<std::weak_ptr<PeerToGraphsyncFeedback>> &graphsync_feedbacks,
                              PeerToNetworkFeedback                               &network_feedback,
                              libp2p::basic::Scheduler                            &scheduler ) :
        peer( std::move( peer_id ) ),
        str( makeStringRepr( peer ) ),
        graphsync_feedbacks_( graphsync_feedbacks ),
        network_feedback_( network_feedback ),
        scheduler_( scheduler )
    {
    }

    // Need to define it here due to unique_ptrs to incomplete types in the header
    PeerContext::~PeerContext()
    {
        logger()->trace( "~PeerContext, {}", str );
        // must be closed
        if ( !closed_ )
        {
            close( RS_INTERNAL_ERROR );
        }
    }

    bool operator<( const PeerContextPtr &ctx, const PeerId &peer )
    {
        if ( !ctx )
        {
            return false;
        }
        return less( ctx->peer, peer );
    }

    bool operator<( const PeerId &peer, const PeerContextPtr &ctx )
    {
        if ( !ctx )
        {
            return false;
        }
        return less( peer, ctx->peer );
    }

    bool operator<( const PeerContextPtr &a, const PeerContextPtr &b )
    {
        if ( !a || !b )
        {
            return false;
        }
        return less( a->peer, b->peer );
    }

    void PeerContext::setOutboundAddress( boost::optional<std::vector<libp2p::multi::Multiaddress>> connect_to )
    {
        StateLock lock( *this );
        if ( connect_to )
        {
            connect_to_ = std::move( connect_to );
        }
    }

    bool PeerContext::needToConnect()
    {
        StateLock lock( *this );
        if ( requests_endpoint_ )
        {
            return false;
        }

        requests_endpoint_ = std::make_unique<OutboundEndpoint>();

        if ( streams_.empty() )
        {
            return true;
        }

        auto it = streams_.begin();
        createMessageQueue( it->first, it->second );
        requests_endpoint_->onConnected( it->second.queue );

        return false;
    }

    PeerContext::State PeerContext::getState() const
    {
        StateLock lock( *this );
        return getStateLocked();
    }

    PeerContext::State PeerContext::getStateLocked() const
    {
        State state = can_connect;
        if ( closed_ )
        {
            state = is_closed;
        }
        else if ( !streams_.empty() )
        {
            state = is_connected;
        }
        else if ( requests_endpoint_ && requests_endpoint_->isConnecting() )
        {
            state = is_connecting;
        }

        return state;
    }

    libp2p::peer::PeerInfo PeerContext::getOutboundPeerInfo() const
    {
        StateLock lock( *this );
        libp2p::peer::PeerInfo pi{ peer, {} };
        if ( connect_to_ )
        {
            //pi.addresses.push_back(connect_to_.value());
            pi.addresses = connect_to_.value();
        }
        return pi;
    }

    void PeerContext::onNewStream( StreamPtr stream )
    {
        assert( stream );
        assert( streams_.count( stream ) == 0 );

        if ( !stream || (streams_.count( stream ) != 0) )
        {
            logger()->error( "onNewStream: inconsistency, peer={}", str );
            return;
        }

        // Posted: mplex runs this callback synchronously, i.e. under state_mutex_.
        stream->adjustWindowSize( GRAHPSYNC_WINDOW_SIZE,
                                  [wptr{ weak_from_this() }, stream, &scheduler = scheduler_](
                                      IPFS::outcome::result<void> res )
                                  {
                                      scheduler.schedule(
                                          [wptr, stream, res]
                                          {
                                              auto self = wptr.lock();
                                              if ( !self )
                                              {
                                                  return;
                                              }
                                              if ( res )
                                              {
                                                  self->finishStreamConfig( stream );
                                                  return;
                                              }
                                              logger()->error( "cannot adjustWindowSize, peer={} with size {}",
                                                               self->str,
                                                               GRAHPSYNC_WINDOW_SIZE );
                                              if ( self->getState() == is_connecting )
                                              {
                                                  self->close( RS_CANNOT_CONNECT );
                                              }
                                          } );
                                  } );
    }

    void PeerContext::finishStreamConfig( StreamPtr stream )
    {
        StateLock lock( *this );
        StreamCtx stream_ctx;
        stream_ctx.reader = std::make_unique<MessageReader>( stream, shared_from_this() );

        if ( getStateLocked() == is_connecting )
        {
            assert( requests_endpoint_ );

            createMessageQueue( stream, stream_ctx );
            requests_endpoint_->onConnected( stream_ctx.queue );
        }

        if ( streams_.empty() )
        {
            timer_ = scheduler_.scheduleWithHandle(
                [wptr{ weak_from_this() }]()
                {
                    auto self = wptr.lock();
                    if ( self )
                    {
                        self->onStreamCleanupTimer();
                    }
                },
                kPeerCloseDelayMsec // Changed from kStreamCloseDelayMsec
            );
        }

        shiftExpireTime( stream_ctx );

        // Reset peer timeout when new stream is established (connection activity)
        resetPeerTimeout();

        streams_.emplace( std::move( stream ), std::move( stream_ctx ) );
    }

    void PeerContext::onStreamConnected( libp2p::StreamAndProtocolOrError rstream )
    {
        StateLock lock( *this );
        if ( closed_ )
        {
            return;
        }
        if ( rstream )
        {
            logger()->debug( "connected to peer={}", str );
            onNewStream( std::move( rstream.value().stream ) );
        }
        else
        {
            logger()->error( "cannot connect, peer={}, msg='{}' state={}",
                             str,
                             rstream.error().message(),
                             static_cast<int>( getStateLocked() ) );
            if ( getStateLocked() == is_connecting )
            {
                // Close the entire PeerContext, not just local requests
                // This prevents the PeerContext from being reused in a broken state
                closeLocked( RS_CANNOT_CONNECT );
            }
        }
    }

    void PeerContext::onStreamAccepted( StreamPtr stream )
    {
        StateLock lock( *this );
        if ( closed_ )
        {
            stream->reset();
            return;
        }
        onNewStream( std::move( stream ) );
    }

    void PeerContext::enqueueRequest( RequestId request_id, SharedData request_body )
    {
        StateLock lock( *this );
        if ( closed_ )
        {
            logger()->warn(
                "enqueueRequest: PeerContext is already closed for peer {}, state={}, calling onResponse with RS_INTERNAL_ERROR",
                str,
                static_cast<int>( getStateLocked() ) );
            // Immediately notify failure for this request since the context is closed
            deferFeedback( [request_id]( PeerToGraphsyncFeedback &fb, const PeerId &peer )
                           { fb.onResponse( peer, request_id, RS_INTERNAL_ERROR, {} ); } );
            return;
        }

        if ( !requests_endpoint_ )
        {
            logger()->error( "enqueueRequest: Internal error, state={}, calling close()",
                             static_cast<int>( getStateLocked() ) );
            closeLocked( RS_INTERNAL_ERROR );
            return;
        }
        auto res = requests_endpoint_->enqueue( std::move( request_body ) );
        if ( res )
        {
            local_request_ids_.insert( request_id );
            logger()->debug( "enqueueRequest: request_id {} added to local_request_ids_ for peer {}, state={}",
                             request_id,
                             str,
                             static_cast<int>( getStateLocked() ) );
        }
        else
        {
            logger()->error( "enqueueRequest: outbound buffers overflow for peer {}, state={}, calling close()",
                             str,
                             static_cast<int>( getStateLocked() ) );
            closeLocked( RS_SLOW_STREAM );
        }
    }

    void PeerContext::cancelRequest( RequestId request_id, SharedData request_body )
    {
        StateLock lock( *this );
        if ( closed_ )
        {
            logger()->trace( "cancelRequest: PeerContext is already closed for peer {}", str );
            return;
        }

        local_request_ids_.erase( request_id );
        if ( requests_endpoint_ )
        {
            if ( requests_endpoint_->enqueue( std::move( request_body ) ) )
            {
                return;
            }
            logger()->error( "cancelRequest: outbound buffers overflow for peer {}", str );
        }
    }

    PeerContext::Streams::iterator PeerContext::findResponseSink( RequestId request_id )
    {
        auto r_iter = remote_requests_streams_.find( request_id );
        if ( r_iter == remote_requests_streams_.end() )
        {
            logger()->debug( "findResponseSink: remote request {} is no longer actual, peer={}", request_id, str );
            return streams_.end();
        }
        auto s_iter = streams_.find( r_iter->second );
        if ( s_iter == streams_.end() )
        {
            logger()->error( "findResponseSink: cannot find stream, peer={}", str );
        }
        return s_iter;
    }

    bool PeerContext::addBlockToResponse( RequestId request_id, const CID &cid, const common::Buffer &data )
    {
        StateLock lock( *this );
        auto it = findResponseSink( request_id );
        if ( it == streams_.end() )
        {
            return false;
        }
        auto &ctx = it->second;

        createResponseEndpoint( it->first, ctx );

        auto res = ctx.response_endpoint->addBlockToResponse( request_id, cid, data );
        if ( !res )
        {
            logger()->error( "addBlockToResponse: {}, peer={}", res.error().message(), str );

            closeLocked( RS_SLOW_STREAM );
            return false;
        }
        return true;
    }

    void PeerContext::sendResponse( RequestId                     request_id,
                                    ResponseStatusCode            status,
                                    const std::vector<Extension> &extensions )
    {
        StateLock lock( *this );
        sendResponseLocked( request_id, status, extensions );
    }

    void PeerContext::sendResponseLocked( RequestId                     request_id,
                                          ResponseStatusCode            status,
                                          const std::vector<Extension> &extensions )
    {
        auto it = findResponseSink( request_id );
        if ( it == streams_.end() )
        {
            return;
        }
        auto &ctx = it->second;

        createResponseEndpoint( it->first, ctx );

        auto res = ctx.response_endpoint->sendResponse( request_id, status, extensions );
        if ( !res )
        {
            logger()->error( "sendResponse: {}, peer={}", res.error().message(), str );

            closeLocked( RS_SLOW_STREAM );
        }
    }

    void PeerContext::keepAlive()
    {
        StateLock lock( *this );
        resetPeerTimeout();
    }

    void PeerContext::close( ResponseStatusCode status )
    {
        StateLock lock( *this );
        closeLocked( status );
    }

    void PeerContext::closeLocked( ResponseStatusCode status )
    {
        if ( closed_ )
        {
            return;
        }

        logger()->debug( "close peer={} status={}", str, statusCodeToString( status ) );

        closed_ = true;
        remote_requests_streams_.clear();
        while ( !streams_.empty() )
        {
            auto s = streams_.begin()->first;
            closeStream( s, status );
        }

        if ( requests_endpoint_ )
        {
            closeLocalRequests( status );
        }

        timer_ = scheduler_.scheduleWithHandle(
            [wptr{ weak_from_this() }, status]
            {
                if ( auto self = wptr.lock() )
                {
                    self->network_feedback_.peerClosed( self->peer, status );
                }
            } );
    }

    void PeerContext::closeStream( StreamPtr stream, ResponseStatusCode status )
    {
        auto it = streams_.find( stream );
        if ( it == streams_.end() )
        {
            logger()->error( "closeStream: stream not found, peer={}", str );
            return;
        }

        logger()->trace( "closeStream: peer={}", str );

        for ( auto id : it->second.remote_request_ids )
        {
            remote_requests_streams_.erase( id );
        }

        streams_.erase( it );

        if ( requests_endpoint_ && requests_endpoint_->getStream() == stream )
        {
            closeLocalRequests( status );
        }

        stream->close( [stream]( IPFS::outcome::result<void> ) { logger()->trace( "stream closed" ); } );

        // If this was the last stream and we're closing due to an error,
        // close the entire PeerContext to prevent new requests from being queued to a broken peer
        if ( !closed_ && streams_.empty() && isError( status ) )
        {
            closeLocked( status );
        }
    }

    void PeerContext::closeLocalRequests( ResponseStatusCode status )
    {
        logger()->debug( "closeLocalRequests: peer={} status={} num_requests={}",
                         str,
                         statusCodeToString( status ),
                         local_request_ids_.size() );
        if ( !local_request_ids_.empty() )
        {
            deferFeedback( [ids = std::move( local_request_ids_ ), status]( PeerToGraphsyncFeedback &fb,
                                                                             const PeerId            &peer )
                           {
                               for ( auto id : ids )
                               {
                                   fb.onResponse( peer, id, status, {} );
                               }
                           } );
            local_request_ids_.clear();
        }
        requests_endpoint_.reset();
    }

    void PeerContext::onResponse( Message::Response &response )
    {
        auto it = local_request_ids_.find( response.id );
        if ( it == local_request_ids_.end() )
        {
            logger()->info( "ignoring response for unexpected request id={} from peer {}", response.id, str );
            return;
        }

        if ( isTerminal( response.status ) )
        {
            local_request_ids_.erase( it );
        }
        deferFeedback( [id = response.id, status = response.status, extensions = std::move( response.extensions )](
                           PeerToGraphsyncFeedback &fb, const PeerId &peer )
                       { fb.onResponse( peer, id, status, extensions ); } );
    }

    void PeerContext::onRequest( const StreamPtr &stream, Message::Request &request )
    {
        auto it = streams_.find( stream );
        if ( it == streams_.end() )
        {
            logger()->error( "onRequest: stream not found, peer={}", str );
            return;
        }
        StreamCtx &ctx = it->second;

        if ( request.cancel )
        {
            ctx.remote_request_ids.erase( request.id );
            remote_requests_streams_.erase( request.id );
            logger()->debug( "onRequest: peer {} cancelled request {}", str, request.id );
        }
        else
        {
            createResponseEndpoint( stream, ctx );
            if ( remote_requests_streams_.count( request.id ) != 0 )
            {
                sendResponseLocked( request.id, RS_REJECTED, {} );
            }
            else
            {
                remote_requests_streams_.emplace( request.id, stream );
                ctx.remote_request_ids.insert( request.id );
                logger()->debug( "onRequest: peer {} created request {} sending to {} feedbacks",
                                 str,
                                 request.id,
                                 graphsync_feedbacks_.size() );
                deferFeedback( [request = std::move( request )]( PeerToGraphsyncFeedback &fb, const PeerId &peer )
                               { fb.onRemoteRequest( peer, request ); } );
            }
        }
    }

    void PeerContext::createMessageQueue( const StreamPtr &stream, PeerContext::StreamCtx &ctx )
    {
        if ( !ctx.queue )
        {
            ctx.queue = std::make_shared<MessageQueue>(
                stream,
                [wptr{ weak_from_this() }]( const StreamPtr &stream, IPFS::outcome::result<void> res )
                {
                    if ( auto self = wptr.lock() )
                    {
                        self->onWriterEvent( stream, res );
                    }
                },
                scheduler_ );
        }
    }

    void PeerContext::createResponseEndpoint( const StreamPtr &stream, PeerContext::StreamCtx &ctx )
    {
        createMessageQueue( stream, ctx );
        if ( !ctx.response_endpoint )
        {
            ctx.response_endpoint = std::make_unique<InboundEndpoint>( ctx.queue );
        }
    }

    void PeerContext::onReaderEvent( const StreamPtr &stream, IPFS::outcome::result<Message> msg_res )
    {
        StateLock lock( *this );
        if ( !stream )
        {
            logger()->error( "stream read error: this stream is null" );
            return;
        }
        if ( closed_ )
        {
            logger()->info( "stream read error: this stream is closed closed" );
            return;
        }

        if ( !msg_res )
        {
            logger()->info( "stream read error, peer={}, msg={}", str, msg_res.error().message() );
            closeStream( stream, RS_CONNECTION_ERROR );
            return;
        }

        Message &msg = msg_res.value();

        // Reset peer timeout when data activity occurs (message received)
        resetPeerTimeout();

        logger()->trace( "message from peer={}, {} blocks, {} requests, {} responses",
                         str,
                         msg.data.size(),
                         msg.requests.size(),
                         msg.responses.size() );

        auto it = streams_.find( stream );
        if ( it == streams_.end() )
        {
            logger()->error( "onReaderEvent: stream not found, peer={}", str );
            return;
        }

        if ( msg.complete_request_list )
        {
            for ( auto id : it->second.remote_request_ids )
            {
                remote_requests_streams_.erase( id );
            }
            it->second.remote_request_ids.clear();
        }

        for ( auto &item : msg.requests )
        {
            onRequest( stream, item );
        }

        for ( auto &item : msg.responses )
        {
            onResponse( item );
        }

        if ( !msg.data.empty() )
        {
            // One deferred call per message, not per block: this is the hot path.
            deferFeedback(
                [data = std::move( msg.data )]( PeerToGraphsyncFeedback &fb, const PeerId &peer )
                {
                    const CID &root_cid = data.front().first;
                    for ( const auto &[cid, bytes] : data )
                    {
                        fb.onBlock( peer, root_cid, cid, bytes );
                    }
                } );
        }

        // The stream may have been closed while handling the message.
        auto final_it = streams_.find( stream );
        if ( final_it != streams_.end() )
        {
            shiftExpireTime( final_it->second );
        }
    }

    void PeerContext::onWriterEvent( const StreamPtr &stream, IPFS::outcome::result<void> result )
    {
        StateLock lock( *this );
        if ( closed_ )
        {
            return;
        }

        if ( !result )
        {
            logger()->info( "stream write error, peer={}, msg={}", str, result.error().message() );
            closeLocked( RS_CONNECTION_ERROR );
            return;
        }

        // Reset peer timeout when data activity occurs (write completed)
        resetPeerTimeout();

        // Check if stream still exists - it might have been closed during close()
        auto it = streams_.find( stream );
        if ( it != streams_.end() )
        {
            shiftExpireTime( it->second );
        }
    }

    void PeerContext::shiftExpireTime( PeerContext::StreamCtx &ctx )
    {
        ctx.expire_time = scheduler_.now() + kStreamCloseDelayMsec;
    }

    void PeerContext::resetPeerTimeout()
    {
        if ( closed_ )
        {
            return;
        }

        // Peer timer tracks peer liveness only. Per-stream timeout extension is handled
        // inside onStreamCleanupTimer() using each stream's own expire_time.
        auto res = timer_.reschedule( kPeerCloseDelayMsec );
        if ( !res )
        {
            logger()->warn( "resetPeerTimeout: cannot reschedule peer timer for peer={}, msg='{}'",
                            str,
                            res.error().message() );
        }
    }

    void PeerContext::onStreamCleanupTimer()
    {
        StateLock lock( *this );
        std::chrono::milliseconds max_expire_time(0);

        if ( streams_.empty() )
        {
            closeLocked( RS_TIMEOUT );
            return;
        }

        auto now = scheduler_.now();

        std::vector<StreamPtr> timed_out;

        for ( auto &[stream, ctx] : streams_ )
        {
            if ( ctx.queue )
            {
                auto queue_state = ctx.queue->getState();

                // If actively writing, don't timeout
                if ( queue_state.writing_bytes > 0 )
                {
                    continue;
                }

                // If we have pending bytes but not actively writing, this could indicate
                // window exhaustion - extend the timeout significantly to allow recovery
                if ( queue_state.pending_bytes > 0 )
                {
                    // Extend timeout for window exhaustion scenarios
                    ctx.expire_time = std::max( ctx.expire_time,
                                                now + ( kStreamCloseDelayMsec * kWindowExhaustionTimeoutMultiplier ) );
                    if ( ctx.expire_time > max_expire_time )
                    {
                        max_expire_time = ctx.expire_time;
                    }
                    logger()->trace( "Window exhaustion detected for peer {}: {} pending bytes, extending timeout",
                                     str,
                                     queue_state.pending_bytes );
                    continue;
                }
            }

            if ( ctx.expire_time <= now )
            {
                timed_out.push_back( stream );
            }
            else if ( ctx.expire_time > max_expire_time )
            {
                max_expire_time = ctx.expire_time;
            }
        }

        for ( auto &stream : timed_out )
        {
            closeStream( std::move( stream ), RS_TIMEOUT );
        }

        // If closeStream closed the peer and cleared streams_, don't reschedule
        // the timer with a stale handle (avoids scheduler assert on reschedule).
        if ( closed_ || streams_.empty() )
        {
            return;
        }

        if ( max_expire_time > now )
        {
            auto res = timer_.reschedule( max_expire_time - now );
            if ( !res )
            {
                logger()->warn( "onStreamCleanupTimer: cannot reschedule timeout for peer={}, msg='{}'",
                                str,
                                res.error().message() );
            }
        }
        else
        {
            auto res = timer_.reschedule( kPeerCloseDelayMsec );
            if ( !res )
            {
                logger()->warn( "onStreamCleanupTimer: cannot reschedule default timeout for peer={}, msg='{}'",
                                str,
                                res.error().message() );
            }
        }
    }

}
