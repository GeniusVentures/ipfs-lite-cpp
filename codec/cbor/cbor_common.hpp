#ifndef CPP_IPFS_LITE_CORE_CODEC_CBOR_CBOR_COMMON_HPP
#define CPP_IPFS_LITE_CORE_CODEC_CBOR_CBOR_COMMON_HPP

#include <cstdint>

#include <boost/optional.hpp>

#include "codec/cbor/cbor_errors.hpp"
#include "common/outcome.hpp"
#include "primitives/cid/cid.hpp"

namespace ipfs_lite::codec::cbor {
  constexpr uint64_t kCidTag = 42;
}  // namespace ipfs_lite::codec::cbor

#endif  // CPP_IPFS_LITE_CORE_CODEC_CBOR_CBOR_COMMON_HPP
