#pragma once

#include "data_transfer/message.hpp"

namespace sgns::data_transfer
{

    /**
     * MessageSender is an interface to send messages to a peer
     */
    class MessageSender
    {
    public:
        virtual ~MessageSender() = default;

        virtual IPFS::outcome::result<void> sendMessage( const DataTransferMessage &message ) = 0;

        virtual IPFS::outcome::result<void> close() = 0;

        virtual IPFS::outcome::result<void> reset() = 0;
    };

}
