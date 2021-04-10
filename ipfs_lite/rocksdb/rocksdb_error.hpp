
#ifndef CPP_IPFS_LITE_ROCKSDB_ERROR_HPP
#define CPP_IPFS_LITE_ROCKSDB_ERROR_HPP

#include "common/outcome.hpp"

namespace sgns::ipfs_lite {

  /**
   * @brief rocksdb returns those type of errors, as described in
   * <rocksdb/status.h>, Status::Code (it is private)
   */
  enum class rocksdbError {
    OK = 0,
    NOT_FOUND = 1,
    CORRUPTION = 2,
    NOT_SUPPORTED = 3,
    INVALID_ARGUMENT = 4,
    IO_ERROR = 5,

    UNKNOWN = 1000
  };

}  // namespace sgns::ipfs_lite

OUTCOME_HPP_DECLARE_ERROR_2(sgns::ipfs_lite, rocksdbError);

#endif  // CPP_IPFS_LITE_ROCKSDB_ERROR_HPP
