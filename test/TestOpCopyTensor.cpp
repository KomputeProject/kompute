// SPDX-License-Identifier: Apache-2.0

#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"
#include "kompute/logger/Logger.hpp"

#include "shaders/Utils.hpp"

TEST(TestOpCopyTensor, CopyDeviceToDeviceTensor)
{

    kp::Manager mgr;

    std::vector<float> testVecA{ 1, 2, 3 };
    std::vector<float> testVecB{ 0, 0, 0 };

    std::shared_ptr<kp::TensorT<float>> tensorA = mgr.tensor(testVecA);
    std::shared_ptr<kp::TensorT<float>> tensorB = mgr.tensor(testVecB);

    EXPECT_TRUE(tensorA->isInit());
    EXPECT_TRUE(tensorB->isInit());

    mgr.sequence()
      ->eval<kp::OpSyncDevice>({ tensorA, tensorB })
      ->eval<kp::OpCopy>({ tensorA, tensorB })
      ->eval<kp::OpSyncLocal>({ tensorA, tensorB });

    // Making sure the GPU holds the same vector
    EXPECT_EQ(tensorA->vector(), tensorB->vector());
}

TEST(TestOpCopyTensor, CopyDeviceToDeviceTensorMulti)
{

    kp::Manager mgr;

    std::vector<float> testVecA{ 2, 3, 4 };
    std::vector<float> testVecB{ 0, 0, 0 };
    std::vector<float> testVecC{ 0, 0, 0 };

    std::shared_ptr<kp::TensorT<float>> tensorA = mgr.tensor(testVecA);
    std::shared_ptr<kp::TensorT<float>> tensorB = mgr.tensor(testVecB);
    std::shared_ptr<kp::TensorT<float>> tensorC = mgr.tensor(testVecC);

    EXPECT_TRUE(tensorA->isInit());
    EXPECT_TRUE(tensorB->isInit());
    EXPECT_TRUE(tensorC->isInit());

    mgr.sequence()
      ->eval<kp::OpSyncDevice>({ tensorA })
      ->eval<kp::OpCopy>({ tensorA, tensorB, tensorC })
      ->eval<kp::OpSyncLocal>({ tensorB, tensorC });

    EXPECT_EQ(testVecA, tensorA->vector());
    EXPECT_EQ(testVecA, tensorB->vector());
    EXPECT_EQ(testVecA, tensorC->vector());
}

TEST(TestOpCopyTensor, CopyDeviceToHostTensor)
{

    kp::Manager mgr;

    std::vector<float> testVecA{ 3, 4, 5 };
    std::vector<float> testVecB{ 0, 0, 0 };

    std::shared_ptr<kp::TensorT<float>> tensorA = mgr.tensor(testVecA);
    std::shared_ptr<kp::TensorT<float>> tensorB =
      mgr.tensor(testVecB, kp::Memory::MemoryTypes::eHost);

    //  Only calling sync on device type tensor
    mgr.sequence()->eval<kp::OpSyncDevice>({ tensorA });

    EXPECT_TRUE(tensorA->isInit());
    EXPECT_TRUE(tensorB->isInit());

    mgr.sequence()->eval<kp::OpCopy>({ tensorA, tensorB });

    EXPECT_EQ(tensorA->vector(), tensorB->vector());

    // Making sure the GPU holds the same vector
    mgr.sequence()->eval<kp::OpSyncLocal>({ tensorB });
    EXPECT_EQ(tensorA->vector(), tensorB->vector());
}

