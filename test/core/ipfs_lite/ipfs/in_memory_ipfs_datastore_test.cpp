
#include <gtest/gtest.h>

#include "ipfs_lite/ipfs/impl/in_memory_datastore.hpp"
#include "testutil/literals.hpp"
#include "testutil/outcome.hpp"

using sgns::CID;
using sgns::common::Buffer;
using sgns::ipfs_lite::ipfs::InMemoryDatastore;
using sgns::ipfs_lite::ipfs::IpfsDatastore;
using sgns::ipfs_lite::ipfs::IpfsDatastoreError;
using libp2p::multi::HashType;
using libp2p::multi::MulticodecType;
using libp2p::multi::Multihash;

class InMemoryIpfsDatastoreTest : public ::testing::Test {
 public:
  CID cid1{
      CID::Version::V1,
      MulticodecType::Code::SHA2_256,
      Multihash::create(HashType::sha256,
                        "0123456789ABCDEF0123456789ABCDEF"_unhex)
          .value()};
  CID cid2{
      CID::Version::V1,
      MulticodecType::Code::SHA2_256,
      Multihash::create(HashType::sha256,
                        "FEDCBA9876543210FEDCBA9876543210"_unhex)
          .value()};

  Buffer value{"0123456789ABCDEF0123456789ABCDEF"_unhex};

  std::shared_ptr<IpfsDatastore> datastore{
      std::make_shared<InMemoryDatastore>()};
};

/**
 * @given opened datastore, cid and a value
 * @when put cid with value into datastore @and then get value from datastore by
 * cid
 * @then all operation succeeded, obtained value is equal to original value
 */
TEST_F(InMemoryIpfsDatastoreTest, ContainsExistingTrueSuccess) {
  EXPECT_OUTCOME_TRUE_1(datastore->set(cid1, value));
  EXPECT_OUTCOME_EQ(datastore->contains(cid1), true);
}

/**
 * @given opened datastore, 2 different instances of CID and a value
 * @when put cid1 with value into datastore and check if datastore contains cid2
 * @then all operations succeed and datastore doesn't contains cid2
 */
TEST_F(InMemoryIpfsDatastoreTest, ContainsNotExistingFalseSuccess) {
  EXPECT_OUTCOME_TRUE_1(datastore->set(cid1, value));
  EXPECT_OUTCOME_EQ(datastore->contains(cid2), false);
}

/**
 * @given opened datastore, CID instance and a value
 * @when put cid with value into datastore @and then get value by cid
 * @then all operations succeed
 */
TEST_F(InMemoryIpfsDatastoreTest, GetExistingSuccess) {
  EXPECT_OUTCOME_TRUE_1(datastore->set(cid1, value));
  EXPECT_OUTCOME_EQ(datastore->get(cid1), value);
}

/**
 * @given opened datastore, 2 different CID instances and a value
 * @when put cid1 with value into datastore @and then get value by cid2
 * @then put operation succeeds, get operation fails
 */
TEST_F(InMemoryIpfsDatastoreTest, GetNotExistingFailure) {
  EXPECT_OUTCOME_TRUE_1(datastore->set(cid1, value));
  EXPECT_OUTCOME_ERROR(IpfsDatastoreError::NOT_FOUND, datastore->get(cid2));
}

/**
 * @given opened datastore, CID instance and a value
 * @when put cid with value into datastore @and remove cid from datastore
 * @then all operations succeed and datastore doesn't contain cid anymore
 */
TEST_F(InMemoryIpfsDatastoreTest, RemoveSuccess) {
  EXPECT_OUTCOME_TRUE_1(datastore->set(cid1, value));
  EXPECT_OUTCOME_TRUE_1(datastore->remove(cid1));
  EXPECT_OUTCOME_EQ(datastore->contains(cid1), false);
}

/**
 * @given opened datastore, 2 CID instances and a value
 * @when put cid1 with value into datastore @and remove cid2 from datastore
 * @then all operations succeed and datastore still contains cid1
 */
TEST_F(InMemoryIpfsDatastoreTest, RemoveNotExistingSuccess) {
  EXPECT_OUTCOME_TRUE_1(datastore->set(cid1, value));
  EXPECT_OUTCOME_TRUE_1(datastore->remove(cid2));
  EXPECT_OUTCOME_EQ(datastore->contains(cid1), true);
}

/** Setting same key twice succeeds */
TEST_F(InMemoryIpfsDatastoreTest, SetTwice) {
  EXPECT_OUTCOME_TRUE_1(datastore->set(cid1, value));
  EXPECT_OUTCOME_TRUE_1(datastore->set(cid1, value));
}
