#pragma once

#include <string>

#include "primitives/cid/cid.hpp"

namespace sgns::ipfs_lite::ipld
{
    class IPLDLink
    {
    public:
        /**
         * @brief Construct Link
         * @param id - CID of the target object
         * @param name - name of the target object
         * @param size - total size of the target object
         */
        IPLDLink( CID id, std::string name, size_t size ) :
            cid_( std::move( id ) ), name_( std::move( name ) ), size_( size )
        {
        }

        ~IPLDLink() = default;

        /**
         * @brief Get name of the link
         * @return Name, which should be unique per object
         */
        const std::string &getName() const
        {
            return name_;
        }

        /**
         * @brief Get identifier of the target object
         * @return Content identifier
         */
        const CID &getCID() const
        {
            return cid_;
        }

        /**
         * @brief Get target object size
         * @return Cumulative size of the target object
         */
        size_t getSize() const
        {
            return size_;
        }

    private:
        CID         cid_;
        std::string name_;
        size_t      size_{};
    };
}
