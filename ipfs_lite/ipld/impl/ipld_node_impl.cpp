
#include "ipfs_lite/ipld/impl/ipld_node_impl.hpp"

#include "ipfs_lite/ipld/impl/ipld_node_decoder_pb.hpp"
#include "ipfs_lite/ipld/impl/protobuf/ipld_node.pb.h"

using protobuf::ipld::node::PBLink;
using protobuf::ipld::node::PBNode;

namespace sgns::ipfs_lite::ipld {

  const CID &IPLDNodeImpl::getCID() const {
    return getIPLDBlock().cid;
  }

  const IPLDNode::Buffer &IPLDNodeImpl::getRawBytes() const {
    return getIPLDBlock().bytes;
  }

  size_t IPLDNodeImpl::size() const {
    return getRawBytes().size() + child_nodes_size_;
  }

  void IPLDNodeImpl::assign(common::Buffer input) {
    content_ = std::move(input);
    ipld_block_ =
        boost::none;  // Need to recalculate CID after changing Node content
  }

  const common::Buffer &IPLDNodeImpl::content() const {
    return content_;
  }

  IPFS::outcome::result<void> IPLDNodeImpl::addChild(
      const std::string &name, std::shared_ptr<const IPLDNode> node) {
    IPLDLinkImpl link{node->getCID(), name, node->size()};
    links_.emplace(name, std::move(link));
    ipld_block_ =
        boost::none;  // Need to recalculate CID after adding link to child node
    child_nodes_size_ += node->size();
    return IPFS::outcome::success();
  }

  IPFS::outcome::result<std::reference_wrapper<const IPLDLink>> IPLDNodeImpl::getLink(
      const std::string &name) const {
    if (auto index = links_.find(name); index != links_.end()) {
      return index->second;
    }
    return IPLDNodeError::LINK_NOT_FOUND;
  }

  void IPLDNodeImpl::removeLink(const std::string &link_name) {
    if (auto index = links_.find(link_name); index != links_.end()) {
      child_nodes_size_ -= index->second.getSize();
      links_.erase(index);
    }
  }

  void IPLDNodeImpl::addLink(const IPLDLink &link) {
    auto &link_impl = dynamic_cast<const IPLDLinkImpl &>(link);
    links_.emplace(link.getName(), link_impl);
  }

  std::vector<std::reference_wrapper<const IPLDLink>> IPLDNodeImpl::getLinks()
      const {
    std::vector<std::reference_wrapper<const IPLDLink>> link_refs{};
    for (const auto &link : links_) {
      link_refs.emplace_back(link.second);
    }
    return link_refs;
  }

  IPLDNode::Buffer IPLDNodeImpl::serialize() const {
    return Buffer{IPLDNodeEncoderPB::encode(content_, links_)};
  }

  std::shared_ptr<IPLDNode> IPLDNodeImpl::createFromString(
      const std::string &content) {
    std::vector<uint8_t> data{content.begin(), content.end()};
    auto node = std::make_shared<IPLDNodeImpl>();
    node->assign(common::Buffer{data});
    return node;
  }

  IPFS::outcome::result<std::shared_ptr<IPLDNode>> IPLDNodeImpl::createFromRawBytes(
      gsl::span<const uint8_t> input) {
    IPLDNodeDecoderPB decoder;
    if (auto result = decoder.decode(input); result.has_error()) {
      return result.error();
    }
    auto node = createFromString(decoder.getContent());
    for (size_t i = 0; i < decoder.getLinksCount(); ++i) {
      std::vector<uint8_t> link_cid_bytes{decoder.getLinkCID(i).begin(),
                                          decoder.getLinkCID(i).end()};
      OUTCOME_TRY((auto &&, link_cid), CID::fromBytes(link_cid_bytes));
      IPLDLinkImpl link{
          std::move(link_cid), decoder.getLinkName(i), decoder.getLinkSize(i)};
      node->addLink(link);
    }
    return node;
  }

  const IPLDBlock &IPLDNodeImpl::getIPLDBlock() const {
    if (!ipld_block_) {
      ipld_block_ = IPLDBlock::create(*this);
    }
    return ipld_block_.value();
  }
}  // namespace sgns::ipfs_lite::ipld

OUTCOME_CPP_DEFINE_CATEGORY_3(sgns::ipfs_lite::ipld, IPLDNodeError, e) {
  using sgns::ipfs_lite::ipld::IPLDNodeError;
  switch (e) {
    case (IPLDNodeError::LINK_NOT_FOUND):
      return "MerkleDAG Node: link not exist";
    case (IPLDNodeError::INVALID_RAW_DATA):
      return "MerkleDAG Node: failed to deserialize from incorrect raw bytes";
  }
  return "MerkleDAG Node: unknown error";
}
