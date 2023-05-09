#include <gtest/gtest.h>

#include <boost/filesystem.hpp>

#include "ipfs_lite/MNN/MNN_parsing.hpp"
#include "ipfs_lite/MNN/MNN_error.hpp"
#include "testutil/outcome.hpp"
#include "testutil/ipfs_lite/base_fs_test.hpp"

namespace ipfs = sgns::ipfs_lite;

struct MNN_Open_Invalid: public test::BaseMNN_Test
{
        MNN_Open_Invalid() : test::BaseMNN_Test("sgns_mnn_parsing")
        {
        }
};

struct MNN_Open_Valid: public test::BaseMNN_Test
{
        MNN_Open_Valid() : test::BaseMNN_Test("test_mnn.onnx")
        {
        }
};

/**
 * @given invalid input file
 * @when parsing file
 * @then return input data error
 */
TEST_F(MNN_Open_Invalid, OpenInvalidFile)
{
    logger_->warn("1. parsing file....{}", getPathString());
    auto r = mnn_parsing_->debug_printing();
    EXPECT_EQ(r.error().value(), (int )ipfs::MNNError::INPUT_DATA_ERROR);
}

/**
 * @given valid input file
 * @when parsing file
 * @then return success
 */
TEST_F(MNN_Open_Valid, OpenValidFile)
{
    logger_->warn("1. parsing file....{}", getPathString());
    auto r = mnn_parsing_->debug_printing();
    ASSERT_NE(r.error().value(), (int )ipfs::MNNError::INPUT_DATA_ERROR);
}
