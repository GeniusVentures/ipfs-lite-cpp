#ifndef IPFS_LITE_IPLD_BLOCK_HPP
#define IPFS_LITE_IPLD_BLOCK_HPP

#include <vector>

#include "codec/cbor/cbor.hpp"
#include "common/buffer.hpp"
#include "crypto/hasher/hasher.hpp"
#include "primitives/cid/cid.hpp"

namespace sgns::ipfs_lite::ipld {

  /**
   * @struct IPLD block CID and content properties
   */
  struct IPLDType {
    using Version = CID::Version;
    using Content = libp2p::multi::MulticodecType::Code;
    using Hash = libp2p::multi::HashType;

    Version cid_version;
    Content content_type;
    Hash hash_type;
  };

  /**
   * @struct IPLD block: CID and serialized bytes
   */
  struct IPLDBlock {
    CID cid;
    common::Buffer bytes;

    /**
     * @brief Create IPLD block from supported entity
     * @tparam T - type of entity
     * @param entity - source object
     * @return constructed block
     */
    template <class T>
    static IPLDBlock create(const T &entity) {
      IPLDType params = IPLDBlock::getType<T>();
      common::Buffer bytes = IPLDBlock::serialize(entity);
      auto multihash = crypto::IPFSHasher::calculate(params.hash_type, bytes);
      return IPLDBlock{CID{params.cid_version, params.content_type, multihash},
                       bytes};
    }

    /**
     * @brief Get default IPLD block params:
     * @note This method should be specialized for each IPLD entity,
     *       which uses another types
     * @tparam T - type of IPLD entity
     * @return IPLD block params
     */
    template <class T>
    static IPLDType getType() {
      return {IPLDType::Version::V1,
              IPLDType::Content::DAG_CBOR,
              IPLDType::Hash::blake2b_256};
    }

    /**
     * @brief CBOR serialize IPLD entity
     * @note This method should be specialized for each IPLD entity,
     *       which uses another serialization codec
     * @tparam T - type of the entity
     * @param entity - object to serialize
     * @return serialized bytes
     */
    template <class T>
    static common::Buffer serialize(const T &entity) {
      auto data = codec::cbor::encode(entity);
      BOOST_ASSERT(data.has_value());
      return std::move(data.value());
    }
  };
}  // namespace sgns::ipfs_lite::ipld

#endif