TEST(TestOpCopyTensor, CopyHostToDeviceTensor)
{

    kp::Manager mgr;

    std::vector<float> testVecA{ 4, 5, 6 };
    std::vector<float> testVecB{ 0, 0, 0 };

    std::shared_ptr<kp::TensorT<float>> tensorA =
      mgr.tensor(testVecA, kp::Memory::MemoryTypes::eHost);
    std::shared_ptr<kp::TensorT<float>> tensorB = mgr.tensor(testVecB);

    //  Only calling sync on device type tensor
    mgr.sequence()->eval<kp::OpSyncDevice>({ tensorA, tensorB });

    EXPECT_TRUE(tensorA->isInit());
    EXPECT_TRUE(tensorB->isInit());

    mgr.sequence()->eval<kp::OpCopy>({ tensorA, tensorB });

    EXPECT_EQ(tensorA->vector(), tensorB->vector());

    // Making sure the GPU holds the same vector
    mgr.sequence()->eval<kp::OpSyncLocal>({ tensorB });
    EXPECT_EQ(tensorA->vector(), tensorB->vector());
}

TEST(TestOpCopyTensor, CopyHostToHostTensor)
{
    kp::Manager mgr;

    std::vector<float> testVecA{ 5, 6, 7 };
    std::vector<float> testVecB{ 0, 0, 0 };

    std::shared_ptr<kp::TensorT<float>> tensorA =
      mgr.tensor(testVecA, kp::Memory::MemoryTypes::eHost);
    std::shared_ptr<kp::TensorT<float>> tensorB =
      mgr.tensor(testVecB, kp::Memory::MemoryTypes::eHost);

    EXPECT_TRUE(tensorA->isInit());
    EXPECT_TRUE(tensorB->isInit());

    mgr.sequence()
      ->eval<kp::OpSyncDevice>({ tensorA })
      ->eval<kp::OpCopy>({ tensorA, tensorB });

    EXPECT_EQ(tensorA->vector(), tensorB->vector());

    // Making sure the GPU holds the same vector
    mgr.sequence()->eval<kp::OpSyncLocal>({ tensorB });
    EXPECT_EQ(tensorA->vector(), tensorB->vector());
}

TEST(TestOpCopyTensor, SingleTensorShouldFail)
{

    kp::Manager mgr;

    std::vector<float> testVecA{ 6, 7, 8 };

    std::shared_ptr<kp::TensorT<float>> tensorA =
      mgr.tensor(testVecA, kp::Memory::MemoryTypes::eHost);

    EXPECT_TRUE(tensorA->isInit());

    EXPECT_THROW(mgr.sequence()->eval<kp::OpCopy>({ tensorA }),
                 std::runtime_error);
}

TEST(TestOpCopyTensor, MismatchedTensorSizesShouldFail)
{
    kp::Manager mgr;

    std::vector<float> testVecA{ 1, 2, 3 };
    std::vector<float> testVecB{ 0, 0, 0, 0 };

    std::shared_ptr<kp::TensorT<float>> tensorA = mgr.tensor(testVecA);
    std::shared_ptr<kp::TensorT<float>> tensorB = mgr.tensor(testVecB);

    EXPECT_TRUE(tensorA->isInit());
    EXPECT_TRUE(tensorB->isInit());

    EXPECT_THROW(mgr.sequence()->eval<kp::OpCopy>({ tensorA, tensorB }),
                 std::runtime_error);
}

TEST(TestOpCopyTensor, MismatchedDataTypesShouldFail)
{
    kp::Manager mgr;

    std::vector<float> testVecA{ 1, 2, 3 };
    std::vector<int> testVecB{ 0, 0, 0 };

    std::shared_ptr<kp::TensorT<float>> tensorA = mgr.tensor(testVecA);
    std::shared_ptr<kp::TensorT<int>> tensorB = mgr.tensorT<int>(testVecB);

    EXPECT_TRUE(tensorA->isInit());
    EXPECT_TRUE(tensorB->isInit());

    EXPECT_THROW(mgr.sequence()->eval<kp::OpCopy>({ tensorA, tensorB }),
                 std::runtime_error);
}

