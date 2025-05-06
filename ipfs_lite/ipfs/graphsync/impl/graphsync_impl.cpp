
#include "network/network.hpp"
#include "graphsync_impl.hpp"

#include <cassert>

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
namespace sgns::ipfs_lite::ipfs::graphsync {
  /// Selector that matches current node
  common::Buffer kSelectorMatcher{0xa1, 0x61, 0x2e, 0xa0};

  GraphsyncImpl::GraphsyncImpl(
      std::shared_ptr<libp2p::Host> host,
      std::shared_ptr<libp2p::protocol::Scheduler> scheduler,
      std::shared_ptr<Network> network,
      std::shared_ptr<RequestIdGenerator> generator)
      : scheduler_(scheduler),
        network_(network),
        local_requests_(std::make_shared<LocalRequests>(
            std::move(scheduler),
            [this](RequestId request_id, SharedData body) {
              cancelLocalRequest(request_id, std::move(body));
            }, generator)) {
              scheduler_->schedule(kCleanupIntervalMs, [weak_this = weak_from_this()]() {
                auto self = weak_this.lock();
                if (self) {
                    self->cleanupOldRequests();
                }
              });
            }

  GraphsyncImpl::~GraphsyncImpl() {
    doStop();
  }

  void GraphsyncImpl::start(std::shared_ptr<MerkleDagBridge> dag,
                            Graphsync::BlockCallback callback) {
    assert(dag);
    assert(callback);

    network_->start(shared_from_this());
    dag_ = std::move(dag);
    block_cb_ = std::move(callback);
    started_ = true;
  }

  void GraphsyncImpl::stop() {
    doStop();
  }

  void GraphsyncImpl::doStop() {
    if (started_) {
      started_ = false;
      block_cb_ = Graphsync::BlockCallback{};
      dag_.reset();
      network_->stop(shared_from_this());
      logger()->trace("{}: Stopping all", __FUNCTION__);
      tracked_requests_.clear();
      
      local_requests_->cancelAll();
    }
  }

  Subscription GraphsyncImpl::makeRequest(
      const libp2p::peer::PeerId &peer,
      boost::optional<std::vector<libp2p::multi::Multiaddress>> address,
      const CID &root_cid,
      gsl::span<const uint8_t> selector,
      const std::vector<Extension> &extensions,
      RequestProgressCallback callback) {
    if (!started_ || !network_->canSendRequest(peer)) {
      logger()->trace("makeRequest: rejecting request to peer {}",
                      peer.toBase58().substr(46));
      return local_requests_->newRejectedRequest(std::move(callback));
    }

    if (selector.empty()) {
      selector = kSelectorMatcher;
    }

    std::lock_guard<std::mutex> lock(requested_cids_mutex_);
    auto it = tracked_requests_.find(root_cid);
    
    if (it != tracked_requests_.end() && it->second.state == RequestState::IN_PROGRESS) {
        // Only reject if there's an in-progress request
        logger()->trace("makeRequest: already in progress for {}", 
            root_cid.toString().value());   
        return local_requests_->newRejectedRequest(std::move(callback));  
    }

    auto newRequest = local_requests_->newRequest(
        root_cid, selector, extensions, std::move(callback));

    // Update or insert the tracking info
    tracked_requests_[root_cid] = {RequestState::IN_PROGRESS, newRequest.request_id};

    if (newRequest.request_id > 0) {
      assert(newRequest.body);
      assert(!newRequest.body->empty());

      logger()->trace("makeRequest: sending request to peer {}",
                      peer.toBase58().substr(46));

      network_->makeRequest(peer,
                            std::move(address),
                            newRequest.request_id,
                            std::move(newRequest.body));
    }

    return std::move(newRequest.subscription);
  }

