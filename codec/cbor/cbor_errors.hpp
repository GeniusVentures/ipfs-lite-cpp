#pragma once

#include "common/outcome.hpp"

namespace sgns::codec::cbor
{
    enum class CborEncodeError
    {
        INVALID_CID = 1,
        EXPECTED_MAP_VALUE_SINGLE
    };

    enum class CborDecodeError
    {
        INVALID_CBOR = 1,
        WRONG_TYPE,
        INT_OVERFLOW,
        INVALID_CBOR_CID,
        INVALID_CID,
        WRONG_SIZE,
    };
}

OUTCOME_HPP_DECLARE_ERROR_2( sgns::codec::cbor, CborEncodeError );
OUTCOME_HPP_DECLARE_ERROR_2( sgns::codec::cbor, CborDecodeError );
