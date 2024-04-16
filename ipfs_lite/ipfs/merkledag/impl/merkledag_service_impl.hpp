#ifndef CPP_IPFS_LITE_IPFS_MERKLEDAG_SERVICE_IMPL_HPP
#define CPP_IPFS_LITE_IPFS_MERKLEDAG_SERVICE_IMPL_HPP

#include <memory>

#include "ipfs_lite/ipfs/datastore.hpp"
#include "ipfs_lite/ipfs/merkledag/impl/leaf_impl.hpp"
#include "ipfs_lite/ipfs/merkledag/merkledag_service.hpp"
#include "ipfs_lite/ipld/ipld_link.hpp"

namespace sgns::ipfs_lite::ipfs::merkledag {
  using ipld::IPLDLink;

  class MerkleDagServiceImpl : public MerkleDagService {
   public:
    /**
     * @brief Construct service
     * @param service - underlying block service
     */
    explicit MerkleDagServiceImpl(std::shared_ptr<IpfsDatastore> service);

    IPFS::outcome::result<void> addNode(
        std::shared_ptr<const IPLDNode> node) override;

    IPFS::outcome::result<std::shared_ptr<IPLDNode>> getNode(
        const CID &cid) const override;

    IPFS::outcome::result<void> removeNode(const CID &cid) override;

    IPFS::outcome::result<size_t> select(
        gsl::span<const uint8_t> root_cid,
        gsl::span<const uint8_t> selector,
        std::function<bool(std::shared_ptr<const IPLDNode> node)> handler)
        const override;

    IPFS::outcome::result<std::shared_ptr<Leaf>> fetchGraph(
        const CID &cid) const override;

    IPFS::outcome::result<std::shared_ptr<Leaf>> fetchGraphOnDepth(
        const CID &cid, uint64_t depth) const override;

    /**
     * @brief Fetches graph starting from specified node
     * @param cid - root node id, for which we should extract child nodes
     * @param max_depth - e.g. "1" means "Fetch only root node with all
     * children, but without children of their children"
     * @return operation result
     */
    static IPFS::outcome::result<std::shared_ptr<Leaf>> fetchGraphOnDepth(
        std::function<IPFS::outcome::result<std::shared_ptr<IPLDNode>>(const CID& cid)> nodeGetter,
        const CID& cid, std::optional<uint64_t> depth);

   private:
    std::shared_ptr<IpfsDatastore> block_service_;

    /**
     * @brief Fetch graph internal recursive implementation
     * @param nodeGetter a function that returns a node with specific id
     * @param root - leaf, for which we should extract child nodes
     * @param links - links to the child nodes of this leaf
     * @param max_depth - e.g. "1" means "Fetch only root node with all
     * children, but without children of their children"
     * @param current_depth - value of the depth during current operation
     * @return operation result
     */
    static IPFS::outcome::result<void> buildGraph(
        std::function<IPFS::outcome::result<std::shared_ptr<IPLDNode>>(const CID& cid)> nodeGetter,
        const std::shared_ptr<LeafImpl> &root,
        const std::vector<std::reference_wrapper<const IPLDLink>> &links,
        std::optional<size_t> max_depth,
        size_t current_depth = 0);
  };
}  // namespace sgns::ipfs_lite::ipfs::merkledag

#endif
