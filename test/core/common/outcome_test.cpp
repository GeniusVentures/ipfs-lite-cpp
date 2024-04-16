
#include "common/outcome.hpp"

#include <gtest/gtest.h>
#include "common/todo_error.hpp"

namespace sgns {

  const int ret = 42;

  IPFS::outcome::result<void> funcSuccess() {
    return IPFS::outcome::success();
  }

  IPFS::outcome::result<void> funcFailure() {
    return TodoError::ERROR;
  }

  IPFS::outcome::result<int> funcSuccessReturn() {
    return ret;
  }

  IPFS::outcome::result<int> funcFailureReturn() {
    return TodoError::ERROR;
  }

  /**
   * No throw on success result with no value
   */
  TEST(OutcomeExcept, OneArgNoExcept) {
    EXPECT_NO_THROW(_OUTCOME_EXCEPT_1(_UNIQUE_NAME(_r), funcSuccess()));
  }

  /**
   * Throw on failure result with no value
   */
  TEST(OutcomeExcept, OneArgExcept) {
    EXPECT_THROW(_OUTCOME_EXCEPT_1(_UNIQUE_NAME(_r), funcFailure()), std::system_error);
  }

  /**
   * No throw on success result with value returned
   */
  TEST(OutcomeExcept, ValueReturnedNoExcept) {
    _OUTCOME_EXCEPT_2(_UNIQUE_NAME(_r), res, funcSuccessReturn());
    EXPECT_EQ(res, ret);
  }

  /**
   * Throw on failure result with value returned
   */
  TEST(OutcomeExcept, ValueReturnedExcept) {
    EXPECT_THROW(_OUTCOME_EXCEPT_2(_UNIQUE_NAME(_r), res, funcFailureReturn()), std::system_error);
  }

}  // namespace sgns
