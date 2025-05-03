
#include "network/network.hpp"
#include "graphsync_impl.hpp"

#include <cassert>

#include "local_requests.hpp"
// #include "network/network.hpp"

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
            }, generator)) {}

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

    std::lock_guard lock(requested_cids_mutex_);
    if (!requested_cids_.insert(root_cid).second) {
      logger()->trace("makeRequest: already requested {}",
        root_cid.toString().value());   
        return local_requests_->newRejectedRequest(std::move(callback));  
    }
    auto newRequest = local_requests_->newRequest(
        root_cid, selector, extensions, std::move(callback));

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
        // Look up the request to get the root CID
        auto root_cid = local_requests_->getRequestRootCid(request_id);
        if (root_cid) {
            clearRequestedCid(root_cid.value());
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
    if (requested_cids_.find(root_cid) == requested_cids_.end()) {
      logger()->error("Got a block, but we're not waiting for this root cid {} to cid{}", root_cid.toString().value(), cid.toString().value());
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

  void GraphsyncImpl::clearRequestedCid(const CID &cid) {
    std::lock_guard lock(requested_cids_mutex_);
    requested_cids_.erase(cid);
    logger()->trace("clearRequestedCid: removed {} from tracking", 
                   cid.toString().value());
  }


}  // namespace sgns::ipfs_lite::ipfs::graphsync
