#include "network.hpp"

#include <cassert>

#include "inbound_endpoint.hpp"
#include "message_reader.hpp"
#include "outbound_endpoint.hpp"
#include "peer_context.hpp"

namespace sgns::ipfs_lite::ipfs::graphsync
{

    Network::Network( std::shared_ptr<libp2p::Host> host, std::shared_ptr<libp2p::basic::Scheduler> scheduler ) :
        host_( std::move( host ) ), scheduler_( std::move( scheduler ) ), protocol_id_( kProtocolVersion )
    {
        assert( host_ );
        assert( scheduler_ );
    }

    Network::~Network()
    {
        closeAllPeers();
    }

    void Network::start( std::shared_ptr<PeerToGraphsyncFeedback> feedback )
    {
        assert( feedback );

        std::lock_guard<std::mutex> lock( state_mutex_ );

        bool first_feedback = feedbacks_.empty();
        feedbacks_.emplace_back( feedback );

        if ( first_feedback )
        {
            host_->setProtocolHandler( { protocol_id_ },
                                       [wptr = weak_from_this()]( libp2p::StreamAndProtocol rstream )
                                       {
                                           if ( auto self = wptr.lock() )
                                           {
                                               self->onStreamAccepted( std::move( rstream ) );
                                           }
                                       } );
            started_ = true;
        }
    }

    void Network::stop( const std::shared_ptr<PeerToGraphsyncFeedback> &feedback )
    {
        bool should_close = false;
        {
            std::lock_guard<std::mutex> lock( state_mutex_ );
            feedbacks_.erase( std::remove_if( feedbacks_.begin(),
                                              feedbacks_.end(),
                                              [&]( const std::weak_ptr<PeerToGraphsyncFeedback> &f )
                                              {
                                                  auto s = f.lock();
                                                  return !s || s == feedback;
                                              } ),
                              feedbacks_.end() );
            if ( feedbacks_.empty() )
            {
                started_      = false;
                should_close = true;
            }
        }

        if ( should_close )
        {
            closeAllPeers();
        }
    }

    bool Network::canSendRequest( const PeerId &peer )
    {
        {
            std::lock_guard<std::mutex> lock( state_mutex_ );
            if ( !started_ )
            {
                return false;
            }
        }

        auto ctx = findContext( peer, true );
        if ( !ctx )
        {
            return false;
        }
        if ( ctx->getState() == PeerContext::is_closed )
        {
            std::lock_guard<std::mutex> lock( state_mutex_ );
            auto it = peers_.find( peer );
            if ( it != peers_.end() && *it == ctx )
            {
                peers_.erase( it );
            }
            return false;
        }
        return true;
    }

    void Network::makeRequest( const PeerId                                             &peer,
                               boost::optional<std::vector<libp2p::multi::Multiaddress>> address,
                               RequestId                                                 request_id,
                               SharedData                                                request_body )
    {
        if ( !canSendRequest( peer ) )
        {
            logger()->error( "inconsistency in making request to network" );
            return;
        }

        auto ctx = findContext( peer, false );
        assert( ctx );

        logger()->trace( "makeRequest: {} has state {}", ctx->str, fmt::underlying( ctx->getState() ) );

        ctx->setOutboundAddress( std::move( address ) );
        if ( ctx->needToConnect() )
        {
            tryConnect( ctx );
        }

        ctx->enqueueRequest( request_id, std::move( request_body ) );
    }

    void Network::cancelRequest( RequestId request_id, SharedData request_body )
    {
        PeerContextPtr ctx;
        {
            std::lock_guard<std::mutex> lock( state_mutex_ );
            if ( !started_ )
            {
                return;
            }

            auto it = active_requests_per_peer_.find( request_id );
            if ( it == active_requests_per_peer_.end() )
            {
                return;
            }

            ctx = it->second;
            active_requests_per_peer_.erase( it );
        }

        if ( request_body && ctx )
        {
            ctx->cancelRequest( request_id, std::move( request_body ) );
        }
    }

