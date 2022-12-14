
#include "merkledag_bridge_impl.hpp"

#include <cassert>

#include "ipfs_lite/ipfs/merkledag/merkledag_service.hpp"

namespace sgns::ipfs_lite::ipfs::graphsync {

  std::shared_ptr<MerkleDagBridge> MerkleDagBridge::create(
      std::shared_ptr<merkledag::MerkleDagService> service) {
    return std::make_shared<MerkleDagBridgeImpl>(std::move(service));
  }

  MerkleDagBridgeImpl::MerkleDagBridgeImpl(
      std::shared_ptr<merkledag::MerkleDagService> service)
      : service_(std::move(service)) {
    assert(service_);
  }

  outcome::result<size_t> MerkleDagBridgeImpl::select(
      const CID &root_cid,
      gsl::span<const uint8_t> selector,
      std::function<bool(const CID &, const common::Buffer &)> handler) const {
    auto internal_handler =
        [&handler](std::shared_ptr<const ipld::IPLDNode> node) -> bool {
      return handler(node->getCID(), node->getRawBytes());
    };

    if (selector.empty()) {
      OUTCOME_TRY((auto &&, node), service_->getNode(root_cid));
      internal_handler(node);
      return 1;
    }

    // TODO(???): change MerkleDAG service to accept CID instead of bytes
    OUTCOME_TRY((auto &&, cid_encoded), root_cid.toBytes());
    return service_->select(cid_encoded, selector, internal_handler);
  }

}  // namespace sgns::ipfs_lite::ipfs::graphsync
