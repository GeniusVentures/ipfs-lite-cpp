
#ifndef CPP_IPFS_LITE_ROCKSDB_HPP
#define CPP_IPFS_LITE_ROCKSDB_HPP

#include <rocksdb/db.h>
#include <rocksdb/write_batch.h>
#include "common/logger.hpp"
#include "ipfs_lite/buffer_map.hpp"

namespace sgns::ipfs_lite {

  /**
   * @brief An implementation of PersistentBufferMap interface, which uses
   * rocksdb as underlying storage.
   */
  class rocksdb : public PersistentBufferMap {
   public:
    class Batch;
    class Cursor;

    using Options = ::ROCKSDB_NAMESPACE::Options;
    using ReadOptions = ::ROCKSDB_NAMESPACE::ReadOptions;
    using WriteOptions = ::ROCKSDB_NAMESPACE::WriteOptions;
    using Iterator = ::ROCKSDB_NAMESPACE::Iterator;
    using DB = ::ROCKSDB_NAMESPACE::DB;

    ~rocksdb() override = default;

    /**
     * @brief Factory method to create an instance of rocksdb class.
     * @param path filesystem path where database is going to be
     * @param options rocksdb options, such as caching, logging, etc.
     * @return instance of rocksdb
     */
    static outcome::result<std::shared_ptr<rocksdb>> create(
        std::string_view path, Options options = Options());

    /**
     * @brief Set read options, which are used in @see rocksdb#get
     * @param ro options
     */
    void setReadOptions(ReadOptions ro);

    /**
     * @brief Set write options, which are used in @see rocksdb#put
     * @param wo options
     */
    void setWriteOptions(WriteOptions wo);

    std::unique_ptr<BufferMapCursor> cursor() override;

    std::unique_ptr<BufferBatch> batch() override;

    outcome::result<Buffer> get(const Buffer &key) const override;

    bool contains(const Buffer &key) const override;

    outcome::result<void> put(const Buffer &key, const Buffer &value) override;

    // value will be copied, not moved, due to internal structure of rocksdb
    outcome::result<void> put(const Buffer &key, Buffer &&value) override;

    outcome::result<void> remove(const Buffer &key) override;

   private:
    std::unique_ptr<DB> db_;
    ReadOptions ro_;
    WriteOptions wo_;
    common::Logger logger_ = common::createLogger("rocksdb");
  };

}  // namespace sgns::ipfs_lite

#endif  // CPP_IPFS_LITE_ROCKSDB_HPP