    bool Network::addBlockToResponse( const PeerId         &peer,
                                      RequestId             request_id,
                                      const CID            &cid,
                                      const common::Buffer &data )
    {
        if ( !started_ )
        {
            return false;
        }

        auto ctx = findContext( peer, false );
        if ( !ctx )
        {
            return false;
        }

        return ctx->addBlockToResponse( request_id, cid, data );
    }

    void Network::sendResponse( const PeerId                 &peer,
                                int                           request_id,
                                ResponseStatusCode            status,
                                const std::vector<Extension> &extensions )
    {
        if ( !started_ )
        {
            return;
        }

        auto ctx = findContext( peer, false );
        if ( !ctx )
        {
            return;
        }

        ctx->sendResponse( request_id, status, extensions );
    }

    void Network::keepPeerAlive( const PeerId &peer )
    {
        {
            std::lock_guard<std::mutex> lock( state_mutex_ );
            if ( !started_ )
            {
                return;
            }
        }

        auto ctx = findContext( peer, false );
        if ( !ctx )
        {
            return;
        }
        ctx->keepAlive();
    }

    void Network::peerClosed( const PeerId &peer, ResponseStatusCode status )
    {
        (void)status;
        std::lock_guard<std::mutex> lock( state_mutex_ );
        auto it = peers_.find( peer );
        if ( it != peers_.end() )
        {
            peers_.erase( it );
        }
    }

    PeerContextPtr Network::findContext( const PeerId &peer, bool create_if_not_found )
    {
        PeerContextPtr ctx;
        bool ctx_is_closed = false;

        std::lock_guard<std::mutex> lock( state_mutex_ );
        if ( !started_ )
        {
            return {};
        }

        auto it = peers_.find( peer );
        if ( it != peers_.end() )
        {
            ctx = *it;
            ctx_is_closed = ( ctx->getState() == PeerContext::is_closed );
        }

        if ( ctx_is_closed )
        {
            if ( it != peers_.end() )
            {
                peers_.erase( it );
            }
            ctx.reset();
        }

        if ( !ctx && create_if_not_found )
        {
            ctx = std::make_shared<PeerContext>( peer, feedbacks_, *this, *scheduler_ );
            peers_.insert( ctx );
        }

        return ctx;
    }

    void Network::tryConnect( const PeerContextPtr &ctx )
    {
        libp2p::peer::PeerInfo pi = ctx->getOutboundPeerInfo();

        logger()->trace( "connecting to {}, {}",
                         ctx->str,
                         pi.addresses.empty() ? "''" : pi.addresses[0].getStringAddress() );

        host_->newStream(
            pi,
            { protocol_id_ },
            [wptr{ ctx->weak_from_this() }]( libp2p::StreamAndProtocolOrError rstream )
            {
                auto ctx_ = wptr.lock();
                if ( ctx_ )
                {
                    ctx_->onStreamConnected( std::move( rstream ) );
                }
            },
            std::chrono::milliseconds( 10000 ) );
    }

    void Network::onStreamAccepted( libp2p::StreamAndProtocolOrError rstream )
    {
        {
            std::lock_guard<std::mutex> lock( state_mutex_ );
            if ( !started_ )
            {
                return;
            }
        }

        if ( !rstream )
        {
            logger()->error( "accept error, msg='{}'", rstream.error().message() );
            return;
        }

        auto peer_id_res = rstream.value().stream->remotePeerId();
        if ( !peer_id_res )
        {
            logger()->error( "no peer id for accepted stream, msg='{}'", rstream.error().message() );
            return;
        }

        auto ctx = findContext( peer_id_res.value(), true );

        logger()->trace( "accepted stream from peer={}", ctx->str );

        ctx->onStreamAccepted( std::move( rstream.value().stream ) );
    }

    void Network::closeAllPeers()
    {
        PeerSet peers;
        {
            std::lock_guard<std::mutex> lock( state_mutex_ );
            peers = std::move( peers_ );
            active_requests_per_peer_.clear();
        }

        for ( auto &ctx : peers )
        {
            ctx->close( RS_REJECTED_LOCALLY );
        }
    }

}
