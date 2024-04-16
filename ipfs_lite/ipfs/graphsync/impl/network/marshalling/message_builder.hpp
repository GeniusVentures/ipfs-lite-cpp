
#ifndef CPP_IPFS_LITE_GRAPHSYNC_MESSAGE_BUILDER_HPP
#define CPP_IPFS_LITE_GRAPHSYNC_MESSAGE_BUILDER_HPP

#include "message.hpp"

namespace sgns::ipfs_lite::ipfs::graphsync {

  namespace pb {
    // fwd declaration of protobuf message
    class Message;
  }

  /// Base class for request and response builders
  class MessageBuilder {
   public:
    MessageBuilder();
    virtual ~MessageBuilder();

    /// Returns if there is nothing to send
    bool empty() const;

    /// Returns serialized size of protobuf message
    size_t getSerializedSize() const;

    /// Serializes message to shared byte buffer
    IPFS::outcome::result<SharedData> serialize();

    /// Clears all entries added
    void clear();

   protected:
    /// Protobuf message, reused by derived classes
    std::unique_ptr<pb::Message> pb_msg_; //NOLINT

    /// Empty message indicator, reused by derived classes
    bool empty_ = true; //NOLINT
  };

}  // namespace sgns::ipfs_lite::ipfs::graphsync

#endif  // CPP_IPFS_LITE_GRAPHSYNC_MESSAGE_BUILDER_HPP
