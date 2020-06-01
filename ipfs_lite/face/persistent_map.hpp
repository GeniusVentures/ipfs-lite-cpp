
#ifndef CPP_IPFS_LITE_PERSISTENT_MAP_HPP
#define CPP_IPFS_LITE_PERSISTENT_MAP_HPP

#include "storage/face/generic_map.hpp"
#include "storage/face/write_batch.hpp"

namespace sgns::ipfs_lite::face {

  /**
   * @brief An abstraction over a map accessible via filesystem or remove
   * connection. It supports batching for efficiency of modifications.
   * @tparam K key type
   * @tparam V value type
   */
  template <typename K, typename V>
  struct PersistentMap : public GenericMap<K, V> {
    /**
     * @brief Creates new Write Batch - an object, which can be used to
     * efficiently write bulk data.
     */
    virtual std::unique_ptr<WriteBatch<K, V>> batch() = 0;
  };

}  // namespace sgns::ipfs_lite::face

#endif  // CPP_IPFS_LITE_PERSISTENT_MAP_HPP
