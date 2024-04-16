
#include "ipfs_lite/rocksdb/rocksdb_batch.hpp"

#include "ipfs_lite/rocksdb/rocksdb_util.hpp"

namespace sgns::ipfs_lite {

  rocksdb::Batch::Batch(rocksdb &db) : db_(db) {}

  IPFS::outcome::result<void> rocksdb::Batch::put(const Buffer &key,
                                            const Buffer &value) {
    batch_.Put(make_slice(key), make_slice(value));
    return IPFS::outcome::success();
  }

  IPFS::outcome::result<void> rocksdb::Batch::put(const Buffer &key,
                                            Buffer &&value) {
    return put(key, static_cast<const Buffer&>(value));
  }

  IPFS::outcome::result<void> rocksdb::Batch::remove(const Buffer &key) {
    batch_.Delete(make_slice(key));
    return IPFS::outcome::success();
  }

  IPFS::outcome::result<void> rocksdb::Batch::commit() {
    auto status = db_.db_->Write(db_.wo_, &batch_);
    if (status.ok()) {
      return IPFS::outcome::success();
    }

    return error_as_result<void>(status, db_.logger_);
  }

  void rocksdb::Batch::clear() {
    batch_.Clear();
  }

}  // namespace sgns::ipfs_lite
