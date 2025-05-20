#ifndef CPP_IPFS_LITE_CODEC_UVARINT_HPP
#define CPP_IPFS_LITE_CODEC_UVARINT_HPP

#include <gsl/span>
#include <libp2p/multi/uvarint.hpp>

#include "common/outcome.hpp"

namespace sgns::codec::uvarint {
  template <auto error, typename T = uint64_t>
  IPFS::outcome::result<T> read(gsl::span<const uint8_t> &input) {
    auto value = libp2p::multi::UVarint::create(input);
    if (!value) {
      return error;
    }
    input = input.subspan(value->size());
    return static_cast<T>(value->toUInt64());
  }

  template <auto error_length, auto error_data>
  IPFS::outcome::result<gsl::span<const uint8_t>> readBytes(gsl::span<const uint8_t> &input) {
    OUTCOME_TRY((auto &&, size), read<error_length>(input));
    if (input.size() < static_cast<ptrdiff_t>(size)) {
      return error_data;
    }
    auto result = input.subspan(0, size);
    input = input.subspan(size);
    return result;
  }
}  // namespace sgns::codec::uvarint

#endif  // CPP_IPFS_LITE_CODEC_UVARINT_HPP
