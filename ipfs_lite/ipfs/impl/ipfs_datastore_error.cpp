#include "ipfs_lite/ipfs/ipfs_datastore_error.hpp"

OUTCOME_CPP_DEFINE_CATEGORY_3(sgns::ipfs_lite::ipfs, IpfsDatastoreError, e) {
  using sgns::ipfs_lite::ipfs::IpfsDatastoreError;

  switch (e) {
    case IpfsDatastoreError::NOT_FOUND:
      return "IpfsDatastoreError: cid not found";
    case IpfsDatastoreError::UNKNOWN:
      break;
  }

  return "unknown error";
}
