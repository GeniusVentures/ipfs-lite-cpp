#include "graphsync_impl.hpp"

#include <cassert>
#include <utility>
#include <fmt/chrono.h>

#include "ipfs_lite/ipld/ipld_node.hpp"
#include "network/network.hpp"
#include "local_requests.hpp"

// #include "network/network.hpp"
OUTCOME_CPP_DEFINE_CATEGORY_3( sgns::ipfs_lite::ipfs::graphsync, GraphsyncImpl::Error, e )
{
    switch ( e )
    {
        case sgns::ipfs_lite::ipfs::graphsync::GraphsyncImpl::Error::REQUEST_NOT_FOUND:
            return "The Request was not found, it has either been cleaned up or did not exist to begin with.";
    }
    return "Unknown error";
}

namespace sgns::ipfs_lite::ipfs::graphsync
{
    /// Selector that matches current node
    common::Buffer kSelectorMatcher{ 0xa1, 0x61, 0x2e, 0xa0 };

    GraphsyncImpl::GraphsyncImpl( std::shared_ptr<libp2p::Host>             host,
                                  std::shared_ptr<libp2p::basic::Scheduler> scheduler,
                                  std::shared_ptr<Network>                  network,
                                  std::shared_ptr<RequestIdGenerator>       generator,
                                  std::shared_ptr<boost::asio::io_context>  io_context ) :
        scheduler_( scheduler ),
        network_( std::move( network ) ),
        local_requests_( std::make_shared<LocalRequests>(
            std::move( scheduler ),
            [this]( RequestId request_id, SharedData body ) { cancelLocalRequest( request_id, std::move( body ) ); },
            generator ) ),
        io_context_( std::move( io_context ) ),
        reqgenerator_( std::move( generator ) )
    {
        scheduler_->schedule(
            [weak_this = weak_from_this()]()
            {
                auto self = weak_this.lock();
                if ( self )
                {
                    self->cleanupOldRequests();
                }
            },
            kCleanupIntervalMs );
    }

    GraphsyncImpl::~GraphsyncImpl()
    {
        doStop();
    }

    void GraphsyncImpl::start( std::shared_ptr<merkledag::MerkleDagService> service, Graphsync::BlockCallback callback )
    {
        assert( service );
        assert( callback );

        network_->start( shared_from_this() );
        service_  = std::move( service );
        block_cb_ = std::move( callback );
        started_  = true;
    }

    void GraphsyncImpl::stop()
    {
        doStop();
    }

    void GraphsyncImpl::doStop()
    {
        if ( started_ )
        {
            started_  = false;
            block_cb_ = Graphsync::BlockCallback{};
            service_.reset();
            network_->stop( shared_from_this() );
            logger()->trace( "{}: Stopping all", __FUNCTION__ );
            tracked_requests_.clear();

            local_requests_->cancelAll();
        }
    }

    Subscription GraphsyncImpl::makeRequest( const libp2p::peer::PeerId                               &peer,
                                             boost::optional<std::vector<libp2p::multi::Multiaddress>> address,
                                             const CID                                                &root_cid,
                                             gsl::span<const uint8_t>                                  selector,
                                             const std::vector<Extension>                             &extensions,
                                             RequestProgressCallback                                   callback )
    {
        if ( !started_ || !network_->canSendRequest( peer ) )
        {
            logger()->trace( "makeRequest: rejecting request to peer {}", peer.toBase58().substr( 46 ) );
            return local_requests_->newRejectedRequest( std::move( callback ) );
        }

        //if (selector.empty()) {
        //  selector = kSelectorMatcher;
        //}

        std::lock_guard<std::mutex> lock( requested_cids_mutex_ );
        auto                        it = tracked_requests_.find( root_cid );

        if ( it != tracked_requests_.end() && it->second.state == RequestState::IN_PROGRESS )
        {
            // Only reject if there's an in-progress request
            logger()->trace( "makeRequest: already in progress for {}", root_cid.toString().value() );
            return local_requests_->newRejectedRequest( std::move( callback ) );
        }

        auto newRequest = local_requests_->newRequest( root_cid, selector, extensions, std::move( callback ) );

        // Update or insert the tracking info with timestamps
        auto now                    = scheduler_->now();
        tracked_requests_[root_cid] = { RequestState::IN_PROGRESS, newRequest.request_id, now, now };

        if ( newRequest.request_id > 0 )
        {
            assert( newRequest.body );
            assert( !newRequest.body->empty() );

            logger()->trace( "makeRequest: sending request to peer {}", peer.toBase58().substr( 46 ) );

            network_->makeRequest( peer, std::move( address ), newRequest.request_id, std::move( newRequest.body ) );
        }

        return std::move( newRequest.subscription );
    }

