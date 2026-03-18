#pragma once

#include <rocksdb/write_batch.h>
#include "ipfs_lite/rocksdb/rocksdb.hpp"

namespace sgns::ipfs_lite
{

    /**
     * @brief Class that is used to implement efficient bulk (batch) modifications
     * of the Map.
     */
    class rocksdb::Batch : public BufferBatch
    {
    public:
        using WriteBatch = ::ROCKSDB_NAMESPACE::WriteBatch;

        explicit Batch( rocksdb &db );

        IPFS::outcome::result<void> put( const Buffer &key, const Buffer &value ) override;
        IPFS::outcome::result<void> put( const Buffer &key, Buffer &&value ) override;

        IPFS::outcome::result<void> remove( const Buffer &key ) override;

        IPFS::outcome::result<void> commit() override;

        void clear() override;

    private:
        rocksdb   &db_;
        WriteBatch batch_;
    };

}
