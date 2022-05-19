// SPDX-License-Identifier: Apache-2.0

#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"

TEST(TestOpTensorCreate, CreateSingleTensorSingleOp)
{
    std::vector<float> testVecA{ 9, 8, 7 };
    std::shared_ptr<kp::TensorT<float>> tensorA = nullptr;

    {
        kp::Manager mgr;

        tensorA = mgr.tensor(testVecA);

        EXPECT_TRUE(tensorA->isInit());

        EXPECT_EQ(tensorA->vector(), testVecA);
    }

    EXPECT_FALSE(tensorA->isInit());
}

TEST(TestOpTensorCreate, NoErrorIfTensorFreedBefore)
{

    std::vector<float> testVecA{ 9, 8, 7 };
    std::vector<float> testVecB{ 6, 5, 4 };

    kp::Manager mgr;

    std::shared_ptr<kp::TensorT<float>> tensorA = mgr.tensor(testVecA);
    std::shared_ptr<kp::TensorT<float>> tensorB = mgr.tensor(testVecB);

    EXPECT_EQ(tensorA->vector(), testVecA);
    EXPECT_EQ(tensorB->vector(), testVecB);

    tensorA->destroy();
    tensorB->destroy();

    EXPECT_FALSE(tensorA->isInit());
    EXPECT_FALSE(tensorB->isInit());
}

TEST(TestOpTensorCreate, ExceptionOnZeroSizeTensor)
{
    std::vector<float> testVecA;

    kp::Manager mgr;

    try {
        std::shared_ptr<kp::TensorT<float>> tensorA = mgr.tensor(testVecA);
    } catch (const std::runtime_error& err) {
        // check exception
        ASSERT_TRUE(std::string(err.what()).find("zero-sized") !=
                    std::string::npos);
    }
}

int
main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);

#if KOMPUTE_ENABLE_SPDLOG
    spdlog::set_level(
      static_cast<spdlog::level::level_enum>(KOMPUTE_LOG_LEVEL));
#endif

    return RUN_ALL_TESTS();
}