    void GraphsyncImpl::onResponse( const PeerId          &peer,
                                    int                    request_id,
                                    ResponseStatusCode     status,
                                    std::vector<Extension> extensions )
    {
        // TODO peer ratings according to status

        if ( !started_ )
        {
            return;
        }

        auto root_cid = local_requests_->getRequestRootCid( request_id );
        if ( root_cid )
        {
            std::lock_guard<std::mutex> lock( requested_cids_mutex_ );
            auto                        it = tracked_requests_.find( root_cid.value() );
            if ( it != tracked_requests_.end() )
            {
                // Update activity time for any response
                it->second.last_activity_time = scheduler_->now();

                if ( isTerminal( status ) )
                {
                    if ( status == RS_FULL_CONTENT )
                    {
                        // Only mark as completed if we actually received blocks
                        // (the state might already be COMPLETED from onBlock)
                        if ( it->second.state == RequestState::IN_PROGRESS )
                        {
                            // We got success response but no blocks yet - this shouldn't normally happen
                            logger()->warn( "Request {} for CID {} got RS_FULL_CONTENT but no blocks received yet",
                                            request_id,
                                            root_cid.value().toString().value() );
                        }
                        else if ( it->second.state == RequestState::COMPLETED )
                        {
                            logger()->trace( "Request {} for CID {} confirmed completed with RS_FULL_CONTENT",
                                             request_id,
                                             root_cid.value().toString().value() );
                        }
                    }
                    else
                    {
                        // Error cases
                        it->second.state = RequestState::FAILED;
                        logger()->debug( "Request {} for CID {} failed with status {}",
                                         request_id,
                                         root_cid.value().toString().value(),
                                         statusCodeToString( status ) );
                    }
                }
                else
                {
                    logger()->trace( "Request {} for CID {} received status {}",
                                     request_id,
                                     root_cid.value().toString().value(),
                                     statusCodeToString( status ) );
                }
            }
            else
            {
                logger()->warn( "onResponse for request_id {} (CID {}) but not found in tracked_requests_",
                                request_id,
                                root_cid.value().toString().value() );
            }
        }
        else
        {
            logger()->warn( "onResponse for request_id {} but could not find root CID", request_id );
        }
        local_requests_->onResponse( request_id, status, std::move( extensions ) );
    }

    void GraphsyncImpl::onBlock( const PeerId &from, const CID &root_cid, const CID &cid, common::Buffer data )
    {
        // TODO peer ratings according to status
        logger()->trace( "Got a block for CID {}", cid.toString().value() );
        if ( !started_ )
        {
            logger()->error( "Got a block, but Graphsync Not Started" );
            return;
        }

        // Check if this is a tracked request before processing
        bool should_mark_completed = false;
        {
            std::lock_guard<std::mutex> lock( requested_cids_mutex_ );
            auto                        it = tracked_requests_.find( root_cid );
            if ( it == tracked_requests_.end() )
            {
                logger()->debug( "Got a block, but we're not waiting for this root cid {} to cid{}",
                                 root_cid.toString().value(),
                                 cid.toString().value() );
                //this is not an error, it's because the request grabs other blocks as well
                return;
            }

            // Update activity time since we received a block (actual data progress)
            it->second.last_activity_time = scheduler_->now();

            // Check if we should mark as completed after the callback
            should_mark_completed = ( it->second.state == RequestState::IN_PROGRESS );
        }

        // Call the block callback first - this processes the actual data
        logger()->trace( "Block callback for CID {}", cid.toString().value() );
        block_cb_( cid, std::move( data ) );

        // Only mark as completed AFTER the block has been successfully processed
        if ( should_mark_completed )
        {
            std::lock_guard<std::mutex> lock( requested_cids_mutex_ );
            auto                        it = tracked_requests_.find( root_cid );
            if ( it != tracked_requests_.end() && it->second.state == RequestState::IN_PROGRESS )
            {
                it->second.state = RequestState::COMPLETED;
                logger()->trace( "Request {} for root CID {} marked as COMPLETED (block processed for CID {})",
                                 it->second.request_id,
                                 root_cid.toString().value(),
                                 cid.toString().value() );
            }
        }
    }

