#pragma once

#include "primitives/cid/cid.hpp"
#include "codec/cbor/cbor.hpp"

namespace sgns::primitives::cid
{

    /**
     * @brief helper function to calculate cid of an object
     * @tparam T object type
     * @param value object instance reference
     * @return calculated cid or error
     */
    template <class T>
    IPFS::outcome::result<CID> getCidOfCbor( const T &value )
    {
        BOOST_OUTCOME_TRY( auto bytes, sgns::codec::cbor::encode( value ) );
        return common::getCidOf( bytes );
    }
}
