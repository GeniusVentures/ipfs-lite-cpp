#pragma once

#include "common/outcome.hpp"
#include "primitives/cid/cid.hpp"
#include "common/buffer.hpp"

namespace sgns::codec::json
{
    enum class JsonCodecError : int
    {
        BAD_JSON = 1,
        WRONG_CID_ARRAY_FORMAT
    };
    /**
     * @brief json-encodes span of CID objects
     */
    IPFS::outcome::result<std::string> encodeCidVector( gsl::span<const CID> span );

    /**
     * @brief tries to json-decode vector of CID objects
     * @param data source
     * @return vector of CID objects or error
     */
    IPFS::outcome::result<std::vector<CID>> decodeCidVector( std::string_view data );
}

OUTCOME_HPP_DECLARE_ERROR_2( sgns::codec::json, JsonCodecError );
