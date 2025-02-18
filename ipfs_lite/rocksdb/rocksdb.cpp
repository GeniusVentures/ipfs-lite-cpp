#include "ipfs_lite/rocksdb/rocksdb.hpp"

#include <memory>
#include <utility>
#include <rocksdb/table.h>
#include <rocksdb/filter_policy.h>
#include <rocksdb/slice_transform.h>

#include "ipfs_lite/rocksdb/rocksdb_batch.hpp"
#include "ipfs_lite/rocksdb/rocksdb_cursor.hpp"
#include "ipfs_lite/rocksdb/rocksdb_util.hpp"

namespace sgns::ipfs_lite {

  IPFS::outcome::result<std::shared_ptr<rocksdb>> rocksdb::create(
      std::string_view path, const rocksdb::Options &options) {
      // Create a shared_ptr immediately to avoid manual memory management
      auto l = std::make_shared<rocksdb>();

      // Store a deep copy of options
      l->options_ = std::make_shared<Options>(options);

      // Set up bloom filter
      ::ROCKSDB_NAMESPACE::BlockBasedTableOptions table_options;
      table_options.filter_policy.reset(::ROCKSDB_NAMESPACE::NewBloomFilterPolicy(10, false));
      table_options.whole_key_filtering = true;
      l->options_->table_factory.reset(NewBlockBasedTableFactory(table_options));

      // Define a prefix extractor
      l->options_->prefix_extractor.reset(::ROCKSDB_NAMESPACE::NewCappedPrefixTransform(3));

      l->options_->info_log_level = ::ROCKSDB_NAMESPACE::InfoLogLevel::ERROR_LEVEL;
      // Configure threading environment
      l->options_->env = ::rocksdb::Env::Default();
      l->options_->env->SetBackgroundThreads(4, ::rocksdb::Env::Priority::HIGH);

      rocksdb::DB *db = nullptr;
      auto status = rocksdb::DB::Open(options, path.data(), &db);
      if (status.ok())
      {
          // Wrap DB* into a shared_ptr with a custom deleter to ensure cleanup
          l->db_ = std::shared_ptr<DB>(db);
          // Create logger
          l->logger_ = common::createLogger("rocksdb");
          return l;  // Return the shared_ptr
      }

      // Clean up manually allocated DB if Open() succeeded but logic fails
      if (db)
      {
          delete db;
      }

      // Return an error result
      return error_as_result<std::shared_ptr<rocksdb>>(status);  }

  IPFS::outcome::result<std::shared_ptr<rocksdb>> rocksdb::create(const std::shared_ptr<DB> &db) {
    if (db == nullptr)
    {
      return error_as_result<std::shared_ptr<rocksdb>>(rocksdb::Status(rocksdb::Status::PathNotFound()));
    }

    auto l = std::make_shared<rocksdb>();
    l->db_ = db;
    l->logger_ = common::createLogger("rocksdb");
    return l;
  }

  std::unique_ptr<BufferMapCursor> rocksdb::cursor() {
    auto it = std::unique_ptr<rocksdb::Iterator>(db_->NewIterator(ro_));
    return std::make_unique<Cursor>(std::move(it));
  }

  std::unique_ptr<BufferBatch> rocksdb::batch() {
    return std::make_unique<Batch>(*this);
  }

  void rocksdb::setReadOptions(rocksdb::ReadOptions ro) {
    ro_ = ro;
  }

  void rocksdb::setWriteOptions(rocksdb::WriteOptions wo) {
    wo_ = wo;
  }

  IPFS::outcome::result<Buffer> rocksdb::get(const Buffer &key) const {
    std::string value;
    auto status = db_->Get(ro_, make_slice(key), &value);
    if (status.ok()) {
      // FIXME: is it possible to avoid copying string -> Buffer?
      return Buffer{}.put(value);
    }

    return error_as_result<Buffer>(status, logger_);
  }

  bool rocksdb::contains(const Buffer &key) const {
    // here we interpret all kinds of errors as "not found".
    // is there a better way?
    return get(key).has_value();
  }

  IPFS::outcome::result<void> rocksdb::put(const Buffer &key, const Buffer &value) {
    auto status = db_->Put(wo_, make_slice(key), make_slice(value));
    if (status.ok()) {
      return IPFS::outcome::success();
    }

    return error_as_result<void>(status, logger_);
  }

  IPFS::outcome::result<void> rocksdb::put(const Buffer &key, Buffer &&value) {
    Buffer copy(std::move(value));
    return put(key, copy);
  }

  IPFS::outcome::result<void> rocksdb::remove(const Buffer &key) {
    auto status = db_->Delete(wo_, make_slice(key));
    if (status.ok()) {
      return IPFS::outcome::success();
    }

    return error_as_result<void>(status, logger_);
  }
    IPFS::outcome::result<void> rocksdb::close() {

    auto status = db_->Close();
    if (status.ok()) {
      return IPFS::outcome::success();
    }
    return IPFS::outcome::failure(boost::system::error_code{});
  }

}  // namespace sgns::ipfs_lite
