#pragma once

#include "common/outcome.hpp"

namespace sgns
{
    enum class TodoError
    {
        ERROR = 1,
    };
}

OUTCOME_HPP_DECLARE_ERROR_2( sgns, TodoError );
