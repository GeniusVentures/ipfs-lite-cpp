
#ifndef CPP_IPFS_LITE__PRIMITIVES_CID_CID_OF_CBOR_HPP
#define CPP_IPFS_LITE__PRIMITIVES_CID_CID_OF_CBOR_HPP

#include "primitives/cid/cid.hpp"
#include "codec/cbor/cbor.hpp"

namespace ipfs_lite::primitives::cid {

  /**
   * @brief helper function to calculate cid of an object
   * @tparam T object type
   * @param value object instance reference
   * @return calculated cid or error
   */
  template <class T>
  outcome::result<CID> getCidOfCbor(const T &value) {
    OUTCOME_TRY(bytes, ipfs_lite::codec::cbor::encode(value));
    return common::getCidOf(bytes);
  }
}  // namespace ipfs_lite::primitives::cid

#endif  // CPP_IPFS_LITE__PRIMITIVES_CID_CID_OF_CBOR_HPP
