#pragma once

#include <map>
#include <memory>
#include <set>
#include <string>

#include <gsl/span>
#include "common/buffer.hpp"
#include "common/outcome.hpp"
#include "ipfs_lite/ipld/ipld_link.hpp"
#include "ipfs_lite/ipld/ipld_node.hpp"

namespace sgns::ipfs_lite::ipld
{
    /**
     * @class Protobuf-serializer for MerkleDAG Nodes
     * @details Order of parts of the Protobuf-serialized data is forced
     *          specially for backward-compatibility with reference golang
     *          implementation
     * @warning Need to update serialization algorithm on Protobuf-scheme change
     */
    class IPLDNodeEncoderPB
    {
    public:
        /**
         * @brief Serialize Node
         * @param content - Node data
         * @param links - references for child Nodes
         * @param destinations - destination identifiers
         * @return Protobuf-encoded data
         */
        static std::vector<uint8_t> encode( const common::Buffer                  &content,
                                            const std::map<std::string, IPLDLink> &links,
                                            const std::set<std::string>           &destinations );

    private:
        using PBTag = uint8_t;

        // Protobuf wire types
        enum class PBFieldType : uint8_t
        {
            VARINT = 0,
            BITS_64,
            LENGTH_DELEMITED,
            START_GROUP,
            END_GROUP,
            BITS_32
        };

        enum class PBLinkOrder : uint8_t
        {
            HASH = 1,
            NAME,
            SIZE
        };

        enum class PBNodeOrder : uint8_t
        {
            DATA = 1,
            LINKS,
            DESTINATIONS
        };

        // Serialized content
        common::Buffer data_;

        /**
         * @brief Calculate length of the serialized link
         * @param name - link name
         * @param link - child link
         * @return Number of bytes
         */
        static size_t getLinkLengthPB( const std::string &name, const IPLDLink &link );

        /**
         * @brief Calculate length of the serialized content
         * @param content - Node's data
         * @return Number of bytes
         */
        static size_t getContentLengthPB( const common::Buffer &content );

        /**
         * @brief Calculate length of the serialized destinations
         * @param destinations - Set of destination strings
         * @return Number of bytes
         */
        static size_t getDestinationsLengthPB( const std::set<std::string> &destinations );

        /**
         * @brief Serialized Node's links
         * @param links - Node's children
         * @return Raw bytes
         */
        static std::vector<uint8_t> serializeLinks( const std::map<std::string, IPLDLink> &links );

        /**
         * @brief Serialized Node's content
         * @param content - Node's data
         * @return Raw bytes
         */
        static std::vector<uint8_t> serializeContent( const common::Buffer &content );

        /**
         * @brief Serialize Node's destinations
         * @param destinations - Node's destination identifiers
         * @return Raw bytes
         */
        static std::vector<uint8_t> serializeDestinations( const std::set<std::string> &destinations );

        /**
         * @brief Create Protobuf filed header
         * @param type - field type
         * @param order - field order
         * @return Tag value
         */
        static PBTag createTag( PBFieldType type, uint8_t order );
    };
}
