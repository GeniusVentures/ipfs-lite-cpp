#ifndef IPFS_LITE_IPLD_NODE_HPP
#define IPFS_LITE_IPLD_NODE_HPP

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <optional>

#include "common/buffer.hpp"
#include "common/outcome.hpp"

#include "ipfs_lite/ipld/impl/ipld_node_encoder_pb.hpp"
#include "ipfs_lite/ipld/ipld_block.hpp"
#include "ipfs_lite/ipld/ipld_link.hpp"

namespace sgns::ipfs_lite::ipld {
  /**
   * @interface MerkleDAG service node
   */
  class IPLDNode {
   protected:
    using Buffer = common::Buffer;

   public:
    ~IPLDNode() = default;

    /**
     * @brief Get node's CID
     * @return node's CID
     */
    const CID &getCID() const;

    /**
     * @brief Get serialized node's content
     * @note This method is similar with Node::serialize, but uses internal
     * cache for each query
     * @return node raw bytes
     */
    const Buffer &getRawBytes() const;

    /**
     * @brief Total size of the data including the total sizes of references
     * @return Cumulative size in bytes
     */
    size_t size() const;

    /**
     * @brief Assign Node's content
     * @param input - data bytes
     * @return operation result
     */
    void assign(Buffer input);

    /**
     * @brief Get Node data
     * @return content bytes
     */
    const Buffer &content() const {
        return content_;
    }

    /**
     * @brief Add link to the child node
     * @param name - name of the child node
     * @param node - child object to link
     * @return operation result
     */
    IPFS::outcome::result<void> addChild(
        const std::string &name, const IPLDNode& node);

    /**
     * @brief Get particular link to the child node
     * @param name - id of the link
     * @return Requested link of error, if link not found
     */
    IPFS::outcome::result<std::reference_wrapper<const IPLDLink>> getLink(
        const std::string &name) const;

    /**
     * @brief Remove link to the child node
     * @param name - name of the child node
     * @return operation result
     */
    void removeLink(const std::string &name);

    /**
     * @brief Insert link to the child node
     * @param link - object to add
     */
    void addLink(IPLDLink link);

    /**
     * @brief Get links to first-level child nodes
     * @return References to node links
     */
    std::vector<std::reference_wrapper<const IPLDLink>> getLinks()
        const;

    /**
     * @brief Serialize Node
     * @return raw bytes
     */
    Buffer serialize() const;

    static std::shared_ptr<IPLDNode> createFromString(
        const std::string &content);

    static IPFS::outcome::result<std::shared_ptr<IPLDNode>> createFromRawBytes(
        gsl::span<const uint8_t> input);

    const IPLDBlock &getIPLDBlock() const;

   private:
    common::Buffer content_;
    std::map<std::string, IPLDLink> links_;
    IPLDNodeEncoderPB pb_node_codec_;
    size_t child_nodes_size_{};
    mutable std::optional<IPLDBlock> ipld_block_;
  };

  /**
   * @class Possible Node errors
   */
  enum class IPLDNodeError { LINK_NOT_FOUND = 1, INVALID_RAW_DATA };

  template <>
  inline IPLDType IPLDBlock::getType<IPLDNode>() {
    return {IPLDType::Version::V0,
            IPLDType::Content::DAG_PB,
            IPLDType::Hash::sha256};
  }

  template <>
  inline common::Buffer IPLDBlock::serialize<IPLDNode>(
      const IPLDNode &entity) {
    return entity.serialize();
  }
}  // namespace sgns::ipfs_lite::ipld

OUTCOME_HPP_DECLARE_ERROR_2(sgns::ipfs_lite::ipld, IPLDNodeError)

#endif
