#ifndef CPP_IPFS_LITE_COMMON_LIBP2P_PEER_CBOR_PEER_ID_HPP
#define CPP_IPFS_LITE_COMMON_LIBP2P_PEER_CBOR_PEER_ID_HPP

#include <libp2p/peer/peer_id.hpp>

#include "codec/cbor/streams_annotation.hpp"
#include "common/outcome.hpp"

using libp2p::peer::PeerId;

namespace ipfs_lite::codec::cbor {
  /// Default value of PeerId for CBOR stream decoder
  template <>
  inline PeerId kDefaultT<PeerId>() {
    return PeerId::fromHash(
               libp2p::multi::Multihash::create(libp2p::multi::sha256, {})
                   .value())
        .value();
  }
}  // namespace ipfs_lite::codec::cbor

namespace libp2p::peer {

  CBOR_ENCODE(PeerId, peer) {
    return s << peer.toVector();
  }

  CBOR_DECODE(PeerId, peer) {
    std::vector<uint8_t> bytes;
    s >> bytes;
    _OUTCOME_EXCEPT_2(UNIQUE_NAME(_r), peer_id, PeerId::fromBytes(bytes));
    peer = std::move(peer_id);
    return s;
  }

}  // namespace libp2p::peer

#endif  // CPP_IPFS_LITE_COMMON_LIBP2P_PEER_CBOR_PEER_ID_HPP
