#include "common/outcome.hpp"
#include "common/buffer.hpp"
#include "common/logger.hpp"

#ifndef IPFS_LITE_MNN_MNN_ERROR_HPP_
#define IPFS_LITE_MNN_MNN_ERROR_HPP_
namespace sgns::ipfs_lite
{
    /**
     * Wrapper <MNN/ErrorCode.hpp>
     */
    enum class MNNError
    {
        NO_ERROR = 0,
        OUT_OF_MEMORY = 1,
        NOT_SUPPORT = 2,
        COMPUTE_SIZE_ERROR = 3,
        NO_EXECUTION = 4,
        INVALID_VALUE = 5,
        INPUT_DATA_ERROR = 10,
        CALL_BACK_STOP = 11,
        TENSOR_NOT_SUPPORT = 20,
        TENSOR_NEED_DIVIDE = 21,
        UNKNOWN = 22,
    };
} // End namespace
OUTCOME_HPP_DECLARE_ERROR_2(sgns::ipfs_lite, MNNError);
#endif /* IPFS_LITE_MNN_MNN_ERROR_HPP_ */
