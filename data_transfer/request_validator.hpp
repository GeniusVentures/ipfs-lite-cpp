#pragma once

#include <libp2p/peer/peer_id.hpp>
#include "common/outcome.hpp"
#include "primitives/cid/cid.hpp"
#include "ipfs_lite/ipld/ipld_node.hpp"

namespace sgns::data_transfer
{

    using ipfs_lite::ipld::IPLDNode;
    using libp2p::peer::PeerId;

    /**
     * RequestValidator is an interface implemented by the client of the data
     * transfer module to validate requests
     */
    class RequestValidator
    {
    public:
        virtual ~RequestValidator() = default;

        /**
         * ValidatePush validates a push request received from the peer that will
         * send data
         */
        virtual IPFS::outcome::result<void> validatePush( const PeerId             &sender,
                                                          std::vector<uint8_t>      voucher,
                                                          CID                       base_cid,
                                                          std::shared_ptr<IPLDNode> selector ) = 0;
        /**
         * ValidatePull validates a pull request received from the peer thatF will
         * receive data
         */
        virtual IPFS::outcome::result<void> validatePull( const PeerId             &receiver,
                                                          std::vector<uint8_t>      voucher,
                                                          CID                       base_cid,
                                                          std::shared_ptr<IPLDNode> selector ) = 0;
    };

}
