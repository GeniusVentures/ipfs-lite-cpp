
#include "ipfs_lite/rocksdb/rocksdb.hpp"

#include <utility>

#include "ipfs_lite/rocksdb/rocksdb_batch.hpp"
#include "ipfs_lite/rocksdb/rocksdb_cursor.hpp"
#include "ipfs_lite/rocksdb/rocksdb_util.hpp"

namespace sgns::ipfs_lite {

  IPFS::outcome::result<std::shared_ptr<rocksdb>> rocksdb::create(
      std::string_view path, rocksdb::Options options) {
    rocksdb::DB *db = nullptr;
    auto status = rocksdb::DB::Open(options, path.data(), &db);
    if (status.ok()) {
      auto l = std::make_shared<rocksdb>();
      l->db_ = std::shared_ptr<rocksdb::DB>(db);
      return std::move(l); // clang 6.0.1 issue
    }

    return error_as_result<std::shared_ptr<rocksdb>>(status);
  }

  IPFS::outcome::result<std::shared_ptr<rocksdb>> rocksdb::create(const std::shared_ptr<DB>& db)
  {
    if (db == nullptr)
    {
      return error_as_result<std::shared_ptr<rocksdb>>(rocksdb::Status(rocksdb::Status::PathNotFound()));
    }

    auto l = std::make_unique<rocksdb>();
    l->db_ = db;
    return std::move(l); // clang 6.0.1 issue
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
  }

}  // namespace sgns::ipfs_lite
