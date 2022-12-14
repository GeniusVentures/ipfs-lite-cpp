#include "ipfs_lite/ipfs/impl/datastore_rocksdb.hpp"

#include "ipfs_lite/rocksdb/rocksdb_error.hpp"

namespace sgns::ipfs_lite::ipfs {
  namespace {
    /**
     * @brief convenience function to encode value
     * @param value key value to encode
     * @return encoded value as Buffer
     */
    inline outcome::result<common::Buffer> encode(const CID &value) {
      OUTCOME_TRY((auto &&, encoded), value.toBytes());
      return common::Buffer(std::move(encoded));
    }
  }  // namespace

  RocksdbDatastore::RocksdbDatastore(std::shared_ptr<rocksdb> rocksdb)
      : rocksdb_{std::move(rocksdb)} {
    BOOST_ASSERT_MSG(rocksdb_ != nullptr, "rocksdb argument is nullptr");
  }

  outcome::result<std::shared_ptr<RocksdbDatastore>> RocksdbDatastore::create(
      std::string_view rocksdb_directory, rocksdb::Options options) {
    OUTCOME_TRY((auto &&, rocksdb), rocksdb::create(rocksdb_directory, options));

    return std::make_shared<RocksdbDatastore>(std::move(rocksdb));
  }

  outcome::result<bool> RocksdbDatastore::contains(const CID &key) const {
    OUTCOME_TRY((auto &&, encoded_key), encode(key));
    return rocksdb_->contains(encoded_key);
  }

  outcome::result<void> RocksdbDatastore::set(const CID &key, Value value) {
    // TODO(turuslan): FIL-117 maybe check value hash matches cid
    OUTCOME_TRY((auto &&, encoded_key), encode(key));
    return rocksdb_->put(encoded_key, common::Buffer(std::move(value)));
  }

  outcome::result<RocksdbDatastore::Value> RocksdbDatastore::get(
      const CID &key) const {
    OUTCOME_TRY((auto &&, encoded_key), encode(key));
    auto res = rocksdb_->get(encoded_key);
    if (res.has_error() && res.error() == sgns::ipfs_lite::rocksdbError::NOT_FOUND)
      return sgns::ipfs_lite::ipfs::IpfsDatastoreError::NOT_FOUND;
    return res;
  }

  outcome::result<void> RocksdbDatastore::remove(const CID &key) {
    OUTCOME_TRY((auto &&, encoded_key), encode(key));
    return rocksdb_->remove(encoded_key);
  }

}  // namespace sgns::ipfs_lite::ipfs
