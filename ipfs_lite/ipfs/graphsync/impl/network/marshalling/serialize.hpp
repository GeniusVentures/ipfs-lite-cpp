
#ifndef CPP_IPFS_LITE_GRAPHSYNC_SERIALIZE_HPP
#define CPP_IPFS_LITE_GRAPHSYNC_SERIALIZE_HPP

// TODO(FIL-144) move this stuff to libp2p

#include <memory>

#include <boost/optional.hpp>

#include <libp2p/common/byteutil.hpp>

namespace google::protobuf {
  // protobuf's MessageLite fwd decl
  class MessageLite;
}

namespace sgns::ipfs_lite::ipfs::graphsync {

  /**
   * Tries to serialize generic protobuf message into shared array of bytes
   * with varint length prefix
   * @param msg protobuf message
   * @return shared buffer to serialized bytes, none if serialize failed, i.e.
   * not all required fields are set in the msg
   */
  boost::optional<std::shared_ptr<const libp2p::common::ByteArray>>
  serializeProtobufMessage(const google::protobuf::MessageLite &msg);

}  // namespace sgns::ipfs_lite::ipfs::graphsync

#endif  // CPP_IPFS_LITE_GRAPHSYNC_SERIALIZE_HPP
