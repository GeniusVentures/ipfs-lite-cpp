
#ifndef CPP_IPFS_LITE_LEVELDB_CURSOR_HPP
#define CPP_IPFS_LITE_LEVELDB_CURSOR_HPP

#include <leveldb/iterator.h>
#include "ipfs-lite/leveldb/leveldb.hpp"

namespace sgns::ipfs_lite {

  /**
   * @brief Instance of cursor can be used as bidirectional iterator over
   * key-value bindings of the Map.
   */
  class LevelDB::Cursor : public BufferMapCursor {
   public:
    ~Cursor() override = default;

    explicit Cursor(std::shared_ptr<leveldb::Iterator> it);

    void seekToFirst() override;

    void seek(const Buffer &key) override;

    void seekToLast() override;

    bool isValid() const override;

    void next() override;

    void prev() override;

    Buffer key() const override;

    Buffer value() const override;

   private:
    std::shared_ptr<leveldb::Iterator> i_;
  };

}  // namespace sgns::ipfs_lite

#endif  // CPP_IPFS_LITE_LEVELDB_CURSOR_HPP
