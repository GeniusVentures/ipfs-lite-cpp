
#include "codec/rle/rle_plus_errors.hpp"

OUTCOME_CPP_DEFINE_CATEGORY_3(sgns::codec::rle, RLEPlusDecodeError, e) {
  using sgns::codec::rle::RLEPlusDecodeError;
  switch (e) {
    case (RLEPlusDecodeError::VersionMismatch):
      return "RLE+ data header has invalid version";
    case (RLEPlusDecodeError::DataIndexFailure):
      return "RLE+ incorrect structure";
    case (RLEPlusDecodeError::UnpackOverflow):
      return "RLE+ invalid encoding";
    case (RLEPlusDecodeError::MaxSizeExceed):
      return "RLE+ object size too large";
    default:
      return "RLE+ unknown error";
  }
}
