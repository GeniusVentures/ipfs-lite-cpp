
#ifndef IPFS_LITE_TEST_TESTUTIL_LITERALS_HPP
#define IPFS_LITE_TEST_TESTUTIL_LITERALS_HPP

#include "common/blob.hpp"
#include "common/hexutil.hpp"
#include "primitives/cid/cid.hpp"

inline std::vector<uint8_t> operator""_unhex(const char *c, size_t s) {
  return sgns::common::unhex(std::string_view(c, s)).value();
}

inline sgns::common::Hash256 operator""_hash256(const char *c, size_t s) {
  return sgns::common::Hash256::fromHex(std::string_view(c, s)).value();
}

inline sgns::common::Blob<20> operator""_blob20(const char *c, size_t s) {
  return sgns::common::Blob<20>::fromHex(std::string_view(c, s)).value();
}

inline sgns::common::Blob<32> operator""_blob32(const char *c, size_t s) {
  return sgns::common::Blob<32>::fromHex(std::string_view(c, s)).value();
}

inline sgns::common::Blob<48> operator""_blob48(const char *c, size_t s) {
  return sgns::common::Blob<48>::fromHex(std::string_view(c, s)).value();
}

inline sgns::common::Blob<65> operator""_blob65(const char *c, size_t s) {
    return sgns::common::Blob<65>::fromHex(std::string_view(c, s)).value();
}

inline sgns::common::Blob<96> operator""_blob96(const char *c, size_t s) {
  return sgns::common::Blob<96>::fromHex(std::string_view(c, s)).value();
}

inline auto operator""_cid(const char *c, size_t s) {
  return sgns::CID::fromBytes(sgns::common::unhex(std::string_view(c, s)).value())
      .value();
}

#endif  // IPFS_LITE_TEST_TESTUTIL_LITERALS_HPP
