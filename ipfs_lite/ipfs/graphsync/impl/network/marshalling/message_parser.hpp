
#ifndef CPP_IPFS_LITE_GRAPHSYNC_MESSAGE_PARSER_HPP
#define CPP_IPFS_LITE_GRAPHSYNC_MESSAGE_PARSER_HPP

#include "message.hpp"

namespace sgns::ipfs_lite::ipfs::graphsync {

  /// Parses protobuf message received from wire
  /// \param bytes Raw bytes of received message, without length prefix
  /// \return Message or error
  outcome::result<Message> parseMessage(gsl::span<const uint8_t> bytes);

}  // namespace sgns::ipfs_lite::ipfs::graphsync

#endif  // CPP_IPFS_LITE_GRAPHSYNC_MESSAGE_PARSER_HPP
