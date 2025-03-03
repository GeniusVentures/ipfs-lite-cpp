
#include "message_builder.hpp"

#include "serialize.hpp"
#include "ipfs_lite/ipfs/graphsync/impl/common.hpp"

#include "protobuf/message.pb.h"

namespace sgns::ipfs_lite::ipfs::graphsync {

  MessageBuilder::MessageBuilder() : pb_msg_(std::make_unique<pb::Message>()) {}

  // need to define default dtor here in translation unit due to unique_ptr
  // to type which is incomplete in header
  MessageBuilder::~MessageBuilder() = default;

  bool MessageBuilder::empty() const {
    return empty_;
  }

  size_t MessageBuilder::getSerializedSize() const {
    return pb_msg_->ByteSizeLong();
  }

  IPFS::outcome::result<SharedData> MessageBuilder::serialize() {
    auto res = serializeProtobufMessage(*pb_msg_);
    if (!res) {
      logger()->error("cannot serialize protobuf message, size={}",
                      getSerializedSize());
      return Error::MESSAGE_SERIALIZE_ERROR;
    }
    return res.value();
  }

  void MessageBuilder::clear() {
    empty_ = true;
    pb_msg_->Clear();
  }

}  // namespace sgns::ipfs_lite::ipfs::graphsync
