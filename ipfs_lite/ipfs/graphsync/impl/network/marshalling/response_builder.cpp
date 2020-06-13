#include "response_builder.hpp"

#include "codec/cbor/cbor_encode_stream.hpp"

#include "protobuf/message.pb.h"

namespace sgns::ipfs_lite::ipfs::graphsync {

  namespace {
    using codec::cbor::CborEncodeStream;
  }  // namespace

  void ResponseBuilder::addResponse(RequestId request_id,
                                    ResponseStatusCode status,
                                    const std::vector<Extension> &extensions) {
    auto *dst = pb_msg_->add_responses();
    dst->set_id(request_id);
    dst->set_status(status);

    for (auto extension : extensions) {
      dst->mutable_extensions()->insert(
          {std::string(extension.name),
           std::string(extension.data.begin(), extension.data.end())});
    }

    empty_ = false;
  }

  void ResponseBuilder::addDataBlock(const CID &cid,
                                     const common::Buffer &data) {
    auto *dst = pb_msg_->add_data();

    _OUTCOME_EXCEPT_2(_UNIQUE_NAME(_r), d, cid.toBytes());
    auto prefix_reader = gsl::make_span(std::as_const(d));
    _OUTCOME_EXCEPT_1(_UNIQUE_NAME(_r), CID::read(prefix_reader, true));
    dst->set_prefix(d.data(), d.size() - prefix_reader.size());
    dst->set_data(data.data(), data.size());
    empty_ = false;
  }

}  // namespace sgns::ipfs_lite::ipfs::graphsync
