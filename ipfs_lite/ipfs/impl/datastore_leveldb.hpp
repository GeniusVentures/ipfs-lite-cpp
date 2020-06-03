
#ifndef CPP_IPFS_LITE_IPFS_IMPL_DATASTORE_LEVELDB_HPP
#define CPP_IPFS_LITE_IPFS_IMPL_DATASTORE_LEVELDB_HPP

#include <memory>

#include "common/outcome.hpp"
#include "ipfs_lite/ipfs/datastore.hpp"
#include "ipfs_lite/leveldb/leveldb.hpp"

namespace sgns::ipfs_lite::ipfs {

  /**
   * @class LeveldbDatastore IpfsDatastore implementation based on LevelDB
   * database wrapper
   */
  class LeveldbDatastore : public IpfsDatastore {
   public:
    /**
     * @brief constructor
     * @param leveldb shared pointer to leveldb instance
     */
    explicit LeveldbDatastore(std::shared_ptr<LevelDB> leveldb);

    ~LeveldbDatastore() override = default;

    /**
     * @brief creates LeveldbDatastore instance
     * @param leveldb_directory path to leveldb directory
     * @param options leveldb database options
     * @return shared pointer to instance
     */
    static outcome::result<std::shared_ptr<LeveldbDatastore>> create(
        std::string_view leveldb_directory, leveldb::Options options);

    outcome::result<bool> contains(const CID &key) const override;

    outcome::result<void> set(const CID &key, Value value) override;

    outcome::result<Value> get(const CID &key) const override;

    outcome::result<void> remove(const CID &key) override;

   private:
    std::shared_ptr<LevelDB> leveldb_;  ///< underlying db wrapper
  };

}  // namespace sgns::ipfs_lite::ipfs

#endif  // CPP_IPFS_LITE_IPFS_IMPL_DATASTORE_LEVELDB_HPP