  void GraphsyncImpl::onResponse(const PeerId &peer,
                                 int request_id,
                                 ResponseStatusCode status,
                                 std::vector<Extension> extensions) {
    // TODO peer ratings according to status

    if (!started_) {
      return;
    }
    if (isTerminal(status)) {
        auto root_cid = local_requests_->getRequestRootCid(request_id);
        if (root_cid) {
            std::lock_guard<std::mutex> lock(requested_cids_mutex_);
            auto it = tracked_requests_.find(root_cid.value());
            if (it != tracked_requests_.end()) {
                // Update the state based on status
                if (status == RS_FULL_CONTENT) {
                    it->second.state = RequestState::COMPLETED;
                } else {
                    it->second.state = RequestState::FAILED;
                }
                logger()->trace("Request {} for CID {} completed with status {}", 
                          request_id, root_cid.value().toString().value(), 
                          statusCodeToString(status));
            }
        }
    }
    local_requests_->onResponse(request_id, status, std::move(extensions));
  }

  void GraphsyncImpl::onBlock(const PeerId &from,
                              const CID &root_cid,
                              const CID &cid,
                              common::Buffer data) {
    // TODO peer ratings according to status
    logger()->trace("Got a block for CID {}", cid.toString().value());
    if (!started_) {
      logger()->error("Got a block, but Graphsync Not Started");
      return;
    }
    std::lock_guard<std::mutex> lock(requested_cids_mutex_);
    if (tracked_requests_.find(root_cid) == tracked_requests_.end()) {
        logger()->error("Got a block, but we're not waiting for this root cid {} to cid{}", 
                     root_cid.toString().value(), cid.toString().value());
        return;
    }
    logger()->trace("Block callback for CID {}", cid.toString().value());
    block_cb_(std::move(cid), std::move(data));
  }

  void GraphsyncImpl::onRemoteRequest(const PeerId &from,
                                      Message::Request request) {
    // TODO make this asynchronous

    bool send_response = true;

    auto data_handler = [&](const CID &cid,
                            const common::Buffer &data) -> bool {
      bool data_present = !data.empty();

      if (data_present) {
        if (!network_->addBlockToResponse(from, request.id, cid, data)) {
          send_response = false;
          return false;
        }
      }

      return true;
    };

    auto select_res =
        dag_->select(request.root_cid, request.selector, data_handler);

    if (!send_response) {
      // ignore response due to network side
      return;
    }

    ResponseStatusCode status = RS_REQUEST_FAILED;
    if (select_res) {
      if (select_res.value() > 0) {
        status = RS_FULL_CONTENT;
      } else {
        status = RS_NOT_FOUND;
      }
    }

    network_->sendResponse(from, request.id, status, request.extensions);
  }

  void GraphsyncImpl::cancelLocalRequest(RequestId request_id,
                                         SharedData body) {
    network_->cancelRequest(request_id, std::move(body));
  }

  void GraphsyncImpl::cleanupOldRequests() {
    std::lock_guard<std::mutex> lock(requested_cids_mutex_);
    for (auto it = tracked_requests_.begin(); it != tracked_requests_.end();) {
        // Only remove COMPLETED or FAILED requests
        if (it->second.state != RequestState::IN_PROGRESS) {
            it = tracked_requests_.erase(it);
        } else {
            ++it;
        }
    }
          // Reschedule the next cleanup
    scheduler_->schedule(kCleanupIntervalMs, [weak_this = weak_from_this()]() {
      auto self = weak_this.lock();
      if (self) {
          self->cleanupOldRequests();
      }
    });
  }

  IPFS::outcome::result<Graphsync::RequestState> GraphsyncImpl::getRequestState(const CID &root_cid) const {
    std::lock_guard<std::mutex> lock(requested_cids_mutex_);
    
    auto it = tracked_requests_.find(root_cid);
    if (it == tracked_requests_.end()) {
      // Request not found
      return IPFS::outcome::failure(Error::REQUEST_NOT_FOUND);
    }
    
    return it->second.state;
  }

}  // namespace sgns::ipfs_lite::ipfs::graphsync
