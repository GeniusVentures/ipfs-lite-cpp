#pragma once

#include <memory>

#include "common/outcome.hpp"
#include "ipfs_lite/ipfs/merkledag/leaf.hpp"
#include "ipfs_lite/ipld/ipld_node.hpp"

namespace sgns::ipfs_lite::ipfs::merkledag
{
    using ipld::IPLDNode;

    class MerkleDagService
    {
    public:
        /**
         * @brief Destructor
         */
        virtual ~MerkleDagService() = default;

        /**
         * @brief Add new node
         * @param node - entity to add
         * @return operation result
         */
        virtual IPFS::outcome::result<void> addNode( std::shared_ptr<const IPLDNode> node ) = 0;

        /**
         * @brief       Mark that a node (CID) had all its children added
         * @param[in]   node The node to mark for resolved
         * @return      Success if marked, error otherwise
         */
        virtual IPFS::outcome::result<void> markResolved( const CID &cid ) = 0;
        /**
         * @brief Get node by id
         * @param cid - node id
         * @return operation result
         */
        virtual IPFS::outcome::result<std::shared_ptr<IPLDNode>> getNode( const CID &cid ) const = 0;

        /**
         * @brief       Check if a CID was already resolved (no need to traverse it)
         * @param[in]   cid Node CID to check
         * @return      True if resolved, false otherwise
         */
        virtual IPFS::outcome::result<bool> isResolved( const CID &cid ) const = 0;

        /**
         * @brief Remove node by id
         * @param cid - node id
         * @return operation result
         */
        virtual IPFS::outcome::result<void> removeNode( const CID &cid ) = 0;

        /**
         * @brief Get nodes with IPLD-selector
         * @param root_cid - bytes of the root Node CID
         * @param selector - IPLD-selector raw bytes
         * @param handler - receiver of the selected Nodes, should return false to
         * break receiving process
         * @return count of the received by handler Nodes
         */
        virtual IPFS::outcome::result<size_t> select(
            gsl::span<const uint8_t>                                    root_cid,
            gsl::span<const uint8_t>                                    selector,
            std::function<bool( std::shared_ptr<const IPLDNode> node )> handler ) const = 0;

        /**
         * @brief Fetcg graph from given root node
         * @param cid - identifier of the root node
         * @return operation result
         */
        virtual IPFS::outcome::result<std::shared_ptr<Leaf>> fetchGraph( const CID &cid ) const = 0;

        /**
         * @brief Construct graph from given root node till chosen limit
         *        (0 means "Fetch only root node")
         * @param cid - identifier of the root node
         * @param depth - limit of the depth to fetch
         * @return operation result
         */
        virtual IPFS::outcome::result<std::shared_ptr<Leaf>> fetchGraphOnDepth( const CID &cid,
                                                                                uint64_t   depth ) const = 0;
    };

    /**
     * @class Possible MerkleDAG service errors
     */
    enum class ServiceError: uint8_t
    {
        UNRESOLVED_LINK = 1 // This error can occur if child node not found
    };
}

OUTCOME_HPP_DECLARE_ERROR_2( sgns::ipfs_lite::ipfs::merkledag, ServiceError )
