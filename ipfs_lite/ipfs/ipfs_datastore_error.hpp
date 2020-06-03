
#ifndef CPP_IPFS_LITE_IPFS_DATASTORE_ERROR_HPP
#define CPP_IPFS_LITE_IPFS_DATASTORE_ERROR_HPP

#include "common/outcome.hpp"

namespace sgns::ipfs_lite::ipfs {

  /**
   * @brief Type of errors returned by Keystore
   */
  enum class IpfsDatastoreError {
    NOT_FOUND = 1,

    UNKNOWN = 1000
  };

}  // namespace sgns::ipfs_lite::ipfs

OUTCOME_HPP_DECLARE_ERROR_2(sgns::ipfs_lite::ipfs, IpfsDatastoreError);

#endif  // CPP_IPFS_LITE_IPFS_DATASTORE_ERROR_HPP
