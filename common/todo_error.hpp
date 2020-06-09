#ifndef CPP_IPFS_LITE_COMMON_TODO_ERROR_HPP
#define CPP_IPFS_LITE_COMMON_TODO_ERROR_HPP

#include "common/outcome.hpp"

namespace sgns {
  enum class TodoError {
    ERROR = 1,
  };
}  // namespace sgns

OUTCOME_HPP_DECLARE_ERROR_2(sgns, TodoError);

#endif  // CPP_IPFS_LITE_COMMON_TODO_ERROR_HPP
