#ifndef CPP_IPFS_LITE__COMMON_ENUM_HPP
#define CPP_IPFS_LITE__COMMON_ENUM_HPP

namespace ipfs_lite::common {

  /**
   * @brief Convert enum class value as integer
   * Usage:
   *   Given an enum class
   *   @code
   *   enum class Enumerator {
   *     one = 1,
   *     eleven = 11,
   *     hundred = 100
   *   }
   *   @nocode
   *   Can be converted as:
   *   @code
   *   int v = to_int(Enumerator::eleven); // v == 11
   *   @nocode
   *
   * @tparam Enumeration - enum type
   * @param value - to convert
   * @return integer value of enum
   */
  template <typename Enumeration>
  auto to_int(Enumeration const value) ->
      typename std::underlying_type<Enumeration>::type {
    return static_cast<typename std::underlying_type<Enumeration>::type>(value);
  }

}  // namespace ipfs_lite::common

#endif  // CPP_IPFS_LITE__COMMON_ENUM_HPP
