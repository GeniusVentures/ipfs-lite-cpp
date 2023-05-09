#ifndef CPP_IPFS_LITE_MNN_MNN_PARSING_HPP
#define CPP_IPFS_LITE_MNN_MNN_PARSING_HPP

#include <memory>
#include <string>
#include "ipfs_lite/MNN/MNN_config.hpp"


namespace sgns::ipfs_lite
{
    /**
     * This is class Parsing all information of MNN model file.
     * If you want to use this class, we can inheritance from this class and implement logic base on model info
     */
    class MNNParsing
    {
        public:
            outcome::result<void> debug_printing();
        protected:
            // MNN Information
            std::shared_ptr<MNN::Interpreter> mnn_interpreter_; // The holder of the model data
            MNN::Session *mnn_session_ = nullptr; // The holder of the inference data
            MNN::Tensor *input_tensor_ = nullptr; //
            MNN::ScheduleConfig schedule_config_;
            const char *log_id_ = nullptr;
            const char *mnn_file_path_ = nullptr;
            // Configuration for multi threading input
            const unsigned int num_threads_;
            int input_batch_;
            int input_channel_;
            int input_height_;
            int input_width_;
            MNN::Tensor::DimensionType dimension_type_;
            int num_output_;
        protected:
            explicit MNNParsing(const std::string &mnn_file_name, unsigned int num_thread = 1);
            virtual ~MNNParsing();

            // Non-Copy
            MNNParsing(const MNNParsing&) = delete;
            MNNParsing(MNNParsing&&) = delete;
            MNNParsing& operator=(const MNNParsing&) = delete;
            MNNParsing& operator=(MNNParsing&&) = delete;
        private:
            outcome::result<void> initialize();
        private:
            common::Logger logger_ = sgns::common::createLogger("MNN");
    };
// End class MNNParsing
}

#endif /* CPP_IPFS_LITE_MNN_MNN_PARSING_HPP */
