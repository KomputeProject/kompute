// SPDX-License-Identifier: Apache-2.0

#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"
#include "kompute/logger/Logger.hpp"

TEST(TestOpSync, SyncToDeviceMemorySingleTensor)
{
    kp::Manager mgr;

    std::vector<float> testVecPreA{ 0, 0, 0 };
    std::vector<float> testVecPostA{ 9, 8, 7 };

    std::shared_ptr<kp::TensorT<float>> tensorA = mgr.tensor(testVecPreA);

    EXPECT_TRUE(tensorA->isInit());

    tensorA->setData(testVecPostA);

    mgr.sequence()->eval<kp::OpSyncDevice>({ tensorA });

    mgr.sequence()->eval<kp::OpSyncLocal>({ tensorA });

    EXPECT_EQ(tensorA->vector(), testVecPostA);
}

TEST(TestOpSync, SyncToDeviceMemoryMultiTensor)
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

    mgr.sequence()->eval<kp::OpSyncDevice>({ tensorA });

    mgr.sequence()->eval<kp::OpCopy>({ tensorA, tensorB, tensorC });

    mgr.sequence()->eval<kp::OpSyncLocal>({ tensorA, tensorB, tensorC });

    EXPECT_EQ(tensorA->vector(), testVec);
    EXPECT_EQ(tensorB->vector(), testVec);
    EXPECT_EQ(tensorC->vector(), testVec);
}

TEST(TestOpSync, SyncToDeviceMemorySingleImage)
{
    kp::Manager mgr;

    std::vector<float> testVecPreA{ 0, 0, 0 };
    std::vector<float> testVecPostA{ 9, 8, 7 };

    std::shared_ptr<kp::ImageT<float>> imageA = mgr.image(testVecPreA, 3, 1, 1);

    EXPECT_TRUE(imageA->isInit());

    imageA->setData(testVecPostA);

    mgr.sequence()->eval<kp::OpSyncDevice>({ imageA });

    mgr.sequence()->eval<kp::OpSyncLocal>({ imageA });

    EXPECT_EQ(imageA->vector(), testVecPostA);
}

TEST(TestOpSync, SyncToDeviceMemoryMultiImage)
{
    kp::Manager mgr;

    std::vector<float> testVec{ 9, 8, 7 };

    std::shared_ptr<kp::ImageT<float>> imageA = mgr.image({ 0, 0, 0 }, 3, 1, 1);
    std::shared_ptr<kp::ImageT<float>> imageB = mgr.image({ 0, 0, 0 }, 3, 1, 1);
    std::shared_ptr<kp::ImageT<float>> imageC = mgr.image({ 0, 0, 0 }, 3, 1, 1);

    EXPECT_TRUE(imageA->isInit());
    EXPECT_TRUE(imageB->isInit());
    EXPECT_TRUE(imageC->isInit());

    imageA->setData(testVec);

    mgr.sequence()->eval<kp::OpSyncDevice>({ imageA });

    mgr.sequence()->eval<kp::OpCopy>({ imageA, imageB, imageC });

    mgr.sequence()->eval<kp::OpSyncLocal>({ imageA, imageB, imageC });

    EXPECT_EQ(imageA->vector(), testVec);
    EXPECT_EQ(imageB->vector(), testVec);
    EXPECT_EQ(imageC->vector(), testVec);
}

TEST(TestOpSync, NegativeUnrelatedImageSync)
{
    kp::Manager mgr;

    std::vector<float> testVecIn{ 9, 1, 3 };
    std::vector<float> testVecOut{ 0, 0, 0 };

    std::shared_ptr<kp::ImageT<float>> ImageIn = mgr.image(testVecIn, 3, 1, 1);
    std::shared_ptr<kp::ImageT<float>> ImageOut =
      mgr.image(testVecOut, 3, 1, 1);

    EXPECT_TRUE(ImageIn->isInit());
    EXPECT_TRUE(ImageOut->isInit());

    // Syncing one image to the device should not update an unrelated one
    mgr.sequence()
      ->eval<kp::OpSyncDevice>({ ImageIn })
      ->eval<kp::OpSyncLocal>({ ImageOut });

    // Making sure the GPU holds the same vector
    EXPECT_NE(ImageIn->vector(), ImageOut->vector());
}
