#pragma once

#include "common/outcome.hpp"

namespace sgns::ipfs_lite::ipld
{
    /**
     * @enum Possible PBNodeDecoder errors
     */
    enum class IPLDNodeDecoderPBError: uint8_t
    {
        INVALID_RAW_BYTES = 1
    };
}

OUTCOME_HPP_DECLARE_ERROR_2( sgns::ipfs_lite::ipld, IPLDNodeDecoderPBError )
