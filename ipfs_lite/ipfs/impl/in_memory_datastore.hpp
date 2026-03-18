
#ifndef CPP_IPFS_LITE_IPFS_IMPL_IN_MEMORY_DATASTORE_HPP
#define CPP_IPFS_LITE_IPFS_IMPL_IN_MEMORY_DATASTORE_HPP

#include <map>

#include "ipfs_lite/ipfs/datastore.hpp"

namespace sgns::ipfs_lite::ipfs
{

    class InMemoryDatastore : public IpfsDatastore
    {
    public:
        ~InMemoryDatastore() override = default;

        /** @copydoc IpfsDatastore::contains() */
        IPFS::outcome::result<bool> contains( const CID &key ) const override;

        /** @copydoc IpfsDatastore::set() */
        IPFS::outcome::result<void> set( const CID &key, Value value ) override;

        /** @copydoc IpfsDatastore::seal() */
        IPFS::outcome::result<void> seal( const CID &key ) override;

        /** @copydoc IpfsDatastore::is_sealed() */
        IPFS::outcome::result<bool> is_sealed( const CID &key ) const override;

        /** @copydoc IpfsDatastore::get() */
        IPFS::outcome::result<Value> get( const CID &key ) const override;

        /** @copydoc IpfsDatastore::remove() */
        IPFS::outcome::result<void> remove( const CID &key ) override;

    private:
        std::map<CID, Value> storage_;
        std::map<CID, Value> seals_;
    };

}

#endif // CPP_IPFS_LITE_IPFS_IMPL_IN_MEMORY_DATASTORE_HPP
