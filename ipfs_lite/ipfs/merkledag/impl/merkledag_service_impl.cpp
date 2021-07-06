#include "ipfs_lite/ipfs/merkledag/impl/merkledag_service_impl.hpp"

#include <boost/assert.hpp>

#include "ipfs_lite/ipld/impl/ipld_node_impl.hpp"

namespace sgns::ipfs_lite::ipfs::merkledag {
  using ipld::IPLDNodeImpl;

  MerkleDagServiceImpl::MerkleDagServiceImpl(
      std::shared_ptr<IpfsDatastore> service)
      : block_service_{std::move(service)} {
    BOOST_ASSERT_MSG(block_service_ != nullptr,
                     "MerkleDAG service: Block service not connected");
  }

  outcome::result<void> MerkleDagServiceImpl::addNode(
      std::shared_ptr<const IPLDNode> node) {
    const common::Buffer &raw_bytes = node->getRawBytes();
    return block_service_->set(node->getCID(), raw_bytes);
  }

  outcome::result<std::shared_ptr<IPLDNode>> MerkleDagServiceImpl::getNode(
      const CID &cid) const {
    OUTCOME_TRY(content, block_service_->get(cid));
    return IPLDNodeImpl::createFromRawBytes(content);
  }

  outcome::result<void> MerkleDagServiceImpl::removeNode(const CID &cid) {
    return block_service_->remove(cid);
  }

  outcome::result<size_t> MerkleDagServiceImpl::select(
      gsl::span<const uint8_t> root_cid,
      gsl::span<const uint8_t> selector,
      std::function<bool(std::shared_ptr<const IPLDNode>)> handler) const {
    std::ignore = selector;
    OUTCOME_TRY(cid, CID::fromBytes(root_cid));
    OUTCOME_TRY(root_node, getNode(cid));
    std::vector<std::shared_ptr<const IPLDNode>> node_set{};
    node_set.emplace_back(std::move(root_node));
    const auto &links = node_set.front()->getLinks();
    for (const auto &link : links) {
      auto request = getNode(link.get().getCID());
      if (request.has_error()) return ServiceError::UNRESOLVED_LINK;
      node_set.emplace_back(std::move(request.value()));
    }
    size_t sent_count{};
    for (const auto &node : node_set) {
      ++sent_count;
      if (!handler(node)) break;
    }
    return sent_count;
  }

  outcome::result<std::shared_ptr<Leaf>> MerkleDagServiceImpl::fetchGraph(
      const CID &cid) const {
      return fetchGraphOnDepth(std::bind(&MerkleDagService::getNode, this, std::placeholders::_1), cid, {});
  }

  outcome::result<std::shared_ptr<Leaf>>
  MerkleDagServiceImpl::fetchGraphOnDepth(const CID &cid,
                                          uint64_t depth) const {
    return fetchGraphOnDepth(std::bind(&MerkleDagService::getNode, this, std::placeholders::_1), cid, depth);
  }

  outcome::result<std::shared_ptr<Leaf>> MerkleDagServiceImpl::fetchGraphOnDepth(
      std::function<outcome::result<std::shared_ptr<IPLDNode>>(const CID& cid)> nodeGetter,
      const CID& cid, std::optional<uint64_t> depth)
  {
      OUTCOME_TRY(node, nodeGetter(cid));
      auto leaf = std::make_shared<LeafImpl>(node->content());
      auto result = buildGraph(nodeGetter, leaf, node->getLinks(), depth, 0);
      if (result.has_error()) return result.error();
      return leaf;
  }

  outcome::result<void> MerkleDagServiceImpl::buildGraph(
      std::function<outcome::result<std::shared_ptr<IPLDNode>>(const CID& cid)> nodeGetter,
      const std::shared_ptr<LeafImpl> &root,
      const std::vector<std::reference_wrapper<const IPLDLink>> &links,
      std::optional<size_t> max_depth,
      size_t current_depth) {
    if (max_depth && current_depth == *max_depth) {
      return outcome::success();
    }
    for (const auto &link : links) {
      auto request = nodeGetter(link.get().getCID());
      if (request.has_error()) return ServiceError::UNRESOLVED_LINK;
      std::shared_ptr<IPLDNode> node = request.value();
      auto child_leaf = std::make_shared<LeafImpl>(node->content());
      auto build_result = buildGraph(nodeGetter,
                                     child_leaf,
                                     node->getLinks(),
                                     max_depth,
                                     ++current_depth);
      if (build_result.has_error()) {
        return build_result;
      }
      auto insert_result =
          root->insertSubLeaf(link.get().getName(), std::move(*child_leaf));
      if (insert_result.has_error()) {
        return insert_result;
      }
    }
    return outcome::success();
  }
}  // namespace sgns::ipfs_lite::ipfs::merkledag

OUTCOME_CPP_DEFINE_CATEGORY_3(sgns::ipfs_lite::ipfs::merkledag, ServiceError, e) {
  using sgns::ipfs_lite::ipfs::merkledag::ServiceError;
  switch (e) {
    case (ServiceError::UNRESOLVED_LINK):
      return "MerkleDAG service: broken link";
  }
  return "MerkleDAG Node: unknown error";
}
