
#ifndef CPP_IPFS_LITE_BASE_LEVELDB_TEST_HPP
#define CPP_IPFS_LITE_BASE_LEVELDB_TEST_HPP

#include "testutil/ipfs_lite/base_fs_test.hpp"

#include "ipfs_lite/leveldb/leveldb.hpp"

namespace test {

  struct BaseLevelDB_Test : public BaseFS_Test {
    using LevelDB = sgns::ipfs_lite::LevelDB;

    explicit BaseLevelDB_Test(const fs::path &path);

    void open();

    void SetUp() override;

    void TearDown() override;

    std::shared_ptr<LevelDB> db_;
  };

}  // namespace test

#endif  // CPP_IPFS_LITE_BASE_LEVELDB_TEST_HPP
