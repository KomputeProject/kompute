// SPDX-License-Identifier: Apache-2.0

#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"
#include "kompute/logger/Logger.hpp"

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

TEST(TestOpTensorSync, SyncToDeviceMemoryCopyRegion)
{
    kp::Manager mgr;

    std::vector<float> testVecPreA{ 1, 2, 3, 4 };
    std::vector<float> testVecPostA{ 1, 1, 1, 4 };
    std::vector<float> testVecPostB{ 0, 0, 0, 1 };

    std::shared_ptr<kp::TensorT<float>> tensorA = mgr.tensor({ 0, 0, 0, 0 });
    std::shared_ptr<kp::TensorT<float>> tensorB = mgr.tensor({ 0, 0, 0, 0 });

    EXPECT_TRUE(tensorA->isInit());
    EXPECT_TRUE(tensorB->isInit());

    mgr.sequence()->eval<kp::OpTensorSyncDevice>({ tensorA, tensorB });

    // tensorA local: { 0, 0, 0, 0 }  |  tensorA device: { 0, 0, 0, 0 }
    // tensorB local: { 0, 0, 0, 0 }  |  tensorB device: { 0, 0, 0, 0 }

    tensorA->setData(testVecPreA);
    
    // tensorA local: { 1, 2, 3, 4 }  |  tensorA device: { 0, 0, 0, 0 }
    // tensorB local: { 0, 0, 0, 0 }  |  tensorB device: { 0, 0, 0, 0 }

    // Copy from tensorA local index 0 to tensorA device index 1 (1 element)
    mgr.sequence()->eval<kp::OpTensorSyncRegionDevice>({{ tensorA, 0, 1, 1 }});
    
    // tensorA local: { 1, 2, 3, 4 }  |  tensorA device: { 0, 1, 0, 0 }
    // tensorB local: { 0, 0, 0, 0 }  |  tensorB device: { 0, 0, 0, 0 }

    // Copy from tensorA device index 1 to tensorA device index 2 (1 element)
    // Copy from tensorA device index 1 to tensorB device index 2 (1 element)
    mgr.sequence()->eval<kp::OpTensorCopyRegion>({ tensorA, {{ tensorA, 1, 2, 1 }, { tensorB, 1, 2, 1 }}});
    
    // tensorA local: { 1, 2, 3, 4 }  |  tensorA device: { 0, 1, 1, 0 }
    // tensorB local: { 0, 0, 0, 0 }  |  tensorB device: { 0, 0, 1, 0 }

    // Copy from tensorA device index 1 to tensorA local index 1 (2 elements)
    // Copy from tensorB device index 2 to tensorB local index 3 (1 element)
    mgr.sequence()->eval<kp::OpTensorSyncRegionLocal>({{ tensorA, 1, 1, 2 }, { tensorB, 2, 3, 1 }});
    
    // tensorA local: { 1, 1, 1, 4 }  |  tensorA device: { 0, 1, 1, 0 }
    // tensorB local: { 0, 0, 1, 0 }  |  tensorB device: { 0, 0, 1, 0 }

    EXPECT_EQ(tensorA->vector(), testVecPostA);
    EXPECT_EQ(tensorB->vector(), testVecPostB);
}
