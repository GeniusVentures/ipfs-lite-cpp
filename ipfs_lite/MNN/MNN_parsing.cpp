#include "ipfs_lite/MNN/MNN_parsing.hpp"
#include "ipfs_lite/MNN/MNN_util.hpp"

namespace sgns::ipfs_lite
{
    MNNParsing::MNNParsing(const std::string &mnn_file_name, unsigned int num_thread = 1) : log_id_(mnn_file_name.data()), mnn_file_path_(
            mnn_file_name.data()), num_threads_(num_thread)
    {
        initialize();

    } // End MNNParsing()

    MNNParsing::~MNNParsing()
    {
        // We release the model
        mnn_interpreter_->releaseModel();
        if (mnn_session_)
        {
            // Release the session
            mnn_interpreter_->releaseSession(mnn_session_);
        }
    } // End ~MNNParsing

    outcome::result<void> MNNParsing::initialize()
    {
        // We need initialize the interpreter
        mnn_interpreter_ = std::shared_ptr<MNN::Interpreter>(MNN::Interpreter::createFromFile(mnn_file_path_));
        if (mnn_interpreter_ == nullptr)
        {
            return error_as_result<void>(MNNError::INPUT_DATA_ERROR, logger_);
        }
        // Schedule config
        schedule_config_.numThread = (int) num_threads_;
        MNN::BackendConfig backend_config;
        backend_config.precision = MNN::BackendConfig::Precision_High;
        schedule_config_.backendConfig = &backend_config;
        // Create session for reading model
        mnn_session_ = mnn_interpreter_->createSession(schedule_config_);
        // Tensor input and input dims
        input_tensor_ = mnn_interpreter_->getSessionInput(mnn_session_, nullptr);
        input_batch_ = input_tensor_->batch();
        input_channel_ = input_tensor_->channel();
        input_height_ = input_tensor_->height();
        input_width_ = input_tensor_->width();
        dimension_type_ = input_tensor_->getDimensionType();
        // Need to adapt interpreter base on dims
        switch (dimension_type_)
        {
            case MNN::Tensor::CAFFE:
                mnn_interpreter_->resizeTensor(input_tensor_, { input_channel_, input_height_, input_width_ });
                mnn_interpreter_->resizeSession(mnn_session_);
                break;
            case MNN::Tensor::TENSORFLOW:
                mnn_interpreter_->resizeTensor(input_tensor_, { input_batch_, input_height_, input_width_, input_channel_ });
                mnn_interpreter_->resizeSession(mnn_session_);
                break;
            default:
                // Any new type MNN support, we can adapt it later
                break;
        }
        num_output_ = mnn_interpreter_->getSessionOutputAll(mnn_session_).size();
        return outcome::success();
    } // End initialize()

    outcome::result<void> MNNParsing::debug_printing()
    {
        if (mnn_interpreter_ == nullptr)
        {
            return error_as_result<void>(MNNError::INPUT_DATA_ERROR, logger_);
        }
        logger_->debug("Log ID: {}", log_id_);
        logger_->debug("============INPUT-DIMS==============");
        if (input_tensor_)
        {
            input_tensor_->printShape();
        }
        switch (dimension_type_)
        {
            case MNN::Tensor::CAFFE:
                logger_->debug("Dimension Type: (CAFE/PyTorch/ONNX) uses NCHW as data format");
                break;
            case MNN::Tensor::TENSORFLOW:
                logger_->debug("Dimension Type: (TENSORFLOW) uses NHWC as data format");
                break;
            case MNN::Tensor::CAFFE_C4:
                logger->debug("Dimension Type: (CAFE_C4) uses NC4HW4 as data format");
                break;
            default:
                logger->debug("Dimension Type: UNKNOWN");
                break;
        }
        logger_->debug("============OUTPUT-DIMS=============");
        auto output_map = mnn_interpreter_->getSessionOutputAll(mnn_session_);
        logger_->debug("getSessionOutputAll() done!");
        for (auto it = output_map.cbegin(); it != output_map.cend(); it++)
        {
            logger->debug("Output: {}", it->first);
            it->second->printShape();
        }
        logger_->debug("====================================");
        return outcome::success();
    } // End debug_printing()
} // End namespace sgns::ipfs_lite
