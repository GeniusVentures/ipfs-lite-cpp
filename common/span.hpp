#ifndef CPP_IPFS_LITE_COMMON_SPAN_HPP
#define CPP_IPFS_LITE_COMMON_SPAN_HPP

#include <gsl/span>

namespace sgns::common::span {
  template <typename To, typename From>
  constexpr auto cast(gsl::span<From> span) {
    static_assert(sizeof(To) == 1);
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    return gsl::make_span(reinterpret_cast<To *>(span.data()), span.size());
  }

  constexpr auto cbytes(gsl::span<const char> span) {
    return cast<const uint8_t>(span);
  }

  constexpr auto cstring(gsl::span<const uint8_t> span) {
    return cast<const char>(span);
  }

  constexpr auto string(gsl::span<uint8_t> span) {
    return cast<char>(span);
  }
}  // namespace sgns::common::span

#endif  // CPP_IPFS_LITE_COMMON_SPAN_HPP
