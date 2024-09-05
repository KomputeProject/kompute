// SPDX-License-Identifier: Apache-2.0

#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"
#include "kompute/logger/Logger.hpp"

#include "shaders/Utils.hpp"

TEST(TestOpCopyTensorToImage, CopyDeviceToDeviceTensor)
{

    kp::Manager mgr;

    std::vector<float> testVecA{ 1, 2, 3 };
    std::vector<float> testVecB{ 0, 0, 0 };

    std::shared_ptr<kp::TensorT<float>> tensor = mgr.tensor(testVecA);
    std::shared_ptr<kp::ImageT<float>> image =
      mgr.image(testVecB, testVecB.size(), 1, 1);

    EXPECT_TRUE(tensor->isInit());
    EXPECT_TRUE(image->isInit());

    mgr.sequence()
      ->eval<kp::OpSyncDevice>({ tensor })
      ->eval<kp::OpSyncDevice>({ image })
      ->eval<kp::OpCopy>({ tensor, image })
      ->eval<kp::OpSyncLocal>({ tensor })
      ->eval<kp::OpSyncLocal>({ image });

    // Making sure the GPU holds the same vector
    EXPECT_EQ(tensor->vector(), image->vector());
}

TEST(TestOpCopyTensorToImage, CopyDeviceToDeviceTensorMulti)
{

    kp::Manager mgr;

    std::vector<float> testVecA{ 2, 3, 4 };
    std::vector<float> testVecB{ 0, 0, 0 };
    std::vector<float> testVecC{ 0, 0, 0 };

    std::shared_ptr<kp::TensorT<float>> tensorA = mgr.tensor(testVecA);
    std::shared_ptr<kp::ImageT<float>> imageB =
      mgr.image(testVecB, testVecB.size(), 1, 1);
    std::shared_ptr<kp::ImageT<float>> imageC =
      mgr.image(testVecC, testVecC.size(), 1, 1);

    EXPECT_TRUE(tensorA->isInit());
    EXPECT_TRUE(imageB->isInit());
    EXPECT_TRUE(imageC->isInit());

    mgr.sequence()
      ->eval<kp::OpSyncDevice>({ tensorA })
      ->eval<kp::OpCopy>({ tensorA, imageB, imageC })
      ->eval<kp::OpSyncLocal>({ imageB, imageC });

    // Making sure the GPU holds the same vector
    EXPECT_EQ(testVecA, tensorA->vector());
    EXPECT_EQ(testVecA, imageB->vector());
    EXPECT_EQ(testVecA, imageC->vector());
}

TEST(TestOpCopyTensorToImage, CopyDeviceToHostTensor)
{

    kp::Manager mgr;

    std::vector<float> testVecA{ 3, 4, 5 };
    std::vector<float> testVecB{ 0, 0, 0 };

    std::shared_ptr<kp::TensorT<float>> tensorA = mgr.tensor(testVecA);
    std::shared_ptr<kp::ImageT<float>> imageB = mgr.image(
      testVecB, testVecB.size(), 1, 1, kp::Memory::MemoryTypes::eHost);

    //  Only calling sync on device type tensor
    mgr.sequence()->eval<kp::OpSyncDevice>({ tensorA });

    EXPECT_TRUE(tensorA->isInit());
    EXPECT_TRUE(imageB->isInit());

    mgr.sequence()->eval<kp::OpCopy>({ tensorA, imageB });

    EXPECT_EQ(tensorA->vector(), imageB->vector());

    // Making sure the GPU holds the same vector
    mgr.sequence()->eval<kp::OpSyncLocal>({ imageB });
    EXPECT_EQ(tensorA->vector(), imageB->vector());
}

TEST(TestOpCopyTensorToImage, CopyHostToDeviceTensor)
{

    kp::Manager mgr;

    std::vector<float> testVecA{ 4, 5, 6 };
    std::vector<float> testVecB{ 0, 0, 0 };

    std::shared_ptr<kp::TensorT<float>> tensorA =
      mgr.tensor(testVecA, kp::Memory::MemoryTypes::eHost);
    std::shared_ptr<kp::ImageT<float>> imageB =
      mgr.image(testVecB, testVecB.size(), 1, 1);

    //  Only calling sync on device type tensor
    mgr.sequence()->eval<kp::OpSyncDevice>({ tensorA });
    mgr.sequence()->eval<kp::OpSyncDevice>({ imageB });

    EXPECT_TRUE(tensorA->isInit());
    EXPECT_TRUE(imageB->isInit());

    mgr.sequence()->eval<kp::OpCopy>({ tensorA, imageB });

    EXPECT_EQ(tensorA->vector(), imageB->vector());

    // Making sure the GPU holds the same vector
    mgr.sequence()->eval<kp::OpSyncLocal>({ imageB });
    EXPECT_EQ(tensorA->vector(), imageB->vector());
}

