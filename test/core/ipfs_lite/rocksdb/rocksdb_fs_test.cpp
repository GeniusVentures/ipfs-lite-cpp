
#include <gtest/gtest.h>

#include <boost/filesystem.hpp>

#include "ipfs_lite/rocksdb/rocksdb.hpp"
#include "ipfs_lite/rocksdb/rocksdb_error.hpp"
#include "testutil/outcome.hpp"
#include "testutil/ipfs_lite/base_fs_test.hpp"

namespace ipfs = sgns::ipfs_lite;
namespace fs = boost::filesystem;

struct rocksdb_Open : public test::BaseFS_Test {
  rocksdb_Open() : test::BaseFS_Test("sgns_rocksdb_open") {}
};

/**
 * @given options with disabled option `create_if_missing`
 * @when open database
 * @then database can not be opened (since there is no db already)
 */
TEST_F(rocksdb_Open, OpenNonExistingDB) {
  ipfs::rocksdb::Options options;
  options.create_if_missing = false;  // intentionally

  auto r = ipfs::rocksdb::create(getPathString(), options);
  EXPECT_FALSE(r);
  EXPECT_EQ(r.error(), ipfs::rocksdbError::INVALID_ARGUMENT);
}

/**
 * @given options with enable option `create_if_missing`
 * @when open database
 * @then database is opened
 */
TEST_F(rocksdb_Open, OpenExistingDB) {
  rocksdb::Options options;
  options.create_if_missing = true;  // intentionally

  EXPECT_OUTCOME_TRUE_2(db, ipfs::rocksdb::create(getPathString(), options));
  EXPECT_TRUE(db) << "db is nullptr";

  boost::filesystem::path p(getPathString());
  EXPECT_TRUE(fs::exists(p));
}
