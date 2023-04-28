#include "testutil/ipfs_lite/base_mnn_test.hpp"

namespace test
{

    class MNNAgeGoogleNet: public sgns::ipfs_lite::MNNParsing
    {
        public:
            explicit MNNAgeGoogleNet(const std::string &_mnn_path, unsigned int _num_threads = 1) : sgns::ipfs_lite::MNNParsing(_mnn_path,
                    _num_threads)
            {
            }
            ~MNNAgeGoogleNet() override = default;
    };

    BaseMNN_Test::BaseMNN_Test(const fs::path &path) : BaseFS_Test(path)
    {

    }

    void BaseMNN_Test::parsing()
    {
        mnn_parsing_ = std::make_shared<MNNAgeGoogleNet>(MNNAgeGoogleNet(getPathString()));
    }

    void BaseMNN_Test::SetUp()
    {
        parsing();
    }

    void BaseMNN_Test::TearDown()
    {
        clear();
    }
}  // namespace test
