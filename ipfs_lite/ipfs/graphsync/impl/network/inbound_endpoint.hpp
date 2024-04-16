
#ifndef CPP_IPFS_LITE_GRAPHSYNC_INBOUND_ENDPOINT_HPP
#define CPP_IPFS_LITE_GRAPHSYNC_INBOUND_ENDPOINT_HPP

#include "marshalling/response_builder.hpp"
#include "network_fwd.hpp"

namespace sgns::ipfs_lite::ipfs::graphsync {

  class MessageQueue;

  /// Graphsync endpoint used to send responses to peer
  class InboundEndpoint {
   public:
    InboundEndpoint(const InboundEndpoint &) = delete;
    InboundEndpoint &operator=(const InboundEndpoint &) = delete;

    /// Ctor.
    /// \param queue Queues raw messages, dependency object
    explicit InboundEndpoint(std::shared_ptr<MessageQueue> queue);

    /// Adds data block to response. Doesn't send unless sending partial
    /// response is needed
    /// \param cid CID of data block
    /// \param data Raw data
    IPFS::outcome::result<void> addBlockToResponse(RequestId request_id,
                                             const CID &cid,
                                             const common::Buffer &data);
    /// Sends response via message queue
    /// \param request_id id of request
    /// \param status status code
    /// \param extensions - data for protocol extensions
    IPFS::outcome::result<void> sendResponse(
        RequestId request_id,
        ResponseStatusCode status,
        const std::vector<Extension> &extensions);

   private:
    /// Enqueues partial response when protobuf message size exceeds protocol
    /// limits
    /// \param request_id request id
    /// \return result of queue operation
    IPFS::outcome::result<void> sendPartialResponse(RequestId request_id);

    /// Max pending bytes in message queue.
    const size_t max_pending_bytes_;

    /// Queues of network messages
    std::shared_ptr<MessageQueue> queue_;

    /// Wire protocol response messages builder
    ResponseBuilder response_builder_;
  };

}  // namespace sgns::ipfs_lite::ipfs::graphsync

#endif  // CPP_IPFS_LITE_GRAPHSYNC_INBOUND_ENDPOINT_HPP