TEST(TestOpCopyTensor, CopyThroughStorageTensor)
{
    kp::Manager mgr;

    std::vector<float> testVecIn{ 9, 1, 3 };
    std::vector<float> testVecOut{ 0, 0, 0 };

    std::shared_ptr<kp::TensorT<float>> tensorIn = mgr.tensor(testVecIn);
    std::shared_ptr<kp::TensorT<float>> tensorOut = mgr.tensor(testVecOut);
    // Tensor storage requires a vector to be passed only to reflect size
    std::shared_ptr<kp::TensorT<float>> tensorStorage =
      mgr.tensor({ 0, 0, 0 }, kp::Memory::MemoryTypes::eStorage);

    mgr.sequence()
      ->eval<kp::OpSyncDevice>({ tensorIn, tensorOut })
      ->eval<kp::OpCopy>({ tensorIn, tensorStorage })
      ->eval<kp::OpCopy>({ tensorStorage, tensorOut })
      ->eval<kp::OpSyncLocal>({ tensorIn, tensorOut });

    // Making sure the GPU holds the same vector
    EXPECT_EQ(tensorIn->vector(), tensorOut->vector());
}

TEST(TestOpCopyTensor, CopyTensorThroughStorageViaAlgorithms)
{
    kp::Manager mgr;

    std::vector<float> testVecIn{ 9, 1, 3 };
    std::vector<float> testVecOut{ 0, 0, 0 };

    std::shared_ptr<kp::TensorT<float>> tensorIn = mgr.tensor(testVecIn);
    std::shared_ptr<kp::TensorT<float>> tensorOut = mgr.tensor(testVecOut);
    // Tensor storage requires a vector to be passed only to reflect size
    std::shared_ptr<kp::TensorT<float>> tensorStorage =
      mgr.tensor({ 0, 0, 0 }, kp::Memory::MemoryTypes::eStorage);
    EXPECT_TRUE(tensorIn->isInit());
    EXPECT_TRUE(tensorOut->isInit());

    // Copy to storage tensor through algorithm
    std::string shaderA = (R"(
        #version 450

        layout (local_size_x = 1) in;

        // The input tensors bind index is relative to index in parameter passed
        layout(set = 0, binding = 0) buffer buf_in { float t_in[]; };
        layout(set = 0, binding = 1) buffer buf_st { float t_st[]; };

        void main() {
            uint index = gl_GlobalInvocationID.x;
            t_st[index] = t_in[index];
        }
    )");

    auto algoA =
      mgr.algorithm({ tensorIn, tensorStorage }, compileSource(shaderA));

    // Copy from storage tensor to output tensor
    std::string shaderB = (R"(
        #version 450

        layout (local_size_x = 1) in;

        // The input tensors bind index is relative to index in parameter passed
        layout(set = 0, binding = 0) buffer buf_st { float t_st[]; };
        layout(set = 0, binding = 1) buffer buf_out { float t_out[]; };

        void main() {
            uint index = gl_GlobalInvocationID.x;
            t_out[index] = t_st[index];
        }
    )");

    auto algoB =
      mgr.algorithm({ tensorStorage, tensorOut }, compileSource(shaderB));

    mgr.sequence()
      ->eval<kp::OpSyncDevice>({ tensorIn })
      ->eval<kp::OpAlgoDispatch>(algoA)
      ->eval<kp::OpAlgoDispatch>(algoB)
      ->eval<kp::OpSyncLocal>({ tensorOut });

    // Making sure the GPU holds the same vector
    EXPECT_EQ(tensorIn->vector(), tensorOut->vector());
}

TEST(TestOpCopyTensor, CopyDeviceToDeviceTensorUninitialised)
{
    kp::Manager mgr;

    std::vector<float> testVecA{ 1, 2, 3 };

    std::shared_ptr<kp::TensorT<float>> tensorA = mgr.tensor(testVecA);
    std::shared_ptr<kp::TensorT<float>> tensorB =
      mgr.tensorT<float>(testVecA.size());

    EXPECT_TRUE(tensorA->isInit());
    EXPECT_TRUE(tensorB->isInit());

    mgr.sequence()
      ->eval<kp::OpSyncDevice>({ tensorA, tensorB })
      ->eval<kp::OpCopy>({ tensorA, tensorB })
      ->eval<kp::OpSyncLocal>({ tensorA, tensorB });

    // Making sure the GPU holds the same vector
    EXPECT_EQ(tensorA->vector(), tensorB->vector());
}

