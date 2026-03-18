#pragma once

#include <memory>

#include "ipfs_lite/ipfs/datastore.hpp"

namespace sgns::ipfs_lite::ipfs
{
    class IpfsBlockService : public IpfsDatastore
    {
    public:
        /**
     * @brief Construct IPFS storage
     * @param data_store - IPFS storage implementation
     */
        explicit IpfsBlockService( std::shared_ptr<IpfsDatastore> data_store );

        IPFS::outcome::result<bool> contains( const CID &key ) const override;

        IPFS::outcome::result<void> set( const CID &key, Value value ) override;

        IPFS::outcome::result<void> seal( const CID &key ) override;

        IPFS::outcome::result<bool> is_sealed( const CID &key ) const override;

        IPFS::outcome::result<Value> get( const CID &key ) const override;

        IPFS::outcome::result<void> remove( const CID &key ) override;

    private:
        std::shared_ptr<IpfsDatastore> local_storage_; /**< Local data storage */
    };
}
