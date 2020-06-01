#ifndef CPP_FILECOIN_BUFFER_MAP_HPP
#define CPP_FILECOIN_BUFFER_MAP_HPP

/**
 * This file contains:
 *  - BufferMap - contains key-value bindings of Buffers
 *  - PersistentBufferMap - stores key-value bindings on filesystem or remote
 * connection.
 */

#include <gsl/span>

#include "common/buffer.hpp"
#include "ipfs_lite/face/generic_map.hpp"
#include "ipfs_lite/face/persistent_map.hpp"
#include "ipfs_lite/face/write_batch.hpp"

namespace fc::storage {

  using Buffer = common::Buffer;

  using BufferMap = face::GenericMap<Buffer, Buffer>;

  using BufferBatch = face::WriteBatch<Buffer, Buffer>;

  using PersistentBufferMap = face::PersistentMap<Buffer, Buffer>;

  using BufferMapCursor = face::MapCursor<Buffer, Buffer>;

}  // namespace fc::storage

#endif  // CPP_FILECOIN_BUFFER_MAP_HPP
