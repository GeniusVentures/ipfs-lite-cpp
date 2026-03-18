#pragma once

#include <libp2p/multi/content_identifier.hpp>

#include "common/outcome.hpp"

namespace sgns
{
    class CID : public libp2p::multi::ContentIdentifier
    {
    public:
        using ContentIdentifier::ContentIdentifier;

        using Multicodec = libp2p::multi::MulticodecType::Code;

        /**
         * ContentIdentifier is not default-constructable, but in some cases we need
         * default value. This value can be used to initialize class member or local
         * variable. Trying to CBOR encode this value will yield error, to ensure
         * proper initialization.
         */
        CID();

        explicit CID( const ContentIdentifier &cid );

        explicit CID( ContentIdentifier &&cid ) noexcept;

        CID( CID &&cid ) noexcept;

        CID( const CID &cid ) = default;

        CID( Version version, Multicodec content_type, libp2p::multi::Multihash content_address );

        ~CID() = default;

        CID &operator=( const CID & ) = default;

        CID &operator=( CID &&cid ) noexcept;

        CID &operator=( const ContentIdentifier &cid );

        CID &operator=( ContentIdentifier &&cid );

        /**
         * @brief string-encodes cid
         * @return encoded value or error
         */
        IPFS::outcome::result<std::string> toString() const;

        /**
         * @brief encodes CID to bytes
         * @return byte-representation of CID
         */
        IPFS::outcome::result<std::vector<uint8_t>> toBytes() const;

        static IPFS::outcome::result<CID> fromString( const std::string &str );

        static IPFS::outcome::result<CID> fromBytes( gsl::span<const uint8_t> input );

        static IPFS::outcome::result<CID> read( gsl::span<const uint8_t> &input, bool prefix = false );
    };
}

namespace std
{
    template <>
    struct hash<sgns::CID>
    {
        size_t operator()( const sgns::CID &cid ) const
        {
            auto result = cid.toBytes();
            if ( !result )
            {
                return 0;
            }

            const auto &bytes = result.value();
            return std::hash<std::string_view>{}(
                std::string_view( reinterpret_cast<const char *>( bytes.data() ), bytes.size() ) );
        }
    };
}

namespace sgns::common
{
    /// Compute CID from bytes
    IPFS::outcome::result<CID> getCidOf( gsl::span<const uint8_t> bytes );

}
