#include "ipfs_lite/ipld/impl/ipld_node_decoder_pb.hpp"

OUTCOME_CPP_DEFINE_CATEGORY_3(sgns::ipfs_lite::ipld, IPLDNodeDecoderPBError, error) {
  using sgns::ipfs_lite::ipld::IPLDNodeDecoderPBError;
  switch (error) {
    case (IPLDNodeDecoderPBError::INVALID_RAW_BYTES):
      return "IPLD node Protobuf decoder: failed to deserialize from incorrect "
             "raw bytes";
  }
  return "IPLD node protobuf decoder: unknown error";
}
