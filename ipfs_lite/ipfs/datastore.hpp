#pragma once

#include <vector>

#include "codec/cbor/cbor.hpp"
#include "common/buffer.hpp"
#include "common/logger.hpp"
#include "common/outcome.hpp"
#include "primitives/cid/cid.hpp"
#include "ipfs_lite/ipfs/ipfs_datastore_error.hpp"

namespace sgns::ipfs_lite::ipfs
{

    class IpfsDatastore
    {
    public:
        using Value = common::Buffer;

        virtual ~IpfsDatastore() = default;

        /**
         * @brief check if data store has value
         * @param key key to find
         * @return true if value exists, false otherwise
         */
        virtual IPFS::outcome::result<bool> contains( const CID &key ) const = 0;

        /**
         * @brief associates key with value in data store
         * @param key key to associate
         * @param value value to associate with key
         * @return success if operation succeeded, error otherwise
         */
        virtual IPFS::outcome::result<void> set( const CID &key, Value value ) = 0;

        /**
         * @brief       Seals a branch that starts with the informed CID
         * @param[in]   key CID to seal the branch (prune)
         * @return      Success if able to seal, error otherwise
         */
        virtual IPFS::outcome::result<void> seal( const CID &key ) = 0;

        /**
         * @brief       Checks if the informed CID is sealed
         * @param[in]   key CID to check if sealed
         * @return      true if sealed, false otherwise
         */
        virtual IPFS::outcome::result<bool> is_sealed( const CID &key ) const = 0;

        /**
         * @brief searches for a key in data store
         * @param key key to find
         * @return value associated with key or error
         */
        virtual IPFS::outcome::result<Value> get( const CID &key ) const = 0;

        /**
         * @brief removes key from data store
         * @param key key to remove
         * @return success if removed or didn't exist, error otherwise
         */
        virtual IPFS::outcome::result<void> remove( const CID &key ) = 0;

        /**
         * @brief CBOR-serialize value and store
         * @param value - data to serialize and store
         * @return cid of CBOR-serialized data
         */
        template <typename T>
        IPFS::outcome::result<CID> setCbor( const T &value )
        {
            BOOST_OUTCOME_TRY( auto bytes, codec::cbor::encode( value ) );
            BOOST_OUTCOME_TRY( auto key, common::getCidOf( bytes ) );
            BOOST_OUTCOME_TRYV2( auto &&, set( key, Value( bytes ) ) );
            return std::move( key );
        }

        /// Get CBOR decoded value by CID
        template <typename T>
        IPFS::outcome::result<T> getCbor( const CID &key ) const
        {
            BOOST_OUTCOME_TRY( auto bytes, get( key ) );
            return codec::cbor::decode<T>( bytes );
        }
    };
}
