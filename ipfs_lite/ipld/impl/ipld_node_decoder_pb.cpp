#include "ipfs_lite/ipld/impl/ipld_node_decoder_pb.hpp"

namespace sgns::ipfs_lite::ipld
{
    IPFS::outcome::result<void> IPLDNodeDecoderPB::decode( gsl::span<const uint8_t> input )
    {
        const uint8_t *raw_bytes = input.data();
        if ( pb_node_.ParseFromArray( raw_bytes, input.size() ) )
        {
            return IPFS::outcome::success();
        }
        return IPLDNodeDecoderPBError::INVALID_RAW_BYTES;
    }

    const std::string &IPLDNodeDecoderPB::getContent() const
    {
        return pb_node_.data();
    }

    size_t IPLDNodeDecoderPB::getLinksCount() const
    {
        return pb_node_.links_size();
    }

    const std::string &IPLDNodeDecoderPB::getLinkName( size_t index ) const
    {
        return pb_node_.links( index ).name();
    }

    const std::string &IPLDNodeDecoderPB::getLinkCID( size_t index ) const
    {
        return pb_node_.links( index ).hash();
    }

    size_t IPLDNodeDecoderPB::getLinkSize( size_t index ) const
    {
        int size = pb_node_.links( index ).tsize();
        return size < 0 ? 0 : static_cast<size_t>( size );
    }

    size_t IPLDNodeDecoderPB::getDestinationsCount() const
    {
        return pb_node_.destinations_size();
    }

    const std::string &IPLDNodeDecoderPB::getDestination( size_t index ) const
    {
        return pb_node_.destinations( index );
    }

    std::vector<std::string> IPLDNodeDecoderPB::getDestinations() const
    {
        std::vector<std::string> destinations;
        destinations.reserve( pb_node_.destinations_size() );
        for ( int i = 0; i < pb_node_.destinations_size(); ++i )
        {
            destinations.push_back( pb_node_.destinations( i ) );
        }
        return destinations;
    }
}

OUTCOME_CPP_DEFINE_CATEGORY_3( sgns::ipfs_lite::ipld, IPLDNodeDecoderPBError, error )
{
    using sgns::ipfs_lite::ipld::IPLDNodeDecoderPBError;
    switch ( error )
    {
        case ( IPLDNodeDecoderPBError::INVALID_RAW_BYTES ):
            return "IPLD node Protobuf decoder: failed to deserialize from incorrect "
                   "raw bytes";
    }
    return "IPLD node protobuf decoder: unknown error";
}
