
#ifndef CPP_IPFS_LITE_IPFS_IMPL_DATASTORE_ROCKSDB_HPP
#define CPP_IPFS_LITE_IPFS_IMPL_DATASTORE_ROCKSDB_HPP

#include <memory>

#include "common/outcome.hpp"
#include "ipfs_lite/ipfs/datastore.hpp"
#include "ipfs_lite/rocksdb/rocksdb.hpp"

namespace sgns::ipfs_lite::ipfs {

  /**
   * @class RocksdbDatastore IpfsDatastore implementation based on RocksDB
   * database wrapper
   */
  class RocksdbDatastore : public IpfsDatastore {
   public:
    /**
     * @brief constructor
     * @param rocksdb shared pointer to rocksdb instance
     */
    explicit RocksdbDatastore(std::shared_ptr<rocksdb> rocksdb);

    ~RocksdbDatastore() override = default;

    /**
     * @brief creates RocksdbDatastore instance
     * @param rocksdb_directory path to rocksdb directory
     * @param options rocksdb database options
     * @return shared pointer to instance
     */
    static IPFS::outcome::result<std::shared_ptr<RocksdbDatastore>> create(
        std::string_view rocksdb_directory, rocksdb::Options options);

    IPFS::outcome::result<bool> contains(const CID &key) const override;

    IPFS::outcome::result<void> set(const CID &key, Value value) override;

    IPFS::outcome::result<Value> get(const CID &key) const override;

    IPFS::outcome::result<void> remove(const CID &key) override;

   private:
    std::shared_ptr<rocksdb> rocksdb_;  ///< underlying db wrapper
  };

}  // namespace sgns::ipfs_lite::ipfs

#endif  // CPP_IPFS_LITE_IPFS_IMPL_DATASTORE_ROCKSDB_HPP
