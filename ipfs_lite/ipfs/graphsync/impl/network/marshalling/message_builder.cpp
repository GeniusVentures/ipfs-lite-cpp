#include "message_builder.hpp"

#include <libp2p/multi/uvarint.hpp>

#include "ipfs_lite/ipfs/graphsync/impl/common.hpp"

#include "protobuf/message.pb.h"

namespace sgns::ipfs_lite::ipfs::graphsync {

  MessageBuilder::MessageBuilder() : pb_msg_(std::make_unique<pb::Message>()) {}

  // need to define default dtor here in translation unit due to unique_ptr
  // to type which is incomplete in header
  MessageBuilder::~MessageBuilder() = default;

  size_t MessageBuilder::getSerializedSize() const {
    return pb_msg_->ByteSizeLong();
  }

  IPFS::outcome::result<SharedData> MessageBuilder::serialize() {
      size_t msg_size = pb_msg_->ByteSizeLong();

      auto encoded_size = libp2p::multi::UVarint{msg_size};
      const auto& encoded_size_vec = encoded_size.toVector();
      auto prefix_size = encoded_size_vec.size();

      auto buffer = std::make_shared<libp2p::common::ByteArray>(prefix_size + msg_size);

      uint8_t* data = buffer->data();

      memcpy(data, encoded_size_vec.data(), prefix_size);

      if (!pb_msg_->SerializeToArray(data + prefix_size, msg_size)) {
          logger()->error("cannot serialize protobuf message, size={}", getSerializedSize());
          return Error::MESSAGE_SERIALIZE_ERROR;
      }

      return buffer;
  }

  void MessageBuilder::clear() {
    empty_ = true;
    pb_msg_->Clear();
  }

}  // namespace sgns::ipfs_lite::ipfs::graphsync
