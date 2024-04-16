
#include "primitives/cid/comm_cid.hpp"
#include "common/outcome.hpp"
#include "primitives/cid/comm_cid_errors.hpp"

namespace sgns::common {

  using libp2p::multi::HashType;

  CID replicaCommitmentV1ToCID(gsl::span<const uint8_t> comm_r) {
    _OUTCOME_EXCEPT_1(cid, commitmentToCID(comm_r, FC_SEALED_V1));
    // return cid;
    return cid.value();
  }

  CID dataCommitmentV1ToCID(gsl::span<const uint8_t> comm_d) {
    _OUTCOME_EXCEPT_1(cid, commitmentToCID(comm_d, FC_UNSEALED_V1));
    // return cid;
    return cid.value();
  }

  CID pieceCommitmentV1ToCID(gsl::span<const uint8_t> comm_p) {
    return dataCommitmentV1ToCID(comm_p);
  }

  bool validFilecoinMultihash(FilecoinMultihashCode code) {
    return kFilecoinMultihashNames.find(code) != kFilecoinMultihashNames.end();
  }

  IPFS::outcome::result<CID> commitmentToCID(gsl::span<const uint8_t> commitment,
                                       FilecoinMultihashCode code) {
    if (!validFilecoinMultihash(code)) {
      return CommCidError::INVALID_HASH;
    }

    OUTCOME_TRY((auto &&, mh), Multihash::create(static_cast<HashType>(code), commitment));

    return CID(
        libp2p::multi::ContentIdentifier::Version::V1, kFilecoinCodecType, mh);
  }

  IPFS::outcome::result<Comm> CIDToPieceCommitmentV1(const CID &cid) {
    return CIDToDataCommitmentV1(cid);
  }

  IPFS::outcome::result<Comm> CIDToDataCommitmentV1(const CID &cid) {
    OUTCOME_TRY((auto &&, result), CIDToCommitment(cid));
    if (static_cast<FilecoinHashType>(result.getType()) != FC_UNSEALED_V1) {
      return CommCidError::INVALID_HASH;
    }
    return Comm::fromSpan(result.getHash());
  }

  IPFS::outcome::result<Multihash> CIDToCommitment(const CID &cid) {
    if (!validFilecoinMultihash(cid.content_address.getType())) {
      return CommCidError::INVALID_HASH;
    }
    return cid.content_address;
  }

  IPFS::outcome::result<Comm> CIDToReplicaCommitmentV1(const CID &cid) {
    OUTCOME_TRY((auto &&, result), CIDToCommitment(cid));
    if (static_cast<FilecoinHashType>(result.getType()) != FC_SEALED_V1) {
      return CommCidError::INVALID_HASH;
    }
    return Comm::fromSpan(result.getHash());
  }
}  // namespace sgns::common
