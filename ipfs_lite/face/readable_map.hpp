
#ifndef CPP_IPFS_LITE_READABLE_MAP_HPP
#define CPP_IPFS_LITE_READABLE_MAP_HPP

#include "common/outcome.hpp"
#include "ipfs_lite/face/map_cursor.hpp"

namespace sgns::ipfs_lite::face {

  /**
   * @brief An abstraction over read-only MAP.
   * @tparam K key type
   * @tparam V value type
   */
  template <typename K, typename V>
  struct ReadableMap {
    virtual ~ReadableMap() = default;

    /**
     * @brief Get value by key
     * @param key K
     * @return V
     */
    virtual IPFS::outcome::result<V> get(const K &key) const = 0;

    /**
     * @brief Returns true if given key-value binding exists in the storage.
     * @param key K
     * @return true if key has value, false otherwise.
     */
    virtual bool contains(const K &key) const = 0;
  };

}  // namespace sgns::ipfs_lite::face

#endif  // CPP_IPFS_LITE_WRITEABLE_KEY_VALUE_HPP
