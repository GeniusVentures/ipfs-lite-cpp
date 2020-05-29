#include "codec/cbor/cbor_errors.hpp"

OUTCOME_CPP_DEFINE_CATEGORY_3(ipfs_lite::codec::cbor, CborEncodeError, e) {
  using ipfs_lite::codec::cbor::CborEncodeError;
  switch (e) {
    case CborEncodeError::INVALID_CID:
      return "Invalid CID";
    case CborEncodeError::EXPECTED_MAP_VALUE_SINGLE:
      return "Expected map value single";
    default:
      return "Unknown error";
  }
}

OUTCOME_CPP_DEFINE_CATEGORY_3(ipfs_lite::codec::cbor, CborDecodeError, e) {
  using ipfs_lite::codec::cbor::CborDecodeError;
  switch (e) {
    case CborDecodeError::INVALID_CBOR:
      return "Invalid CBOR";
    case CborDecodeError::WRONG_TYPE:
      return "Wrong type";
    case CborDecodeError::INT_OVERFLOW:
      return "Int overflow";
    case CborDecodeError::INVALID_CBOR_CID:
      return "Invalid CBOR CID";
    case CborDecodeError::INVALID_CID:
      return "Invalid CID";
    case CborDecodeError::WRONG_SIZE:
      return "Wrong size";
    default:
      return "Unknown error";
  }
}
