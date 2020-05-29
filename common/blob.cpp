#include "common/blob.hpp"

OUTCOME_CPP_DEFINE_CATEGORY_3(ipfs_lite::common, BlobError, e) {
  using ipfs_lite::common::BlobError;

  switch (e) {
    case BlobError::INCORRECT_LENGTH:
      return "Input string has incorrect length, not matching the blob size";
  }

  return "Unknown error";
}

namespace ipfs_lite::common {

  // explicit instantiations for the most frequently used blobs
  template class Blob<8ul>;
  template class Blob<16ul>;
  template class Blob<32ul>;
  template class Blob<64ul>;

}  // namespace ipfs_lite::common
