
#ifndef CPP_IPFS_LITE__PRIMITIVES_CID_CID_OF_CBOR_HPP
#define CPP_IPFS_LITE__PRIMITIVES_CID_CID_OF_CBOR_HPP

#include "primitives/cid/cid.hpp"
#include "codec/cbor/cbor.hpp"

namespace sgns::primitives::cid {

  /**
   * @brief helper function to calculate cid of an object
   * @tparam T object type
   * @param value object instance reference
   * @return calculated cid or error
   */
  template <class T>
  outcome::result<CID> getCidOfCbor(const T &value) {
    OUTCOME_TRY((auto &&, bytes), sgns::codec::cbor::encode(value));
    return common::getCidOf(bytes);
  }
}  // namespace sgns::primitives::cid

#endif  // CPP_IPFS_LITE__PRIMITIVES_CID_CID_OF_CBOR_HPP
