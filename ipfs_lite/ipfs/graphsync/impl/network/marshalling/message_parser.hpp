#pragma once

#include "message.hpp"

namespace sgns::ipfs_lite::ipfs::graphsync
{

    /// Parses protobuf message received from wire
    /// \param bytes Raw bytes of received message, without length prefix
    /// \return Message or error
    IPFS::outcome::result<Message> parseMessage( gsl::span<const uint8_t> bytes );

}
