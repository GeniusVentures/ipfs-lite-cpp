#pragma once

#include "data_transfer/network.hpp"

#include <libp2p/host/host.hpp>

namespace sgns::data_transfer
{

    using libp2p::Host;

    class Libp2pDataTransferNetwork : public DataTransferNetwork
    {
    public:
        explicit Libp2pDataTransferNetwork( std::shared_ptr<Host> host );

        IPFS::outcome::result<void> setDelegate( std::shared_ptr<MessageReceiver> receiver ) override;

        IPFS::outcome::result<void> connectTo( const PeerId &peer ) override;

        IPFS::outcome::result<std::shared_ptr<MessageSender>> newMessageSender( const PeerId &peer ) override;

    private:
        std::shared_ptr<libp2p::Host>    host_;
        std::shared_ptr<MessageReceiver> receiver_;
    };

}
