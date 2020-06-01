#ifndef CPP_IPFS_LITE_WRITE_BATCH_HPP
#define CPP_IPFS_LITE_WRITE_BATCH_HPP

#include "storage/face/writeable_map.hpp"

namespace sgns::ipfs_lite::face {

  /**
   * @brief An abstraction over a storage, which can be used for batch writes
   * @tparam K key type
   * @tparam V value type
   */
  template <typename K, typename V>
  struct WriteBatch : public WriteableMap<K, V> {
    /**
     * @brief Writes batch.
     * @return error code in case of error.
     */
    virtual outcome::result<void> commit() = 0;

    /**
     * @brief Clear batch.
     */
    virtual void clear() = 0;
  };

}  // namespace sgns::ipfs_lite::face

#endif  // CPP_IPFS_LITE_WRITE_BATCH_HPP
