#include "ipfs_lite/rocksdb/rocksdb_cursor.hpp"

#include "ipfs_lite/rocksdb/rocksdb_util.hpp"

namespace sgns::ipfs_lite {

  rocksdb::Cursor::Cursor(std::shared_ptr<rocksdb::Iterator> it)
      : i_(std::move(it)) {}

  void rocksdb::Cursor::seekToFirst() {
    i_->SeekToFirst();
  }

  void rocksdb::Cursor::seek(const Buffer &key) {
    i_->Seek(make_slice(key));
  }

  void rocksdb::Cursor::seekToLast() {
    i_->SeekToLast();
  }

  bool rocksdb::Cursor::isValid() const {
    return i_->Valid();
  }

  void rocksdb::Cursor::next() {
    i_->Next();
  }

  void rocksdb::Cursor::prev() {
    i_->Prev();
  }

  Buffer rocksdb::Cursor::key() const {
    return make_buffer(i_->key());
  }

  Buffer rocksdb::Cursor::value() const {
    return make_buffer(i_->value());
  }

}  // namespace sgns::ipfs_lite
