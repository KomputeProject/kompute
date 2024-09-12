// SPDX-License-Identifier: Apache-2.0

#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"
#include "kompute/logger/Logger.hpp"

#include "shaders/Utils.hpp"

TEST(TestOpCopyImageToTensor, CopyDeviceToDeviceTensor)
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
      ->eval<kp::OpCopy>({ image, tensor })
      ->eval<kp::OpSyncLocal>({ tensor })
      ->eval<kp::OpSyncLocal>({ image });

    // Making sure the GPU holds the same vector
    EXPECT_EQ(tensor->vector(), image->vector());
}

TEST(TestOpCopyImageToTensor, CopyDeviceToDeviceTensorMulti)
{

    kp::Manager mgr;

    std::vector<float> testVecA{ 2, 3, 4 };
    std::vector<float> testVecB{ 0, 0, 0 };
    std::vector<float> testVecC{ 0, 0, 0 };

    std::shared_ptr<kp::ImageT<float>> imageA =
      mgr.image(testVecA, testVecB.size(), 1, 1);
    std::shared_ptr<kp::TensorT<float>> tensorB = mgr.tensor(testVecB);
    std::shared_ptr<kp::TensorT<float>> tensorC = mgr.tensor(testVecC);

    EXPECT_TRUE(imageA->isInit());
    EXPECT_TRUE(tensorB->isInit());
    EXPECT_TRUE(tensorC->isInit());

    mgr.sequence()
      ->eval<kp::OpSyncDevice>({ imageA })
      ->eval<kp::OpCopy>({ imageA, tensorB, tensorC })
      ->eval<kp::OpSyncLocal>({ tensorB, tensorC });

    EXPECT_EQ(testVecA, imageA->vector());
    EXPECT_EQ(testVecA, tensorB->vector());
    EXPECT_EQ(testVecA, tensorC->vector());
}

TEST(TestOpCopyImageToTensor, CopyDeviceToHostTensor)
{

    kp::Manager mgr;

    std::vector<float> testVecA{ 3, 4, 5 };
    std::vector<float> testVecB{ 0, 0, 0 };

    std::shared_ptr<kp::ImageT<float>> imageA =
      mgr.image(testVecA, testVecA.size(), 1, 1);
    std::shared_ptr<kp::TensorT<float>> tensorB =
      mgr.tensor(testVecB, kp::Memory::MemoryTypes::eHost);

    //  Only calling sync on device type tensor
    mgr.sequence()->eval<kp::OpSyncDevice>({ imageA });

    EXPECT_TRUE(imageA->isInit());
    EXPECT_TRUE(tensorB->isInit());

    mgr.sequence()->eval<kp::OpCopy>({ imageA, tensorB });

    EXPECT_EQ(imageA->vector(), tensorB->vector());

    // Making sure the GPU holds the same vector
    mgr.sequence()->eval<kp::OpSyncLocal>({ tensorB });
    EXPECT_EQ(imageA->vector(), tensorB->vector());
}

TEST(TestOpCopyImageToTensor, CopyHostToDeviceTensor)
{

    kp::Manager mgr;

    std::vector<float> testVecA{ 4, 5, 6 };
    std::vector<float> testVecB{ 0, 0, 0 };

    std::shared_ptr<kp::ImageT<float>> imageA = mgr.image(
      testVecA, testVecA.size(), 1, 1, kp::Memory::MemoryTypes::eHost);
    std::shared_ptr<kp::TensorT<float>> tensorB = mgr.tensor(testVecB);

    //  Only calling sync on device type tensor
    mgr.sequence()->eval<kp::OpSyncDevice>({ imageA });
    mgr.sequence()->eval<kp::OpSyncDevice>({ tensorB });

    EXPECT_TRUE(imageA->isInit());
    EXPECT_TRUE(tensorB->isInit());

    mgr.sequence()->eval<kp::OpCopy>({ imageA, tensorB });

    EXPECT_EQ(imageA->vector(), tensorB->vector());

    // Making sure the GPU holds the same vector
    mgr.sequence()->eval<kp::OpSyncLocal>({ tensorB });
    EXPECT_EQ(imageA->vector(), tensorB->vector());
}

