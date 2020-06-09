
#ifndef CPP_IPFS_LITE_MERKLEDAG_BRIDGE_IMPL_HPP
#define CPP_IPFS_LITE_MERKLEDAG_BRIDGE_IMPL_HPP

#include "ipfs_lite/ipfs/graphsync/graphsync.hpp"

namespace sgns::ipfs_lite::ipfs::graphsync {

  /// Default implementation og Graphsync->MerkleDAG bridge
  class MerkleDagBridgeImpl : public MerkleDagBridge {
   public:
    /// Ctor. called form  MerkleDagBridge::create(...)
    explicit MerkleDagBridgeImpl(
        std::shared_ptr<merkledag::MerkleDagService> service);

   private:
    // overrides MerkleDagBridge interface
    outcome::result<size_t> select(
        const CID &root_cid,
        gsl::span<const uint8_t> selector,
        std::function<bool(const CID &cid, const common::Buffer &data)> handler)
        const override;

    /// MerkleDAG service
    std::shared_ptr<merkledag::MerkleDagService> service_;
  };

}  // namespace ipfs_lite::ipfs_lite::ipfs::graphsync

#endif  // CPP_IPFS_LITE_MERKLEDAG_BRIDGE_IMPL_HPP
