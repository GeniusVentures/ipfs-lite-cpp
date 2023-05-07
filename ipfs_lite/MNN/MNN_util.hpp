
#ifndef IPFS_LITE_MNN_MNN_UTIL_HPP_
#define IPFS_LITE_MNN_MNN_UTIL_HPP_

#include "ipfs_lite/MNN/MNN_error.hpp"
#include "common/logger.hpp"

namespace sgns::ipfs_lite {

  template <typename T>
  inline outcome::result<T> error_as_result(const MNNError &s) {
    return s;
  }

  template <typename T>
  inline outcome::result<T> error_as_result(const MNNError &s,
                                            const common::Logger &logger) {
    logger->error(ToString(s));
    return error_as_result<T>(s);
  }
}// End namespace


#endif /* IPFS_LITE_MNN_MNN_UTIL_HPP_ */