TEST(TestOpCopyImageToTensor, CopyHostToHostTensor)
{

    kp::Manager mgr;

    std::vector<float> testVecA{ 5, 6, 7 };
    std::vector<float> testVecB{ 0, 0, 0 };

    std::shared_ptr<kp::ImageT<float>> imageA = mgr.image(
      testVecA, testVecA.size(), 1, 1, kp::Memory::MemoryTypes::eHost);
    std::shared_ptr<kp::TensorT<float>> tensorB =
      mgr.tensor(testVecB, kp::Memory::MemoryTypes::eHost);

    EXPECT_TRUE(imageA->isInit());
    EXPECT_TRUE(tensorB->isInit());

    mgr.sequence()
      ->eval<kp::OpSyncDevice>({ imageA })
      ->eval<kp::OpCopy>({ imageA, tensorB });

    EXPECT_EQ(imageA->vector(), tensorB->vector());

    // Making sure the GPU holds the same vector
    mgr.sequence()->eval<kp::OpSyncLocal>({ tensorB });
    EXPECT_EQ(imageA->vector(), tensorB->vector());
}

TEST(TestOpCopyImageToTensor, SingleTensorShouldFail)
{

    kp::Manager mgr;

    std::vector<float> testVecA{ 6, 7, 8 };

    std::shared_ptr<kp::TensorT<float>> tensorA =
      mgr.tensor(testVecA, kp::Memory::MemoryTypes::eHost);

    EXPECT_TRUE(tensorA->isInit());

    EXPECT_THROW(mgr.sequence()->eval<kp::OpCopy>({ tensorA }),
                 std::runtime_error);
}

TEST(TestOpCopyImageToTensor, CopyThroughStorageTensor)
{
    kp::Manager mgr;

    std::vector<float> testVecIn{ 9, 1, 3 };
    std::vector<float> testVecOut{ 0, 0, 0 };

    std::shared_ptr<kp::TensorT<float>> tensorOut = mgr.tensor(testVecOut);
    std::shared_ptr<kp::ImageT<float>> imageIn =
      mgr.image(testVecIn, testVecIn.size(), 1, 1);
    // Image storage requires a vector to be passed only to reflect size
    std::shared_ptr<kp::ImageT<float>> imageStorage = mgr.image(
      testVecIn, testVecIn.size(), 1, 1, kp::Memory::MemoryTypes::eStorage);

    mgr.sequence()
      ->eval<kp::OpSyncDevice>({ imageIn })
      ->eval<kp::OpSyncDevice>({ tensorOut })
      ->eval<kp::OpCopy>({ imageIn, imageStorage })
      ->eval<kp::OpCopy>({ imageStorage, tensorOut })
      ->eval<kp::OpSyncLocal>({ imageIn })
      ->eval<kp::OpSyncLocal>({ tensorOut });

    // Making sure the GPU holds the same vector
    EXPECT_EQ(imageIn->vector(), tensorOut->vector());
}

TEST(TestOpCopyImageToTensor, CopyDeviceToDeviceImageUninitialised)
{
    kp::Manager mgr;

    std::vector<float> testVecA{ 1, 2, 3 };

    std::shared_ptr<kp::ImageT<float>> imageA =
      mgr.image(testVecA, testVecA.size(), 1, 1);
    std::shared_ptr<kp::TensorT<float>> tensorB =
      mgr.tensorT<float>(testVecA.size());

    EXPECT_TRUE(imageA->isInit());
    EXPECT_TRUE(tensorB->isInit());

    mgr.sequence()
      ->eval<kp::OpSyncDevice>({ imageA })
      ->eval<kp::OpSyncDevice>({ tensorB })
      ->eval<kp::OpCopy>({ imageA, tensorB })
      ->eval<kp::OpSyncLocal>({
        imageA,
      })
      ->eval<kp::OpSyncLocal>({ tensorB });

    // Making sure the GPU holds the same vector
    EXPECT_EQ(imageA->vector(), tensorB->vector());
}

TEST(TestOpImageCopyToTensor, CopyDeviceAndHostToDeviceAndHostTensor)
{
    kp::Manager mgr;

    std::vector<float> testVecA{ 1, 2, 3 };
    std::vector<float> testVecB{ 0, 0, 0 };

    std::shared_ptr<kp::ImageT<float>> imageA = mgr.image(
      testVecA, testVecA.size(), 1, 1, kp::Memory::MemoryTypes::eDeviceAndHost);
    std::shared_ptr<kp::TensorT<float>> tensorB =
      mgr.tensor(testVecB, kp::Memory::MemoryTypes::eDeviceAndHost);

    EXPECT_TRUE(imageA->isInit());
    EXPECT_TRUE(tensorB->isInit());

    mgr.sequence()
      ->eval<kp::OpSyncDevice>({ imageA })
      ->eval<kp::OpSyncDevice>({ tensorB })
      ->eval<kp::OpCopy>({ imageA, tensorB })
      ->eval<kp::OpSyncLocal>({ imageA })
      ->eval<kp::OpSyncLocal>({ tensorB });

    // Making sure the GPU holds the same vector
    EXPECT_EQ(imageA->vector(), tensorB->vector());
}
