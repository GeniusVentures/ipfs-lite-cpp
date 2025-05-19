
#include "common/outcome.hpp"

#include <gtest/gtest.h>

namespace sgns {

  const int ret = 42;

  IPFS::outcome::result<void> funcSuccess() {
    return IPFS::outcome::success();
  }

  IPFS::outcome::result<int> funcSuccessReturn() {
    return ret;
  }

  /**
   * No throw on success result with no value
   */
  TEST(OutcomeExcept, OneArgNoExcept) {
    EXPECT_NO_THROW(_OUTCOME_EXCEPT_1(_UNIQUE_NAME(_r), funcSuccess()));
  }

  /**
   * No throw on success result with value returned
   */
  TEST(OutcomeExcept, ValueReturnedNoExcept) {
    _OUTCOME_EXCEPT_2(_UNIQUE_NAME(_r), res, funcSuccessReturn());
    EXPECT_EQ(res, ret);
  }

}  // namespace sgns
