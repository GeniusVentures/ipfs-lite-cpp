
#ifndef IPFS_LITE_IPLD_NODE_PB_DECODER
#define IPFS_LITE_IPLD_NODE_PB_DECODER

#include <string>

#include <gsl/span>
#include "common/buffer.hpp"
#include "common/outcome.hpp"
#include "ipfs_lite/ipld/impl/protobuf/ipld_node.pb.h"

namespace sgns::ipfs_lite::ipld {
  /**
   * @class     Protobuf Node decoder
   */
  class IPLDNodeDecoderPB {
   public:
    /**
     * @brief Try to decode input bytes as Protobuf-encoded Node
     * @param input - bytes to decode
     * @return operation result
     */
    outcome::result<void> decode(gsl::span<const uint8_t> input);

    /**
     * @brief Get Node content
     * @return content data
     */
    const std::string &getContent() const;

    /**
     * @brief Get count of the children
     * @return Links num
     */
    size_t getLinksCount() const;

    /**
     * @brief Get link to the children name
     * @param index - id of the link
     * @return operation result
     */
    const std::string &getLinkName(size_t index) const;

    /**
     * @brief Get CID of the children
     * @param index - id of the link
     * @return CID bytes
     */
    const std::string &getLinkCID(size_t index) const;

    /**
     * @brief Get name of the link to the children
     * @param index - id of the link
     * @return operation result
     */
    size_t getLinkSize(size_t index) const;

   private:
    ::protobuf::ipld::node::PBNode pb_node_;
  };

  /**
   * @enum Possible PBNodeDecoder errors
   */
  enum class IPLDNodeDecoderPBError { INVALID_RAW_BYTES = 1 };
}  // namespace sgns::ipfs_lite::ipfs::merkledag

OUTCOME_HPP_DECLARE_ERROR_2(sgns::ipfs_lite::ipld, IPLDNodeDecoderPBError)

#endif
