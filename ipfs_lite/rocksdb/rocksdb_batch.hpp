
#ifndef CPP_IPFS_LITE_ROCKSDB_BATCH_HPP
#define CPP_IPFS_LITE_ROCKSDB_BATCH_HPP

#include <rocksdb/write_batch.h>
#include "ipfs_lite/rocksdb/rocksdb.hpp"

namespace sgns::ipfs_lite {

  /**
   * @brief Class that is used to implement efficient bulk (batch) modifications
   * of the Map.
   */
  class rocksdb::Batch : public BufferBatch {
   public:
    using WriteBatch = ::ROCKSDB_NAMESPACE::WriteBatch;

    explicit Batch(rocksdb &db);

    outcome::result<void> put(const Buffer &key, const Buffer &value) override;
    outcome::result<void> put(const Buffer &key, Buffer &&value) override;

    outcome::result<void> remove(const Buffer &key) override;

    outcome::result<void> commit() override;

    void clear() override;

   private:
    rocksdb &db_;
    WriteBatch batch_;
  };

}  // namespace sgns::ipfs_lite

#endif  // CPP_IPFS_LITE_rocksdb_BATCH_HPP
