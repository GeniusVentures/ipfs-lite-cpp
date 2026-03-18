#pragma once

#include "data_transfer/message_sender.hpp"

#include <libp2p/connection/stream.hpp>

namespace sgns::data_transfer
{

    class StreamMessageSender : public MessageSender
    {
    public:
        explicit StreamMessageSender( std::shared_ptr<libp2p::connection::Stream> stream );

        IPFS::outcome::result<void> sendMessage( const DataTransferMessage &message ) override;

        IPFS::outcome::result<void> close() override;

        IPFS::outcome::result<void> reset() override;

    private:
        std::shared_ptr<libp2p::connection::Stream> stream_;
    };

}
