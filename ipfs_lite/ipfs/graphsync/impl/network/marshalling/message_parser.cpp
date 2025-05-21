
#include "message_parser.hpp"

#include "codec/cbor/cbor_decode_stream.hpp"
#include "common/span.hpp"
#include "crypto/hasher/hasher.hpp"

#include "protobuf/message.pb.h"

namespace sgns::ipfs_lite::ipfs::graphsync {
namespace {
using codec::cbor::CborDecodeStream;

// Checks status code received from wire
IPFS::outcome::result<ResponseStatusCode> extractStatusCode(int code) {
    switch (code) {
        // clang-format off
#define CHECK_CASE(X) case RS_##X: return RS_##X;
        CHECK_CASE(NO_PEERS)
        CHECK_CASE(CANNOT_CONNECT)
        CHECK_CASE(TIMEOUT)
        CHECK_CASE(CONNECTION_ERROR)
        CHECK_CASE(INTERNAL_ERROR)
        CHECK_CASE(REJECTED_LOCALLY)
        CHECK_CASE(SLOW_STREAM)
        CHECK_CASE(AWAITING_STATUS)

        CHECK_CASE(REQUEST_ACKNOWLEDGED)
        CHECK_CASE(ADDITIONAL_PEERS)
        CHECK_CASE(NOT_ENOUGH_GAS)
        CHECK_CASE(OTHER_PROTOCOL)
        CHECK_CASE(PARTIAL_RESPONSE)

        CHECK_CASE(FULL_CONTENT)
        CHECK_CASE(PARTIAL_CONTENT)

        CHECK_CASE(REJECTED)
        CHECK_CASE(TRY_AGAIN)
        CHECK_CASE(REQUEST_FAILED)
        CHECK_CASE(LEGAL_ISSUES)
        CHECK_CASE(NOT_FOUND)
#undef CHECK_CASE
        // clang-format on

        default:
            logger()->error("{}: unknonwn status code {}", __FUNCTION__, code);
            return Error::MESSAGE_PARSE_ERROR;
    }
}

// Extracts requests from protobuf message
IPFS::outcome::result<void> parseRequests(pb::Message& pb_msg, Message& msg) {
    if (auto sz = pb_msg.requests_size(); sz > 0) {
        msg.requests.reserve(sz);

        for (const auto& src : pb_msg.requests()) {
            Message::Request dst;
            dst.id = src.id();

            if (src.cancel()) {
                dst.cancel = true;
            } else {
                OUTCOME_TRY((auto&&, cid), CID::fromBytes(common::span::cbytes(src.root())));
                dst.root_cid = std::move(cid);
                dst.selector = common::Buffer{common::span::cbytes(src.selector())};
                dst.priority = src.priority();

                for (const auto& kv : src.extensions()) {
                    std::vector<uint8_t> data(kv.second.begin(), kv.second.end());
                    dst.extensions.emplace_back(Extension{kv.first, data});
                }
            }

            msg.requests.emplace_back(dst);
        }
    }

    return IPFS::outcome::success();
}

// Extracts responses from protobuf message
IPFS::outcome::result<void> parseResponses(pb::Message& pb_msg, Message& msg) {
    size_t sz = pb_msg.responses_size();
    if (sz > 0) {
        msg.responses.reserve(sz);

        for (const auto& src : pb_msg.responses()) {
            auto& dst = msg.responses.emplace_back(Message::Response());
            dst.id = src.id();
            OUTCOME_TRY((auto&&, status), extractStatusCode(src.status()));
            dst.status = status;

            for (const auto& kv : src.extensions()) {
                std::vector<uint8_t> data(kv.second.begin(), kv.second.end());
                dst.extensions.emplace_back(Extension{kv.first, data});
            }
        }
    }
    return IPFS::outcome::success();
}

// Extracts data blocks from protobuf message
IPFS::outcome::result<void> parseBlocks(pb::Message& pb_msg, Message& msg) {
    size_t sz = pb_msg.data_size();
    if (sz > 0) {
        msg.data.reserve(sz);

        for (const auto& src : pb_msg.data()) {
            auto data = common::Buffer{common::span::cbytes(src.data())};
            auto prefix_reader = common::span::cbytes(src.prefix());
            OUTCOME_TRY((auto&&, cid), CID::read(prefix_reader, true));
            if (!prefix_reader.empty()) {
                return Error::MESSAGE_PARSE_ERROR;
            }
            cid.content_address = crypto::IPFSHasher::calculate(
                cid.content_address.getType(),
                data
            );
            msg.data.emplace_back(std::move(cid), data);
        }
    }
    return IPFS::outcome::success();
}
}

IPFS::outcome::result<Message> parseMessage(gsl::span<const uint8_t> bytes) {
    pb::Message pb_msg;

    if (!pb_msg.ParseFromArray(bytes.data(), bytes.size())) {
        logger()->warn("{}: cannot parse protobuf message, size={}", __FUNCTION__, bytes.size());
        return Error::MESSAGE_PARSE_ERROR;
    }

    Message msg;

    msg.complete_request_list = pb_msg.completerequestlist();

    auto res = parseRequests(pb_msg, msg);
    if (!res) {
        return res.error();
    }

    res = parseResponses(pb_msg, msg);
    if (!res) {
        return res.error();
    }

    res = parseBlocks(pb_msg, msg);
    if (!res) {
        return res.error();
    }

    return msg;
}

} // namespace sgns::ipfs_lite::ipfs::graphsync
