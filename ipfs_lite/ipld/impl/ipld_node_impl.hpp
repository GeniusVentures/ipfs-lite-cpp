
#ifndef IPFS_LITE_IPLD_NODE_IMPL_HPP
#define IPFS_LITE_IPLD_NODE_IMPL_HPP

#include <map>
#include <memory>
#include <string_view>
#include <vector>

#include <boost/optional.hpp>
#include "ipfs_lite/ipld/impl/ipld_link_impl.hpp"
#include "ipfs_lite/ipld/impl/ipld_node_encoder_pb.hpp"
#include "ipfs_lite/ipld/ipld_node.hpp"

namespace sgns::ipfs_lite::ipld {
  class IPLDNodeImpl : public IPLDNode {
   public:
    const CID &getCID() const override;

    const Buffer &getRawBytes() const override;

    size_t size() const override;

    void assign(common::Buffer input) override;

    const common::Buffer &content() const override;

    IPFS::outcome::result<void> addChild(
        const std::string &name, std::shared_ptr<const IPLDNode> node) override;

    IPFS::outcome::result<std::reference_wrapper<const IPLDLink>> getLink(
        const std::string &name) const override;

    void removeLink(const std::string &name) override;

    void addLink(const IPLDLink &link) override;

    std::vector<std::reference_wrapper<const IPLDLink>> getLinks()
        const override;

    Buffer serialize() const override;

    static std::shared_ptr<IPLDNode> createFromString(
        const std::string &content);

    static IPFS::outcome::result<std::shared_ptr<IPLDNode>> createFromRawBytes(
        gsl::span<const uint8_t> input);

    const IPLDBlock &getIPLDBlock() const;

   private:
    common::Buffer content_;
    std::map<std::string, IPLDLinkImpl> links_;
    IPLDNodeEncoderPB pb_node_codec_;
    size_t child_nodes_size_{};
    mutable boost::optional<IPLDBlock> ipld_block_;
  };

  template <>
  inline IPLDType IPLDBlock::getType<IPLDNodeImpl>() {
    return {IPLDType::Version::V0,
            IPLDType::Content::DAG_PB,
            IPLDType::Hash::sha256};
  }

  template <>
  inline common::Buffer IPLDBlock::serialize<IPLDNodeImpl>(
      const IPLDNodeImpl &entity) {
    return entity.serialize();
  }
}  // namespace sgns::ipfs_lite::ipld

#endif
