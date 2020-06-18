#include "ipfs_lite/ipfs/graphsync/extension.hpp"

#include <gtest/gtest.h>
#include "ipfs_lite/ipfs/graphsync/impl/common.hpp"
#include "testutil/outcome.hpp"

using sgns::CID;
using sgns::ipfs_lite::ipfs::graphsync::decodeDontSendCids;
using sgns::ipfs_lite::ipfs::graphsync::decodeResponseMetadata;
using sgns::ipfs_lite::ipfs::graphsync::encodeDontSendCids;
using sgns::ipfs_lite::ipfs::graphsync::encodeResponseMetadata;
using sgns::ipfs_lite::ipfs::graphsync::Error;
using sgns::ipfs_lite::ipfs::graphsync::Extension;
using sgns::ipfs_lite::ipfs::graphsync::ResponseMetadata;

/**
 * Test encode and decode ResponseMetadata extension
 */
TEST(GraphsyncExtensions, ResponseMetadataEncodeDecode) {
  ResponseMetadata response_metadata{};
  Extension extension = encodeResponseMetadata(response_metadata);
  EXPECT_OUTCOME_TRUE(decoded, decodeResponseMetadata(extension));
  EXPECT_EQ(decoded, response_metadata);
}

/**
 * @given wrong protocol name
 * @when try decode
 * @then error occured
 */
TEST(GraphsyncExtensions, ResponseMetadataWrongName) {
  // Extension extension{.name = "wrong_name"};
  Extension extension{/*.name = */"wrong_name"};
  EXPECT_OUTCOME_ERROR(Error::MESSAGE_PARSE_ERROR,
                       decodeResponseMetadata(extension));
}

/**
 * Test encode and decode do-not-send-cids extension
 */
TEST(GraphsyncExtensions, DontSendCidsEncodeDecode) {
  std::vector<CID> cids;
  Extension extension = encodeDontSendCids(cids);
  EXPECT_OUTCOME_TRUE(decoded, decodeDontSendCids(extension));
  EXPECT_EQ(decoded, std::set<CID>(cids.begin(), cids.end()));
}

/**
 * @given wrong protocol name
 * @when try decode
 * @then error occured
 */
TEST(GraphsyncExtensions, DontSendCidsWrongName) {
  // Extension extension{.name = "wrong_name"};
  Extension extension{/*.name = */"wrong_name"};
  EXPECT_OUTCOME_ERROR(Error::MESSAGE_PARSE_ERROR,
                       decodeDontSendCids(extension));
}
