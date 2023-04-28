
#include "ipfs_lite/MNN/MNN_error.hpp"

OUTCOME_CPP_DEFINE_CATEGORY_3(sgns::ipfs_lite, MNNError, e) {
  using E = sgns::ipfs_lite::MNNError;
  switch (e) {
    case E::NO_ERROR:
      return "success";
    case E::OUT_OF_MEMORY:
      return "Out of memory";
    case E::NOT_SUPPORT:
      return "Not support type";
    case E::COMPUTE_SIZE_ERROR:
      return "Compute size error";
    case E::NO_EXECUTION:
      return "No execution";
    case E::INVALID_VALUE:
      return "Invalid Value";
    case E::INPUT_DATA_ERROR:
        return "Input Data Error";
    case E::CALL_BACK_STOP:
        return "Call Back Stop"
    case E::TENSOR_NOT_SUPPORT:
        return "Tensor not support";
    case E::TENSOR_NEED_DIVIDE:
        return "Tensor need divide";
    case E::UNKNOW:
      break;
  }

  return "unknown error";
}
