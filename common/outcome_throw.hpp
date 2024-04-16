/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CPP_IPFS_LITE_COMMON_OUTCOME_THROW_HPP
#define CPP_IPFS_LITE_COMMON_OUTCOME_THROW_HPP

#include <boost/system/system_error.hpp>
#include <boost/throw_exception.hpp>

namespace sgns::common {
  /**
   * @brief throws IPFS::outcome::result error as boost exception
   * @tparam T enum error type, only IPFS::outcome::result enums are allowed
   * @param t error value
   */
  template <typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
  void raise(T t) {
    std::error_code ec = make_error_code(t);
    boost::throw_exception(std::system_error(ec));
  }

  /**
   * @brief throws IPFS::outcome::result error made of error as boost exception
   * @tparam T IPFS::outcome error type
   * @param t IPFS::outcome error value
   */
  template <typename T, typename = std::enable_if_t<!std::is_enum_v<T>>>
  void raise(const T &t) {
    boost::throw_exception(std::system_error(t.value(), t.category()));
  }
}  // namespace sgns::common

#endif  // CPP_IPFS_LITE_COMMON_OUTCOME_THROW_HPP
