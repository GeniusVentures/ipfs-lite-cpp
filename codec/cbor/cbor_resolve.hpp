#ifndef CPP_IPFS_LITE__CODEC_CBOR_CBOR_RESOLVE_HPP
#define CPP_IPFS_LITE__CODEC_CBOR_CBOR_RESOLVE_HPP

#include "codec/cbor/cbor_decode_stream.hpp"

namespace sgns::codec::cbor {
  enum class CborResolveError {
    INT_KEY_EXPECTED = 1,
    KEY_NOT_FOUND,
    CONTAINER_EXPECTED,
    INT_KEY_TOO_BIG
  };

  using Path = gsl::span<const std::string>;

  outcome::result<uint64_t> parseIndex(const std::string &str);

  /** Resolves path in CBOR object to CBOR subobject */
  outcome::result<void> resolve(CborDecodeStream &stream,
                                const std::string &part);
}  // namespace sgns::codec::cbor

OUTCOME_HPP_DECLARE_ERROR_2(sgns::codec::cbor, CborResolveError);

#endif  // CPP_IPFS_LITE__CODEC_CBOR_CBOR_RESOLVE_HPP
