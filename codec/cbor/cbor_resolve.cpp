#include "codec/cbor/cbor_resolve.hpp"

OUTCOME_CPP_DEFINE_CATEGORY_3(sgns::codec::cbor, CborResolveError, e) {
  using sgns::codec::cbor::CborResolveError;
  switch (e) {
    case CborResolveError::INT_KEY_EXPECTED:
      return "Int key expected";
    case CborResolveError::KEY_NOT_FOUND:
      return "Key not found";
    case CborResolveError::CONTAINER_EXPECTED:
      return "Container expected";
    case CborResolveError::INT_KEY_TOO_BIG:
      return "Int key too big";
    default:
      return "Unknown error";
  }
}

namespace sgns::codec::cbor {
  IPFS::outcome::result<uint64_t> parseIndex(const std::string &str) {
    uint64_t value;
    size_t chars;
    try {
      value = std::stoul(str, &chars);
    } catch (std::invalid_argument &) {
      return IPFS::outcome::failure(CborResolveError::INT_KEY_EXPECTED);
    } catch (std::out_of_range &) {
      return IPFS::outcome::failure(CborResolveError::KEY_NOT_FOUND);
    }
    if (chars != str.size()) {
      return IPFS::outcome::failure(CborResolveError::INT_KEY_EXPECTED);
    }
    if (str[0] == '-') {
      return IPFS::outcome::failure(CborResolveError::INT_KEY_EXPECTED);
    }
    return value;
  }

  IPFS::outcome::result<void> resolve(CborDecodeStream &stream,
                                const std::string &part) {
    try {
      if (stream.isList()) {
        OUTCOME_TRY((auto &&, index), parseIndex(part));
        if (index >= stream.listLength()) {
          return CborResolveError::KEY_NOT_FOUND;
        }
        stream = stream.list();
        for (size_t i = 0; i < index; i++) {
          stream.next();
        }
      } else if (stream.isMap()) {
        auto map = stream.map();
        auto it = map.find(part);
        if (it == map.end()) {
          return CborResolveError::KEY_NOT_FOUND;
        }
        stream = std::move(it->second);
      } else {
        return CborResolveError::CONTAINER_EXPECTED;
      }
    } catch (std::system_error &e) {
      return IPFS::outcome::failure(e.code());
    }
    return IPFS::outcome::success();
  }
}  // namespace sgns::codec::cbor
