#pragma once

#include "common/outcome.hpp"

namespace sgns::common
{

    /**
         * @brief Pieces returns these types of errors
         */
    enum class CommCidError
    {
        TOO_SHORT = 1,
        CANT_READ_CODE,
        CANT_READ_LENGTH,
        DATA_TOO_BIG,
        DIFFERENT_LENGTH,
        INVALID_HASH
    };

}

OUTCOME_HPP_DECLARE_ERROR_2( sgns::common, CommCidError );
