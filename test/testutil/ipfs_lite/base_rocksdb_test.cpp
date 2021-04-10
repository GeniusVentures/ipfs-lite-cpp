
#include "testutil/ipfs_lite/base_rocksdb_test.hpp"

namespace test {

  Baserocksdb_Test::Baserocksdb_Test(const fs::path &path)
      : BaseFS_Test(path) {}

  void Baserocksdb_Test::open() {
    rocksdb::Options options;
    options.create_if_missing = true;

    auto r = rocksdb::create(getPathString(), options);
    if (!r) {
      throw std::invalid_argument(r.error().message());
    }

    db_ = std::move(r.value());
    ASSERT_TRUE(db_) << "Baserocksdb_Test: db is nullptr";
  }

  void Baserocksdb_Test::SetUp() {
    open();
  }

  void Baserocksdb_Test::TearDown() {
     clear();
  }
}  // namespace test