TEST(TestOpCopyTensor, CopyTensorThroughStorageViaAlgorithmsUninitialisedOutput)
{
    kp::Manager mgr;

    std::vector<float> testVecIn{ 9, 1, 3 };

    std::shared_ptr<kp::TensorT<float>> tensorIn = mgr.tensor(testVecIn);
    std::shared_ptr<kp::TensorT<float>> tensorOut =
      mgr.tensorT<float>(testVecIn.size());
    // Tensor storage requires a vector to be passed only to reflect size
    std::shared_ptr<kp::Memory> tensorStorage =
      mgr.tensorT<float>(testVecIn.size(), kp::Memory::MemoryTypes::eStorage);

    EXPECT_TRUE(tensorIn->isInit());
    EXPECT_TRUE(tensorOut->isInit());

    // Copy to storage tensor through algorithm
    std::string shaderA = (R"(
        #version 450

        layout (local_size_x = 1) in;

        // The input tensors bind index is relative to index in parameter passed
        layout(set = 0, binding = 0) buffer buf_in { float t_in[]; };
        layout(set = 0, binding = 1) buffer buf_st { float t_st[]; };

        void main() {
            uint index = gl_GlobalInvocationID.x;
            t_st[index] = t_in[index];
        }
    )");

    auto algoA =
      mgr.algorithm({ tensorIn, tensorStorage }, compileSource(shaderA));

    // Copy from storage tensor to output tensor
    std::string shaderB = (R"(
        #version 450

        layout (local_size_x = 1) in;

        // The input tensors bind index is relative to index in parameter passed
        layout(set = 0, binding = 0) buffer buf_st { float t_st[]; };
        layout(set = 0, binding = 1) buffer buf_out { float t_out[]; };

        void main() {
            uint index = gl_GlobalInvocationID.x;
            t_out[index] = t_st[index];
        }
    )");

    auto algoB =
      mgr.algorithm({ tensorStorage, tensorOut }, compileSource(shaderB));

    mgr.sequence()
      ->eval<kp::OpSyncDevice>({ tensorIn })
      ->eval<kp::OpAlgoDispatch>(algoA)
      ->eval<kp::OpAlgoDispatch>(algoB)
      ->eval<kp::OpSyncLocal>({ tensorOut });

    // Making sure the GPU holds the same vector
    EXPECT_EQ(tensorIn->vector(), tensorOut->vector());
}

TEST(TestOpTensorCopy, CopyDeviceAndHostToDeviceAndHostTensor)
{
    kp::Manager mgr;

    std::vector<float> testVecA{ 1, 2, 3 };
    std::vector<float> testVecB{ 0, 0, 0 };

    std::shared_ptr<kp::TensorT<float>> tensorA =
      mgr.tensor(testVecA, kp::Memory::MemoryTypes::eDeviceAndHost);
    std::shared_ptr<kp::TensorT<float>> tensorB =
      mgr.tensor(testVecB, kp::Memory::MemoryTypes::eDeviceAndHost);

    EXPECT_TRUE(tensorA->isInit());
    EXPECT_TRUE(tensorB->isInit());

    mgr.sequence()
      ->eval<kp::OpSyncDevice>({ tensorA, tensorB })
      ->eval<kp::OpCopy>({ tensorA, tensorB })
      ->eval<kp::OpSyncLocal>({ tensorA, tensorB });

    // Making sure the GPU holds the same vector
    EXPECT_EQ(tensorA->vector(), tensorB->vector());
}
