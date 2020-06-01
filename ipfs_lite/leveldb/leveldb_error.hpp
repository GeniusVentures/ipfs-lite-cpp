
#ifndef CPP_IPFS_LITE_LEVELDB_ERROR_HPP
#define CPP_IPFS_LITE_LEVELDB_ERROR_HPP

#include "common/outcome.hpp"

namespace sgns::ipfs_lite {

  /**
   * @brief LevelDB returns those type of errors, as described in
   * <leveldb/status.h>, Status::Code (it is private)
   */
  enum class LevelDBError {
    OK = 0,
    NOT_FOUND = 1,
    CORRUPTION = 2,
    NOT_SUPPORTED = 3,
    INVALID_ARGUMENT = 4,
    IO_ERROR = 5,

    UNKNOWN = 1000
  };

}  // namespace sgns::ipfs_lite

OUTCOME_HPP_DECLARE_ERROR(sgns::ipfs_lite, LevelDBError);

#endif  // CPP_IPFS_LITE_LEVELDB_ERROR_HPP
