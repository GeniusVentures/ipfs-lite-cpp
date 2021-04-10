
#ifndef CPP_IPFS_LITE_BASE_rocksdb_TEST_HPP
#define CPP_IPFS_LITE_BASE_rocksdb_TEST_HPP

#include "testutil/ipfs_lite/base_fs_test.hpp"

#include "ipfs_lite/rocksdb/rocksdb.hpp"

namespace test {

  struct Baserocksdb_Test : public BaseFS_Test {
    using rocksdb = sgns::ipfs_lite::rocksdb;

    explicit Baserocksdb_Test(const fs::path &path);

    void open();

    void SetUp() override;

    void TearDown() override;

    std::shared_ptr<rocksdb> db_;
  };

}  // namespace test

#endif  // CPP_IPFS_LITE_BASE_rocksdb_TEST_HPP
