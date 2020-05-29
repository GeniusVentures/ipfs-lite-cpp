#ifndef CPP_IPFS_LITE_CODEC_UVARINT_HPP
#define CPP_IPFS_LITE_CODEC_UVARINT_HPP

#include <gsl/span>
#include <libp2p/multi/uvarint.hpp>

#include "common/outcome.hpp"

namespace ipfs_lite::codec::uvarint {
  using Input = gsl::span<const uint8_t>;

  template <auto error, typename T = uint64_t>
  outcome::result<T> read(Input &input) {
    auto value = libp2p::multi::UVarint::create(input);
    if (!value) {
      return error;
    }
    input = input.subspan(value->size());
    return static_cast<T>(value->toUInt64());
  }

  template <auto error_length, auto error_data>
  outcome::result<Input> readBytes(Input &input) {
    OUTCOME_TRY(size, read<error_length>(input));
    if (input.size() < static_cast<ptrdiff_t>(size)) {
      return error_data;
    }
    auto result = input.subspan(0, size);
    input = input.subspan(size);
    return result;
  }
}  // namespace ipfs_lite::codec::uvarint

#endif  // CPP_IPFS_LITE_CODEC_UVARINT_HPP
