

#ifndef CPP_IPFS_LITE__COMMON_MP_UTILS_HPP
#define CPP_IPFS_LITE__COMMON_MP_UTILS_HPP

#include <boost/multiprecision/cpp_int.hpp>
#include <gsl/span>

namespace sgns::common {

  std::array<uint8_t, 8> uint64_t_to_bytes(uint64_t number);

  uint64_t bytes_to_uint64_t(gsl::span<uint8_t, 8> bytes);

  std::array<uint8_t, 16> uint128_t_to_bytes(
      const boost::multiprecision::uint128_t &i);

  boost::multiprecision::uint128_t bytes_to_uint128_t(
      gsl::span<uint8_t, 16> bytes);

  std::array<uint8_t, 32> uint256_t_to_bytes(
      const boost::multiprecision::uint256_t &i);

  boost::multiprecision::uint256_t bytes_to_uint256_t(
      gsl::span<uint8_t, 32> bytes);

}  // namespace sgns::common

#endif  // CPP_IPFS_LITE__COMMON_MP_UTILS_HPP
