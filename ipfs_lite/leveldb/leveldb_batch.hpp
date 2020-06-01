
#ifndef CPP_IPFS_LITE_LEVELDB_BATCH_HPP
#define CPP_IPFS_LITE_LEVELDB_BATCH_HPP

#include <leveldb/write_batch.h>
#include "ipfs-lite/leveldb/leveldb.hpp"

namespace sgns::ipfs_lite {

  /**
   * @brief Class that is used to implement efficient bulk (batch) modifications
   * of the Map.
   */
  class LevelDB::Batch : public BufferBatch {
   public:
    explicit Batch(LevelDB &db);

    outcome::result<void> put(const Buffer &key, const Buffer &value) override;
    outcome::result<void> put(const Buffer &key, Buffer &&value) override;

    outcome::result<void> remove(const Buffer &key) override;

    outcome::result<void> commit() override;

    void clear() override;

   private:
    LevelDB &db_;
    leveldb::WriteBatch batch_;
  };

}  // namespace sgns::ipfs_lite

#endif  // CPP_IPFS_LITE_LEVELDB_BATCH_HPP
