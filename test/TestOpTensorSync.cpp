// SPDX-License-Identifier: Apache-2.0

#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"

TEST(TestOpTensorSync, SyncToDeviceMemorySingleTensor)
{

    kp::Manager mgr;

    std::vector<float> testVecPreA{ 0, 0, 0 };
    std::vector<float> testVecPostA{ 9, 8, 7 };

    std::shared_ptr<kp::TensorT<float>> tensorA = mgr.tensor(testVecPreA);

    EXPECT_TRUE(tensorA->isInit());

    tensorA->setData(testVecPostA);

    mgr.sequence()->eval<kp::OpTensorSyncDevice>({ tensorA });

    mgr.sequence()->eval<kp::OpTensorSyncLocal>({ tensorA });

    EXPECT_EQ(tensorA->vector(), testVecPostA);
}

TEST(TestOpTensorSync, SyncToDeviceMemoryMultiTensor)
{

    kp::Manager mgr;

    std::vector<float> testVec{ 9, 8, 7 };

    std::shared_ptr<kp::TensorT<float>> tensorA = mgr.tensor({ 0, 0, 0 });
    std::shared_ptr<kp::TensorT<float>> tensorB = mgr.tensor({ 0, 0, 0 });
    std::shared_ptr<kp::TensorT<float>> tensorC = mgr.tensor({ 0, 0, 0 });

    EXPECT_TRUE(tensorA->isInit());
    EXPECT_TRUE(tensorB->isInit());
    EXPECT_TRUE(tensorC->isInit());

    tensorA->setData(testVec);

    mgr.sequence()->eval<kp::OpTensorSyncDevice>({ tensorA });

    mgr.sequence()->eval<kp::OpTensorCopy>({ tensorA, tensorB, tensorC });

    mgr.sequence()->eval<kp::OpTensorSyncLocal>({ tensorA, tensorB, tensorC });

    EXPECT_EQ(tensorA->vector(), testVec);
    EXPECT_EQ(tensorB->vector(), testVec);
    EXPECT_EQ(tensorC->vector(), testVec);
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
