#pragma once

#include <chrono>

#include <boost/asio/io_context.hpp>
#include <libp2p/basic/scheduler.hpp>
#include <libp2p/outcome/outcome.hpp>

#include "network/network_fwd.hpp"

namespace libp2p
{
    // libp2p host interface forward declaration
    struct Host;
}

namespace sgns::ipfs_lite::ipfs::graphsync
{

    class LocalRequests;
    class Network;
    class RequestIdGenerator;

    /// Core graphsync component. The central module
    class GraphsyncImpl : public Graphsync,
                          public std::enable_shared_from_this<GraphsyncImpl>,
                          public PeerToGraphsyncFeedback
    {
    public:
        enum class Error: uint8_t
        {
            REQUEST_NOT_FOUND = 0, ///< The requested CID wasn't found
        };

        struct RequestTrackingInfo
        {
            RequestState              state;
            RequestId                 request_id;
            std::chrono::milliseconds last_activity_time; // Last time we received data or response for this request
            std::chrono::milliseconds start_time;         // When the request was initiated
        };

        /// Ctor.
        /// \param host libp2p host object
        /// \param scheduler libp2p scheduler
        GraphsyncImpl( std::shared_ptr<libp2p::Host>             host,
                       std::shared_ptr<libp2p::basic::Scheduler> scheduler,
                       std::shared_ptr<Network>                  network,
                       std::shared_ptr<RequestIdGenerator>       generator,
                       std::shared_ptr<boost::asio::io_context>  io_context );

        ~GraphsyncImpl() override;

        IPFS::outcome::result<Graphsync::RequestState> getRequestState( const CID &root_cid ) const override;

        /// Get detailed request information for debugging
        struct RequestInfo
        {
            RequestState              state;
            RequestId                 request_id;
            std::chrono::milliseconds time_since_start;
            std::chrono::milliseconds time_since_activity;
        };

        boost::optional<RequestInfo> getRequestInfo( const CID &root_cid ) const;

    private:
        /// Callback from LocalRequests module. Cancels a request made by this host
        /// \param request_id request ID
        /// \param body request wire protocol data
        void cancelLocalRequest( RequestId request_id, SharedData body );

        // Graphsync interface overrides
        void         start( std::shared_ptr<merkledag::MerkleDagService> service, BlockCallback callback ) override;
        void         stop() override;
        Subscription makeRequest( const libp2p::peer::PeerId                               &peer,
                                  boost::optional<std::vector<libp2p::multi::Multiaddress>> address,
                                  const CID                                                &root_cid,
                                  gsl::span<const uint8_t>                                  selector,
                                  const std::vector<Extension>                             &extensions,
                                  RequestProgressCallback                                   callback ) override;

        // PeerToGraphsyncFeedback interface overrides
        void onResponse( const PeerId          &peer,
                         RequestId              request_id,
                         ResponseStatusCode     status,
                         std::vector<Extension> extensions ) override;
        void onBlock( const PeerId &from, const CID &root_cid, const CID &cid, common::Buffer data ) override;
        void onRemoteRequest( const PeerId &from, Message::Request request ) override;

        // Clear from list of requested CIDs so we can try to request this again
        void cleanupOldRequests();

        /// NVI for stop()
        void doStop();

        std::shared_ptr<boost::asio::io_context> io_context_;
        std::shared_ptr<libp2p::basic::Scheduler> scheduler_;
        std::shared_ptr<Network> network_;
        std::shared_ptr<LocalRequests> local_requests_;
        std::shared_ptr<RequestIdGenerator> reqgenerator_;
        std::shared_ptr<merkledag::MerkleDagService> service_;
        Graphsync::BlockCallback block_cb_;
        std::unordered_map<CID, RequestTrackingInfo> tracked_requests_;
        mutable std::mutex requested_cids_mutex_;
        bool started_ = false;
    };

    constexpr std::chrono::milliseconds kCleanupIntervalMs( 120000 );

    /// Maximum time a request can be stalled without activity before being considered failed
    constexpr std::chrono::milliseconds kRequestActivityTimeoutMs( 180000 ); // 3 minutes
}
