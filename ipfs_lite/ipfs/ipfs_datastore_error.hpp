#pragma once

#include "common/outcome.hpp"

namespace sgns::ipfs_lite::ipfs
{

    /**
   * @brief Type of errors returned by Keystore
   */
    enum class IpfsDatastoreError
    {
        NOT_FOUND = 1,

        UNKNOWN = 1000
    };

}

OUTCOME_HPP_DECLARE_ERROR_2( sgns::ipfs_lite::ipfs, IpfsDatastoreError );
