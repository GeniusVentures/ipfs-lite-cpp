#ifndef TEST_TESTUTIL_IPFS_LITE_BASE_MNN_TEST_HPP_
#define TEST_TESTUTIL_IPFS_LITE_BASE_MNN_TEST_HPP_

#include "testutil/ipfs_lite/base_fs_test.hpp"

#include "ipfs_lite/MNN/MNN_parsing.hpp"

namespace test
{

    struct BaseMNN_Test: public BaseFS_Test
    {
            using MNNParsing = sgns::ipfs_lite::MNNParsing;

            explicit BaseMNN_Test(const fs::path &path);

            void parsing();

            void SetUp() override;

            void TearDown() override;

            std::shared_ptr<MNNParsing> mnn_parsing_;
    };

}  // namespace test

#endif /* TEST_TESTUTIL_IPFS_LITE_BASE_MNN_TEST_HPP_ */