TEST(TestOpCopyTensorToImage, CopyHostToHostTensor)
{

    kp::Manager mgr;

    std::vector<float> testVecA{ 5, 6, 7 };
    std::vector<float> testVecB{ 0, 0, 0 };

    std::shared_ptr<kp::TensorT<float>> tensorA =
      mgr.tensor(testVecA, kp::Memory::MemoryTypes::eHost);
    std::shared_ptr<kp::ImageT<float>> imageB = mgr.image(
      testVecB, testVecB.size(), 1, 1, kp::Memory::MemoryTypes::eHost);

    EXPECT_TRUE(tensorA->isInit());
    EXPECT_TRUE(imageB->isInit());

    mgr.sequence()
      ->eval<kp::OpSyncDevice>({ tensorA })
      ->eval<kp::OpCopy>({ tensorA, imageB });

    EXPECT_EQ(tensorA->vector(), imageB->vector());

    // Making sure the GPU holds the same vector
    mgr.sequence()->eval<kp::OpSyncLocal>({ imageB });
    EXPECT_EQ(tensorA->vector(), imageB->vector());
}

TEST(TestOpCopyTensorToImage, SingleTensorShouldFail)
{

    kp::Manager mgr;

    std::vector<float> testVecA{ 6, 7, 8 };

    std::shared_ptr<kp::TensorT<float>> tensorA =
      mgr.tensor(testVecA, kp::Memory::MemoryTypes::eHost);

    EXPECT_TRUE(tensorA->isInit());

    EXPECT_THROW(mgr.sequence()->eval<kp::OpCopy>({ tensorA }),
                 std::runtime_error);
}

TEST(TestOpCopyTensorToImage, CopyThroughStorageTensor)
{
    kp::Manager mgr;

    std::vector<float> testVecIn{ 9, 1, 3 };
    std::vector<float> testVecOut{ 0, 0, 0 };

    std::shared_ptr<kp::TensorT<float>> tensorIn = mgr.tensor(testVecIn);
    std::shared_ptr<kp::ImageT<float>> imageOut =
      mgr.image(testVecOut, testVecOut.size(), 1, 1);
    // Tensor storage requires a vector to be passed only to reflect size
    std::shared_ptr<kp::TensorT<float>> tensorStorage =
      mgr.tensor({ 0, 0, 0 }, kp::Memory::MemoryTypes::eStorage);

    mgr.sequence()
      ->eval<kp::OpSyncDevice>({ tensorIn })
      ->eval<kp::OpSyncDevice>({ imageOut })
      ->eval<kp::OpCopy>({ tensorIn, tensorStorage })
      ->eval<kp::OpCopy>({ tensorStorage, imageOut })
      ->eval<kp::OpSyncLocal>({ tensorIn })
      ->eval<kp::OpSyncLocal>({ imageOut });

    // Making sure the GPU holds the same vector
    EXPECT_EQ(tensorIn->vector(), imageOut->vector());
}

TEST(TestOpCopyTensorToImage, CopyDeviceToDeviceImageUninitialised)
{
    kp::Manager mgr;

    std::vector<float> testVecA{ 1, 2, 3 };

    std::shared_ptr<kp::TensorT<float>> tensorA = mgr.tensor(testVecA);
    std::shared_ptr<kp::ImageT<float>> imageB =
      mgr.imageT<float>(testVecA.size(), 1, 1);

    EXPECT_TRUE(tensorA->isInit());
    EXPECT_TRUE(imageB->isInit());

    mgr.sequence()
      ->eval<kp::OpSyncDevice>({ tensorA })
      ->eval<kp::OpSyncDevice>({ imageB })
      ->eval<kp::OpCopy>({ tensorA, imageB })
      ->eval<kp::OpSyncLocal>({
        tensorA,
      })
      ->eval<kp::OpSyncLocal>({ imageB });

    // Making sure the GPU holds the same vector
    EXPECT_EQ(tensorA->vector(), imageB->vector());
}

TEST(TestOpTensorCopyToImage, CopyDeviceAndHostToDeviceAndHostTensor)
{
    kp::Manager mgr;

    std::vector<float> testVecA{ 1, 2, 3 };
    std::vector<float> testVecB{ 0, 0, 0 };

    std::shared_ptr<kp::TensorT<float>> tensorA =
      mgr.tensor(testVecA, kp::Memory::MemoryTypes::eDeviceAndHost);
    std::shared_ptr<kp::ImageT<float>> imageB = mgr.image(
      testVecB, testVecB.size(), 1, 1, kp::Memory::MemoryTypes::eDeviceAndHost);

    EXPECT_TRUE(tensorA->isInit());
    EXPECT_TRUE(imageB->isInit());

    mgr.sequence()
      ->eval<kp::OpSyncDevice>({ tensorA })
      ->eval<kp::OpSyncDevice>({ imageB })
      ->eval<kp::OpCopy>({ tensorA, imageB })
      ->eval<kp::OpSyncLocal>({ tensorA })
      ->eval<kp::OpSyncLocal>({ imageB });

    // Making sure the GPU holds the same vector
    EXPECT_EQ(tensorA->vector(), imageB->vector());
}
