
#ifndef CPP_IPFS_LITE_GRAPHSYNC_MESSAGE_HPP
#define CPP_IPFS_LITE_GRAPHSYNC_MESSAGE_HPP

#include "ipfs_lite/ipfs/graphsync/impl/common.hpp"

namespace sgns::ipfs_lite::ipfs::graphsync {

  /// Wire protocol message
  struct Message {
    /// Graphsync request
    struct Request {
      /// Requester-unique id, int32_t
      RequestId id = 0;

      /// Root CID
      CID root_cid;

      /// Selector string
      common::Buffer selector;

      /// aux information for other protocols extensions
      std::vector<Extension> extensions;

      /// request priority: not used at the moment
      int32_t priority = 1;

      /// flag which cancels previous request with the same id
      bool cancel = false;
    };

    /// Graphsync response
    struct Response {
      /// Request id
      RequestId id = 0;

      /// Status code
      ResponseStatusCode status = RS_NOT_FOUND;

      /// aux information for other protocols extensions
      std::vector<Extension> extensions;
    };

    /// This request list includes *all* requests from the peer, replacing
    /// existing outstanding requests
    bool complete_request_list = false;

    /// The list of requests
    std::vector<Request> requests;

    /// The list of responses
    std::vector<Response> responses;

    /// Blocks related to the responses, as cid->data pairs
    std::vector<std::pair<CID, common::Buffer>> data;
  };
}  // namespace sgns::ipfs_lite::ipfs::graphsync

#endif  // CPP_IPFS_LITE_GRAPHSYNC_MESSAGE_HPP
