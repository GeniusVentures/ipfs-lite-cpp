
#include <gtest/gtest.h>

#include <array>
#include <boost/filesystem.hpp>
#include <exception>

#include "ipfs_lite/rocksdb/rocksdb.hpp"
#include "ipfs_lite/rocksdb/rocksdb_error.hpp"
#include "testutil/outcome.hpp"
#include "testutil/ipfs_lite/base_rocksdb_test.hpp"

using namespace sgns::ipfs_lite;
namespace fs = boost::filesystem;

struct rocksdb_Integration_Test : public test::Baserocksdb_Test {
  rocksdb_Integration_Test()
      : test::Baserocksdb_Test("sgns_rocksdb_integration_test") {}
  
  //Added for error fix when finished testing.
  void TearDown() override {}

  Buffer key_{1, 3, 3, 7};
  Buffer value_{1, 2, 3};
  
};

/**
 * @given opened database, with {key}
 * @when read {key}
 * @then {value} is correct
 */
TEST_F(rocksdb_Integration_Test, Put_Get) {
  logger_->warn("1. start testing....{}", db_);
  EXPECT_OUTCOME_TRUE_1(db_->put(key_, value_));
  logger_->warn("2. start testing....key_ {}, value_ = {}", key_, value_);
  EXPECT_TRUE(db_->contains(key_));
  logger_->warn("3. start testing....key_ {}, value_ = {}", key_, value_);
  EXPECT_OUTCOME_TRUE_2(val, db_->get(key_));
  logger_->warn("4. start testing....key_ {}, value_ = {}", key_, value_);
  EXPECT_EQ(val, value_);
  logger_->warn("5. start testing....key_ {}, value_ = {}", key_, value_);
}

/**
 * @given empty db
 * @when read {key}
 * @then get "not found"
 */
TEST_F(rocksdb_Integration_Test, Get_NonExistent) {
  EXPECT_FALSE(db_->contains(key_));
  EXPECT_OUTCOME_TRUE_1(db_->remove(key_));
  auto r = db_->get(key_);
  EXPECT_FALSE(r);
  EXPECT_EQ(r.error().value(), (int)rocksdbError::NOT_FOUND);
}

/**
 * @given database with [(i,i) for i in range(6)]
 * @when create batch and write KVs
 * @then data is written only after commit
 */

TEST_F(rocksdb_Integration_Test, WriteBatch) {
  std::list<Buffer> keys{{0}, {1}, {2}, {3}, {4}, {5}};
  Buffer toBeRemoved = {3};
  std::list<Buffer> expected{{0}, {1}, {2}, {4}, {5}};

  auto batch = db_->batch();
  ASSERT_TRUE(batch);

  for (const auto &item : keys) {
    EXPECT_OUTCOME_TRUE_1(batch->put(item, item));
    EXPECT_FALSE(db_->contains(item));
  }
  EXPECT_OUTCOME_TRUE_1(batch->remove(toBeRemoved));
  EXPECT_OUTCOME_TRUE_1(batch->commit());

  for (const auto &item : expected) {
    EXPECT_TRUE(db_->contains(item));
    EXPECT_OUTCOME_TRUE_2(val, db_->get(item));
    EXPECT_EQ(val, item);
  }

  EXPECT_FALSE(db_->contains(toBeRemoved));
}

/**
 * @given database with [(i,i) for i in range(100)]
 * @when iterate over kv pairs forward and backward
 * @then we iterate over all items
 */

TEST_F(rocksdb_Integration_Test, Iterator) {
  const size_t size = 100;
  // 100 buffers of size 1 each; 0..99
  std::list<Buffer> keys;
  for (size_t i = 0; i < size; i++) {
    keys.emplace_back(1, i);
  }

  for (const auto &item : keys) {
    EXPECT_OUTCOME_TRUE_1(db_->put(item, item));
  }

  std::array<size_t, size> counter{};

  logger_->warn("forward iteration");
  auto it = db_->cursor();
  for (it->seekToFirst(); it->isValid(); it->next()) {
    auto k = it->key();
    auto v = it->value();
    EXPECT_EQ(k, v);

    logger_->info("key: {}, value: {}", k.toHex(), v.toHex());

    EXPECT_GE(k[0], 0);
    EXPECT_LT(k[0], size);
    EXPECT_GT(k.size(), 0);

    counter[k[0]]++;
  }

  for (size_t i = 0; i < size; i++) {
    EXPECT_EQ(counter[i], 1);
  }

  logger_->warn("backward iteration");
  size_t c = 0;
  uint8_t index = 0xf;
  Buffer seekTo{index};
  // seek to `index` element
  for (it->seek(seekTo); it->isValid(); it->prev()) {
    auto k = it->key();
    auto v = it->value();
    EXPECT_EQ(k, v);

    logger_->info("key: {}, value: {}", k.toHex(), v.toHex());

    c++;
  }

  EXPECT_FALSE(it->isValid());
  EXPECT_EQ(c, index + 1);
}
