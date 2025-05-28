
#include "message_reader.hpp"

#include <cassert>

#include "ipfs_lite/ipfs/graphsync/impl/network/marshalling/message_parser.hpp"
#include "length_delimited_message_reader.hpp"
#include "peer_context.hpp"

namespace sgns::ipfs_lite::ipfs::graphsync {

  MessageReader::MessageReader(StreamPtr stream,
                               EndpointToPeerFeedback &feedback)
      : feedback_(feedback) {
    assert(stream);
    assert(!stream->isClosedForRead());

    if (!stream_reader_) {
      stream_reader_ = std::make_shared<LengthDelimitedMessageReader>(
          [this](const StreamPtr &stream, IPFS::outcome::result<ByteArray> res) {
            onMessageRead(stream, std::move(res));
          },
          kMaxMessageSize);
    }
    stream_reader_->read(std::move(stream));
  }

  MessageReader::~MessageReader() {
    // detach shared ptr from any feedback in case it remains alive
    // after this dtor
    stream_reader_->close();
  }

  void MessageReader::onMessageRead(const StreamPtr &stream,
                                    IPFS::outcome::result<ByteArray> res) {
    if (!res) {
      feedback_.onReaderEvent(stream, res.error());
      return;
    }

    auto msg_res = parseMessage(res.value());

    feedback_.onReaderEvent(stream, std::move(msg_res));
  }

}  // namespace sgns::ipfs_lite::ipfs::graphsync
