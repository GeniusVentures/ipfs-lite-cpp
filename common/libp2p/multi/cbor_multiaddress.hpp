#ifndef CPP_IPFS_LITE_COMMON_LIBP2P_MULTI_CBOR_MULTIADDRESS_HPP
#define CPP_IPFS_LITE_COMMON_LIBP2P_MULTI_CBOR_MULTIADDRESS_HPP

#include <libp2p/multi/multiaddress.hpp>

#include "codec/cbor/streams_annotation.hpp"

using libp2p::multi::Multiaddress;

namespace sgns::codec::cbor {
  /// Default value of Multiaddress for CBOR stream decoder
  template <>
  inline Multiaddress kDefaultT<Multiaddress>() {
    return Multiaddress::create("/ip4/0.0.0.1/udp/1").value();
  };
}  // namespace sgns::codec::cbor

namespace libp2p::multi {

  CBOR_ENCODE(Multiaddress, ma) {
    return s << ma.getBytesAddress();
  }

  CBOR_DECODE(Multiaddress, ma) {
    std::vector<uint8_t> bytes;
    s >> bytes;
    _OUTCOME_EXCEPT_2(_UNIQUE_NAME(_r),created, Multiaddress::create(bytes));
    ma = std::move(created);
    return s;
  }

}  // namespace libp2p::multi

#endif  // CPP_IPFS_LITE_COMMON_LIBP2P_MULTI_CBOR_MULTIADDRESS_HPP
