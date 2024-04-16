
#include "ipfs_lite/ipfs/merkledag/impl/leaf_impl.hpp"

namespace sgns::ipfs_lite::ipfs::merkledag {
  LeafImpl::LeafImpl(common::Buffer data)
      : content_{std::move(data)} {}

  const common::Buffer &LeafImpl::content() const {
    return content_;
  }

  size_t LeafImpl::count() const {
    return children_.size();
  }

  IPFS::outcome::result<std::reference_wrapper<const Leaf>> LeafImpl::subLeaf(
      std::string_view name) const {
    if (auto iter = children_.find(name); iter != children_.end()) {
      return iter->second;
    }
    return LeafError::LEAF_NOT_FOUND;
  }

  std::vector<std::string_view> LeafImpl::getSubLeafNames() const {
    std::vector<std::string_view> names;
    for (const auto &child : children_) {
      names.push_back(child.first);
    }
    return names;
  }

  IPFS::outcome::result<void> LeafImpl::insertSubLeaf(std::string name,
                                                 LeafImpl children) {
    auto result = children_.emplace(std::move(name), std::move(children));
    if (result.second) {
      return IPFS::outcome::success();
    }
    return LeafError::DUPLICATE_LEAF;
  }
}  // namespace sgns::ipfs_lite::ipfs::merkledag

OUTCOME_CPP_DEFINE_CATEGORY_3(sgns::ipfs_lite::ipfs::merkledag, LeafError, e) {
  using sgns::ipfs_lite::ipfs::merkledag::LeafError;
  switch (e) {
    case (LeafError::LEAF_NOT_FOUND):
      return "MerkleDAG leaf: children leaf not found";
    case (LeafError::DUPLICATE_LEAF):
      return "MerkleDAG leaf: duplicate leaf name";
  }
  return "MerkleDAG leaf: unknown error";
}
