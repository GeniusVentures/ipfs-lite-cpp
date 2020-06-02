
#ifndef CPP_IPFS_LITE_ITERABLE_MAP_HPP
#define CPP_IPFS_LITE_ITERABLE_MAP_HPP

#include <memory>

#include "ipfs_lite/face/map_cursor.hpp"

namespace sgns::ipfs_lite::face {

  /**
   * @brief An abstraction of a key-value map, that is iterable.
   * @tparam K key type
   * @tparam V value type
   */
  template <typename K, typename V>
  struct IterableMap {
    virtual ~IterableMap() = default;

    /**
     * @brief Returns new key-value iterator.
     * @return kv iterator
     */
    virtual std::unique_ptr<MapCursor<K, V>> cursor() = 0;
  };

}  // namespace sgns::ipfs_lite::face

#endif  // CPP_IPFS_LITE_ITERABLE_MAP_HPP
