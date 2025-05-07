
#ifndef CPP_IPFS_LITE_GRAPHSYNC_LOCAL_REQUESTS_HPP
#define CPP_IPFS_LITE_GRAPHSYNC_LOCAL_REQUESTS_HPP

#include <map>

#include <libp2p/protocol/common/scheduler.hpp>

#include "network/marshalling/request_builder.hpp"

namespace sgns::ipfs_lite::ipfs::graphsync {
  struct StatusHolder {
    ResponseStatusCode status = RS_AWAITING_STATUS;
    size_t holder_id = 0;
    
    StatusHolder(size_t id) : holder_id(id) {}
  };
  class RequestIdGenerator {
    public:
    RequestIdGenerator() : next_holder_id_(0) {}
     RequestId next() {
       return ++counter_;
     }
    // Register a status for a remote request, returns shared_ptr to status holder
    std::shared_ptr<StatusHolder> registerStatus(RequestId request_id) {
      std::lock_guard<std::mutex> lock(mutex_);
      
      size_t holder_id = next_holder_id_++;
      auto holder = std::make_shared<StatusHolder>(holder_id);
      
      auto& holders = request_holders_[request_id];
      holders.push_back(holder);
      
      return holder;
    }
    
    // Remove a status holder for a request
    void removeStatus(RequestId request_id, std::shared_ptr<StatusHolder> holder) {
      std::lock_guard<std::mutex> lock(mutex_);
      
      auto it = request_holders_.find(request_id);
      if (it == request_holders_.end()) {
        return;
      }
      
      auto& holders = it->second;
      for (auto it = holders.begin(); it != holders.end(); ++it) {
        if (*it == holder) {
          holders.erase(it);
          break;
        }
      }
      
      if (holders.empty()) {
        request_holders_.erase(request_id);
      }
    }
    
    // Check if all statuses for a request are NOT_FOUND
    bool allStatusesNotFound(RequestId request_id) {
      std::lock_guard<std::mutex> lock(mutex_);
      
      auto it = request_holders_.find(request_id);
      if (it == request_holders_.end() || it->second.empty()) {
        return false;
      }
      
      for (const auto& holder : it->second) {
        if (holder->status != RS_NOT_FOUND) {
          return false;
        }
      }
      
      return true;
    }
    
    // Remove all tracking for a request
    void removeRequest(RequestId request_id) {
      std::lock_guard<std::mutex> lock(mutex_);
      request_holders_.erase(request_id);
    }
    private:
     
    std::atomic<RequestId> counter_{0};  // start at 1 (zero is reserved)
    // ID for status holders
    std::atomic<size_t> next_holder_id_;

    // Mutex for protecting the tracking map
    std::mutex mutex_;
      
    // Map of request IDs to their status vectors
    std::unordered_map<RequestId, std::vector<std::shared_ptr<StatusHolder>>> request_holders_;
   };

  /// Local requests module for graphsync, manages requests made by this host
  class LocalRequests : public Subscription::Source {
   public:
    /// Context of a new request
    struct NewRequest {
      /// RAII subscription object, see libp2p
      Subscription subscription;

      /// Request ID
      RequestId request_id = 0;

      /// Serialized request body to be sent to the wire
      SharedData body;
    };

    /// LocalRequests->Graphsync feedback interface
    using CancelRequestFn = std::function<void(RequestId, SharedData)>;

    /// Ctor.
    /// \param scheduler scheduler
    /// \param cancel_fn feedback to the core component
    explicit LocalRequests(
        std::shared_ptr<libp2p::protocol::Scheduler> scheduler,
        CancelRequestFn cancel_fn,
        std::shared_ptr<RequestIdGenerator> generator);

    /// Non-network part of Graphsync's makeRequest implementation.
    /// Creates a new request and NewRequest fields
    /// \param root_cid Root CID of the request
    /// \param selector IPLD selector
    /// \param extensions - protocol extension data
    /// \param callback A callback which keeps track of request progress
    /// \return request context, including serialized body
    NewRequest newRequest(const CID &root_cid,
                          gsl::span<const uint8_t> selector,
                          const std::vector<Extension> &extensions,
                          Graphsync::RequestProgressCallback callback);

    /// Creates Subscription for rejected request callback
    /// \param callback Callback which keeps track of request progress, it will
    /// receive RS_REJECTED_LOCALLY status asynchronously
    /// \return Subscription tid=ed with callback
    Subscription newRejectedRequest(
        Graphsync::RequestProgressCallback callback);

    /// Forwards responses to requests callbacks
    /// \param request_id request ID
    /// \param status response status
    /// \param extensions - data for protocol extensions
    void onResponse(RequestId request_id,
                    ResponseStatusCode status,
                    std::vector<Extension> extensions);

    /// Cancels all requests, called during Graphsync::stop()
    void cancelAll();

    // Obtained a root cid for a given request.
    boost::optional<CID> getRequestRootCid(RequestId request_id) const;

   private:
    /// Container that tracks all local requests
    using RequestMap = std::map<RequestId, Graphsync::RequestProgressCallback>;

    /// Subscription::Source::unsubscribe override
    void unsubscribe(uint64_t ticket) override;

    /// Calls rejected requests' callback functions in async manner
    void asyncNotifyRejectedRequests();

    /// Helper fr cancelAll(), avoids reentrancy
    static void cancelAll(RequestMap &requests);

    /// Returns the next available request id
    RequestId nextRequestId();

    /// licp2p scheduler
    std::shared_ptr<libp2p::protocol::Scheduler> scheduler_;

    /// Feedback to GraphsyncImpl used to cancel requests
    CancelRequestFn cancel_fn_;

    /// All active requests
    RequestMap active_requests_;

    /// All rejected requests
    RequestMap rejected_requests_;

    /// Wire protocol requests builder (and serializer)
    RequestBuilder request_builder_;

    /// Indicates that rejected requests will be closed asynchronously in the
    /// next cycle
    bool rejected_notify_scheduled_ = false;

    /// Current request id, incremented
    RequestId current_request_id_ = 0;

    /// Current rejected id, always negative and decremented
    RequestId current_rejected_request_id_ = 0;

    std::shared_ptr<RequestIdGenerator> id_generator_;

    std::unordered_map<RequestId, CID> request_to_cid_;
  };
}  // namespace sgns::ipfs_lite::ipfs::graphsync

#endif  // CPP_IPFS_LITE_GRAPHSYNC_LOCAL_REQUESTS_HPP
