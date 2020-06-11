#ifndef CPP_IPFS_LITE_TEST_TESTUTIL_CBOR_HPP
#define CPP_IPFS_LITE_TEST_TESTUTIL_CBOR_HPP

#include "codec/cbor/cbor.hpp"
#include "common/hexutil.hpp"
#include "testutil/literals.hpp"
#include "testutil/outcome.hpp"

using sgns::common::hex_upper;

/** Check that:
 * - CBOR encoding value yields expected bytes;
 * - CBOR encoding value decoded from bytes yields same bytes;
 */
template <typename T>
void expectEncodeAndReencode(const T &value,
                             const std::vector<uint8_t> &expected_bytes) {
  EXPECT_OUTCOME_TRUE(actual_bytes, sgns::codec::cbor::encode(value));
  EXPECT_EQ(actual_bytes, expected_bytes)
      << "actual bytes: " << hex_upper(actual_bytes) << std::endl
      << "expected bytes: " << hex_upper(expected_bytes);

  EXPECT_OUTCOME_TRUE(decoded, sgns::codec::cbor::decode<T>(expected_bytes));
  EXPECT_OUTCOME_EQ(sgns::codec::cbor::encode(decoded), expected_bytes);
}

#endif  // CPP_IPFS_LITE_TEST_TESTUTIL_CBOR_HPP