    void GraphsyncImpl::onRemoteRequest( const PeerId &from, Message::Request request )
    {
        logger()->trace( "onRemoteRequest" );
        // Don't process if we're shutting down
        if ( !started_ )
        {
            logger()->trace( "onRemoteRequest Not Started" );
            return;
        }

        // Register a status holder for this instance
        std::shared_ptr<StatusHolder> status_holder = reqgenerator_->registerStatus( request.id );

        // Capture the peer and request by value since they're not shared_ptr
        PeerId           peer_copy    = from;
        Message::Request request_copy = request;

        // Safety: capture weak_ptr to self
        std::weak_ptr<GraphsyncImpl> weak_self = shared_from_this();

        // Post the task to the io_context thread pool - pass shared_ptr by value for safety
        boost::asio::post(
            *io_context_,
            [weak_self, peer_copy, request_copy, status_holder, this]
            {
                // Convert weak_ptr to shared_ptr to check if GraphsyncImpl is still alive
                logger()->trace( "onRemoteRequest Post" );
                auto self = weak_self.lock();
                if ( !self || !self->started_ )
                {
                    logger()->trace( "onRemoteRequest weak pointer fail" );

                    // Clean up our status holder if we still have the generator
                    if ( self )
                    {
                        self->reqgenerator_->removeStatus( request_copy.id, status_holder );
                    }
                    return;
                }

                // Keep peer alive at the start of processing
                self->network_->keepPeerAlive( peer_copy );

                bool               send_response = true;
                ResponseStatusCode status        = RS_REQUEST_FAILED;

                auto data_handler = [&]( const CID &cid, const common::Buffer &data ) -> bool
                {
                    bool data_present = !data.empty();

                    if ( !data_present )
                    {
                        return true;
                    }

                    // Keep peer alive during block processing
                    self->network_->keepPeerAlive( peer_copy );

                    // Use the original shared_ptr directly
                    if ( !self->network_->addBlockToResponse( peer_copy, request_copy.id, cid, data ) )
                    {
                        send_response = false;
                        return false;
                    }

                    return true;
                };

                // Execute the potentially heavy selection process using original shared_ptr
                auto select_res = [&]() -> IPFS::outcome::result<size_t>
                {
                    auto internal_handler = [&data_handler]( std::shared_ptr<const ipld::IPLDNode> node ) -> bool
                    { return data_handler( node->getCID(), node->getRawBytes() ); };

                    if ( request_copy.selector.empty() )
                    {
                        BOOST_OUTCOME_TRY( auto node, service_->getNode( request_copy.root_cid ) );
                        internal_handler( node );
                        return 1;
                    }

                    // TODO(???): change MerkleDAG service to accept CID instead of bytes
                    BOOST_OUTCOME_TRY( auto cid_encoded, request_copy.root_cid.toBytes() );
                    return service_->select( cid_encoded, request_copy.selector, internal_handler );
                }();

                if ( !send_response )
                {
                    // Update status and remove our status holder
                    status_holder->status = RS_REQUEST_FAILED;
                    self->reqgenerator_->removeStatus( request_copy.id, status_holder );
                    return;
                }

                if ( !select_res )
                {
                    // Selection failed
                    status_holder->status = RS_REQUEST_FAILED;
                    self->reqgenerator_->removeStatus( request_copy.id, status_holder );
                    return;
                }

                if ( select_res.value() > 0 )
                {
                    // Content found!
                    status                = RS_FULL_CONTENT;
                    status_holder->status = RS_FULL_CONTENT;

                    // Keep peer alive before sending response
                    self->network_->keepPeerAlive( peer_copy );

                    // Verify we're still started and can send responses before attempting
                    if ( self->started_ )
                    {
                        // Send the positive response immediately
                        self->network_->sendResponse( peer_copy, request_copy.id, status, request_copy.extensions );
                    }
                    else
                    {
                        logger()->debug( "Skipping response send - GraphSync no longer started for request {}",
                                         request_copy.id );
                    }

                    // Remove all tracking for this request
                    self->reqgenerator_->removeRequest( request_copy.id );
                    return;
                }

                // No content found
                status                = RS_NOT_FOUND;
                status_holder->status = RS_NOT_FOUND;

                // Check if all other instances also reported NOT_FOUND
                if ( self->reqgenerator_->allStatusesNotFound( request_copy.id ) )
                {
                    // Keep peer alive before sending response
                    self->network_->keepPeerAlive( peer_copy );

                    // Verify we're still started before attempting to send
                    if ( self->started_ )
                    {
                        // All instances reported NOT_FOUND, safe to send the response
                        self->network_->sendResponse( peer_copy, request_copy.id, status, request_copy.extensions );
                    }
                    else
                    {
                        logger()->debug(
                            "Skipping NOT_FOUND response send - GraphSync no longer started for request {}",
                            request_copy.id );
                    }

                    // Remove all tracking for this request
                    self->reqgenerator_->removeRequest( request_copy.id );
                }
            } );
    }

