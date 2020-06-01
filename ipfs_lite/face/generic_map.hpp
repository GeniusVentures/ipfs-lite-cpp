
#ifndef CPP_IPFS_LITE_GENERIC_MAP_HPP
#define CPP_IPFS_LITE_GENERIC_MAP_HPP

#include "storage/face/iterable_map.hpp"
#include "storage/face/readable_map.hpp"
#include "storage/face/writeable_map.hpp"

namespace sgns::ipfs_lite::face {

  /**
   * @brief An abstraction over readable, writeable, iterable key-value map.
   * @tparam K key type
   * @tparam V value type
   */
  template <typename K, typename V>
  struct GenericMap : public IterableMap<K, V>,
                      public ReadableMap<K, V>,
                      public WriteableMap<K, V> {};
}  // namespace sgns::ipfs_lite::face

#endif  // CPP_IPFS_LITE_GENERIC_MAP_HPP
