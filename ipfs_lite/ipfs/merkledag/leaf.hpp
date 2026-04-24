#pragma once

#include <functional>
#include <string_view>

#include "common/buffer.hpp"
#include "common/outcome.hpp"

namespace sgns::ipfs_lite::ipfs::merkledag
{
    class Leaf
    {
    public:
        /**
         * @brief Construct leaf
         * @param data - leaf content
         */
        explicit Leaf( common::Buffer data );

        /**
         * @brief Get leaf content
         * @return operation result
         */
        const common::Buffer &content() const
        {
            return content_;
        }

        /**
         * @brief Get count of children leaves
         * @return Count of leaves
         */
        size_t count() const
        {
            return children_.size();
        }

        /**
         * @brief Get children leaf
         * @param name - leaf name
         * @return operation result
         */
        IPFS::outcome::result<std::reference_wrapper<const Leaf>> subLeaf( std::string_view name );

        IPFS::outcome::result<void> insertSubLeaf( std::string name, Leaf children );

    private:
        common::Buffer                           content_;
        std::map<std::string, Leaf, std::less<>> children_;
    };

    /**
     * @enum Possible leaf errors
     */
    enum class LeafError : uint8_t
    {
        LEAF_NOT_FOUND = 1,
        DUPLICATE_LEAF
    };
}

OUTCOME_HPP_DECLARE_ERROR_2( sgns::ipfs_lite::ipfs::merkledag, LeafError )