    void GraphsyncImpl::cancelLocalRequest( RequestId request_id, SharedData body )
    {
        network_->cancelRequest( request_id, std::move( body ) );
    }

    void GraphsyncImpl::cleanupOldRequests()
    {
        std::lock_guard<std::mutex> lock( requested_cids_mutex_ );
        auto                        now = scheduler_->now();

        for ( auto it = tracked_requests_.begin(); it != tracked_requests_.end(); )
        {
            bool should_remove = false;

            // Remove COMPLETED or FAILED requests
            if ( it->second.state != RequestState::IN_PROGRESS )
            {
                should_remove = true;
            }
            // Check for stalled IN_PROGRESS requests
            else if ( it->second.state == RequestState::IN_PROGRESS )
            {
                auto time_since_activity = now - it->second.last_activity_time;
                auto time_since_start    = now - it->second.start_time;

                if ( time_since_activity > kRequestActivityTimeoutMs )
                {
                    logger()->error( "Request for CID {} has been stalled for {}ms (total: {}ms) - marking as FAILED",
                                     it->first.toString().value(),
                                     time_since_activity,
                                     time_since_start );
                    it->second.state = RequestState::FAILED;

                    // Cancel the request in local_requests to trigger callback
                    local_requests_->onResponse( it->second.request_id, RS_TIMEOUT, {} );

                    should_remove = true;
                }
                else if ( time_since_start > ( kRequestActivityTimeoutMs * 2 ) )
                {
                    // Absolute timeout - request has been running too long regardless of activity
                    logger()->error( "Request for CID {} has exceeded maximum duration {}ms - marking as FAILED",
                                     it->first.toString().value(),
                                     time_since_start );
                    it->second.state = RequestState::FAILED;

                    // Cancel the request in local_requests to trigger callback
                    local_requests_->onResponse( it->second.request_id, RS_TIMEOUT, {} );

                    should_remove = true;
                }
            }

            if ( should_remove )
            {
                it = tracked_requests_.erase( it );
            }
            else
            {
                ++it;
            }
        }
        // Reschedule the next cleanup
        scheduler_->schedule(
            [weak_this = weak_from_this()]()
            {
                auto self = weak_this.lock();
                if ( self )
                {
                    self->cleanupOldRequests();
                }
            },
            kCleanupIntervalMs );
    }

    IPFS::outcome::result<Graphsync::RequestState> GraphsyncImpl::getRequestState( const CID &root_cid ) const
    {
        std::lock_guard<std::mutex> lock( requested_cids_mutex_ );

        auto it = tracked_requests_.find( root_cid );
        if ( it == tracked_requests_.end() )
        {
            // Request not found
            return IPFS::outcome::failure( Error::REQUEST_NOT_FOUND );
        }

        return it->second.state;
    }

    boost::optional<GraphsyncImpl::RequestInfo> GraphsyncImpl::getRequestInfo( const CID &root_cid ) const
    {
        std::lock_guard<std::mutex> lock( requested_cids_mutex_ );

        auto it = tracked_requests_.find( root_cid );
        if ( it == tracked_requests_.end() )
        {
            return boost::none;
        }

        auto now = scheduler_->now();
        return RequestInfo{ it->second.state,
                            it->second.request_id,
                            now - it->second.start_time,
                            now - it->second.last_activity_time };
    }
}
