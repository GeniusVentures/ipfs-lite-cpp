
#include "primitives/cid/comm_cid_errors.hpp"

OUTCOME_CPP_DEFINE_CATEGORY_3(sgns::common, CommCidError, e) {
  using sgns::common::CommCidError;

  switch (e) {
    case (CommCidError::TOO_SHORT):
      return "CommCid: multihash too short";
    case (CommCidError::CANT_READ_CODE):
      return "CommCid: cannot read code from multihash";
    case (CommCidError::CANT_READ_LENGTH):
      return "CommCid: cannot read length from multihash";
    case (CommCidError::DATA_TOO_BIG):
      return "CommCid: digest too long, supporting only <= 2^31-1";
    case (CommCidError::DIFFERENT_LENGTH):
      return "CommCid: message length does not match declared length";
    case (CommCidError::INVALID_HASH):
      return "CommCid: incorrect hashing function for data commitment";
    default:
      return "Piece: unknown error";
  }
}
