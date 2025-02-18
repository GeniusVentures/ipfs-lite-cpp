#ifndef CPP_IPFS_LITE_ROCKSDB_UTIL_HPP
#define CPP_IPFS_LITE_ROCKSDB_UTIL_HPP

#include <rocksdb/status.h>
#include <gsl/span>
#include "common/outcome.hpp"
#include "common/buffer.hpp"
#include "common/logger.hpp"
#include "ipfs_lite/rocksdb/rocksdb_error.hpp"

namespace sgns::ipfs_lite {

  template <typename T>
  inline IPFS::outcome::result<T> error_as_result(const ::ROCKSDB_NAMESPACE::Status &s) {
    if (s.IsNotFound()) {
      return rocksdbError::NOT_FOUND;
    }

    if (s.IsIOError()) {
      return rocksdbError::IO_ERROR;
    }

    if (s.IsInvalidArgument()) {
      return rocksdbError::INVALID_ARGUMENT;
    }

    if (s.IsCorruption()) {
      return rocksdbError::CORRUPTION;
    }

    if (s.IsNotSupported()) {
      return rocksdbError::NOT_SUPPORTED;
    }

    return rocksdbError::UNKNOWN;
  }

  template <typename T>
  inline IPFS::outcome::result<T> error_as_result(const ::ROCKSDB_NAMESPACE::Status &s,
                                            const common::Logger &logger) {
    // if we are checking if it exists, don't log as an error
    if (s.IsNotFound()) {
      logger->debug(s.ToString());
    } else {
      logger->error(s.ToString());
    }
    return error_as_result<T>(s);
  }

  inline ::ROCKSDB_NAMESPACE::Slice make_slice(const common::Buffer &buf) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    const auto *ptr = reinterpret_cast<const char *>(buf.data());
    size_t n = buf.size();
    return ::ROCKSDB_NAMESPACE::Slice{ptr, n};
  }

  inline gsl::span<const uint8_t> make_span(const ::ROCKSDB_NAMESPACE::Slice &s) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    const auto *ptr = reinterpret_cast<const uint8_t *>(s.data());
    return gsl::make_span(ptr, s.size());
  }

  inline common::Buffer make_buffer(const ::ROCKSDB_NAMESPACE::Slice &s) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    const auto *ptr = reinterpret_cast<const uint8_t *>(s.data());
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    return common::Buffer(ptr, ptr + s.size());
  }

}  // namespace sgns::ipfs_lite

#endif  // CPP_IPFS_LITE_rocksdb_UTIL_HPP
