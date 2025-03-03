

#include "ipfs_lite/ipld/walker.hpp"

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

namespace sgns::ipfs_lite::ipld::walker {
  using google::protobuf::io::CodedInputStream;
  using Input = gsl::span<const uint8_t>;

  struct PbDecoder {
    PbDecoder(Input input) : input{input}, cs(input.data(), input.size()) {}

    Input _str(uint64_t id) {
      if (cs.ExpectTag((id << 3) | 2)) {
        int size;
        if (cs.ReadVarintSizeAsInt(&size)) {
          auto offset = cs.CurrentPosition();
          if (cs.Skip(size)) {
            return input.subspan(offset, size);
          }
        }
      }
      return {};
    }

    bool empty() {
      return cs.CurrentPosition() >= input.size();
    }

    Input input;
    CodedInputStream cs;
  };

  struct PbNodeDecoder {
    static IPFS::outcome::result<std::vector<CID>> links(
        gsl::span<const uint8_t> input) {
      std::vector<CID> cids;
      PbDecoder s{input};
      while (true) {
        PbDecoder link{s._str(2)};
        if (link.empty()) {
          break;
        }
        auto cid_bytes = link._str(1);
        OUTCOME_TRY((auto &&, cid), CID::fromBytes(cid_bytes));
        cids.push_back(std::move(cid));
      }
      return std::move(cids);
    }
  };

  IPFS::outcome::result<void> Walker::select(const CID &root,
                                       const Selector &selector) {
    // TODO(turuslan): implement selectors
    return recursiveAll(root);
  }

  IPFS::outcome::result<void> Walker::recursiveAll(const CID &cid) {
    if (visited.insert(cid).second) {
      cids.push_back(cid);
      OUTCOME_TRY((auto &&, bytes), store.get(cid));
      // TODO(turuslan): what about other types?
      if (cid.content_type == libp2p::multi::MulticodecType::Code::DAG_CBOR) {
        try {
          CborDecodeStream s{bytes};
          recursiveAll(s);
        } catch (std::system_error &e) {
          return IPFS::outcome::failure(e.code());
        }
      } else if (cid.content_type == libp2p::multi::MulticodecType::Code::DAG_PB) {
        OUTCOME_TRY((auto &&, cids), PbNodeDecoder::links(bytes));
        for (auto &cid : cids) {
          BOOST_OUTCOME_TRYV2(auto &&, recursiveAll(cid));
        }
      }
    }
    return IPFS::outcome::success();
  }

  void Walker::recursiveAll(CborDecodeStream &s) {
    if (s.isCid()) {
      CID cid;
      s >> cid;
      auto result = recursiveAll(cid);
      if (!result) {
        IPFS::outcome::raise(result.error());
      }
    } else if (s.isList()) {
      auto n = s.listLength();
      for (auto l = s.list(); n != 0; --n) {
        recursiveAll(l);
      }
    } else if (s.isMap()) {
      for (auto &p : s.map()) {
        recursiveAll(p.second);
      }
    } else {
      s.next();
    }
  }
}  // namespace sgns::ipfs_lite::ipld::walker
