// SPDX-License-Identifier: Apache-2.0

#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"

#include "kompute_test/Shader.hpp"

TEST(TestOpTensorCopy, CopyDeviceToDeviceTensor)
{

    kp::Manager mgr;

    std::vector<float> testVecA{ 1, 2, 3 };
    std::vector<float> testVecB{ 0, 0, 0 };

    std::shared_ptr<kp::TensorT<float>> tensorA = mgr.tensor(testVecA);
    std::shared_ptr<kp::TensorT<float>> tensorB = mgr.tensor(testVecB);

    EXPECT_TRUE(tensorA->isInit());
    EXPECT_TRUE(tensorB->isInit());

    mgr.sequence()
      ->eval<kp::OpTensorSyncDevice>({ tensorA, tensorB })
      ->eval<kp::OpTensorCopy>({ tensorA, tensorB })
      ->eval<kp::OpTensorSyncLocal>({ tensorA, tensorB });

    // Making sure the GPU holds the same vector
    EXPECT_EQ(tensorA->vector(), tensorB->vector());
}

TEST(TestOpTensorCopy, CopyDeviceToDeviceTensorMulti)
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
      ->eval<kp::OpTensorSyncLocal>({ tensorA, tensorB, tensorC })
      ->eval<kp::OpTensorCopy>({ tensorA, tensorB, tensorC });

    EXPECT_EQ(tensorA->vector(), tensorB->vector());
    EXPECT_EQ(tensorA->vector(), tensorC->vector());

    // Making sure the GPU holds the same vector
    mgr.sequence()->eval<kp::OpTensorSyncLocal>({ tensorB, tensorC });

    EXPECT_EQ(tensorA->vector(), tensorB->vector());
    EXPECT_EQ(tensorA->vector(), tensorC->vector());
}

TEST(TestOpTensorCopy, CopyDeviceToHostTensor)
{

    kp::Manager mgr;

    std::vector<float> testVecA{ 3, 4, 5 };
    std::vector<float> testVecB{ 0, 0, 0 };

    std::shared_ptr<kp::TensorT<float>> tensorA = mgr.tensor(testVecA);
    std::shared_ptr<kp::TensorT<float>> tensorB =
      mgr.tensor(testVecB, kp::Tensor::TensorTypes::eHost);

    //  Only calling sync on device type tensor
    mgr.sequence()->eval<kp::OpTensorSyncDevice>({ tensorA });

    EXPECT_TRUE(tensorA->isInit());
    EXPECT_TRUE(tensorB->isInit());

    mgr.sequence()->eval<kp::OpTensorCopy>({ tensorA, tensorB });

    EXPECT_EQ(tensorA->vector(), tensorB->vector());

    // Making sure the GPU holds the same vector
    mgr.sequence()->eval<kp::OpTensorSyncLocal>({ tensorB });
    EXPECT_EQ(tensorA->vector(), tensorB->vector());
}

TEST(TestOpTensorCopy, CopyHostToDeviceTensor)
{

    kp::Manager mgr;

    std::vector<float> testVecA{ 4, 5, 6 };
    std::vector<float> testVecB{ 0, 0, 0 };

    std::shared_ptr<kp::TensorT<float>> tensorA =
      mgr.tensor(testVecA, kp::Tensor::TensorTypes::eHost);
    std::shared_ptr<kp::TensorT<float>> tensorB = mgr.tensor(testVecB);

    //  Only calling sync on device type tensor
    mgr.sequence()->eval<kp::OpTensorSyncDevice>({ tensorA, tensorB });

    EXPECT_TRUE(tensorA->isInit());
    EXPECT_TRUE(tensorB->isInit());

    mgr.sequence()->eval<kp::OpTensorCopy>({ tensorA, tensorB });

    EXPECT_EQ(tensorA->vector(), tensorB->vector());

    // Making sure the GPU holds the same vector
    mgr.sequence()->eval<kp::OpTensorSyncLocal>({ tensorB });
    EXPECT_EQ(tensorA->vector(), tensorB->vector());
}

TEST(TestOpTensorCopy, CopyHostToHostTensor)
{

    kp::Manager mgr;

    std::vector<float> testVecA{ 5, 6, 7 };
    std::vector<float> testVecB{ 0, 0, 0 };

    std::shared_ptr<kp::TensorT<float>> tensorA =
      mgr.tensor(testVecA, kp::Tensor::TensorTypes::eHost);
    std::shared_ptr<kp::TensorT<float>> tensorB =
      mgr.tensor(testVecB, kp::Tensor::TensorTypes::eHost);

    EXPECT_TRUE(tensorA->isInit());
    EXPECT_TRUE(tensorB->isInit());

    mgr.sequence()
      ->eval<kp::OpTensorSyncDevice>({ tensorA })
      ->eval<kp::OpTensorCopy>({ tensorA, tensorB });

    EXPECT_EQ(tensorA->vector(), tensorB->vector());

    // Making sure the GPU holds the same vector
    mgr.sequence()->eval<kp::OpTensorSyncLocal>({ tensorB });
    EXPECT_EQ(tensorA->vector(), tensorB->vector());
}

TEST(TestOpTensorCopy, SingleTensorShouldFail)
{

    kp::Manager mgr;

    std::vector<float> testVecA{ 6, 7, 8 };

    std::shared_ptr<kp::TensorT<float>> tensorA =
      mgr.tensor(testVecA, kp::Tensor::TensorTypes::eHost);

    EXPECT_TRUE(tensorA->isInit());

    EXPECT_THROW(mgr.sequence()->eval<kp::OpTensorCopy>({ tensorA }),
                 std::runtime_error);
}

TEST(TestOpTensorCopy, eStorageTensorCopy)
{
    kp::Manager mgr;

    const auto vec_in = std::vector<float>{ 0.0, 1.2, 3.2 };
    const auto vec_out_with_wrong_values =
      std::vector<float>(vec_in.size(), 0.0);

    auto tensor_in = mgr.tensor(vec_in, kp::Tensor::TensorTypes::eDevice);
    auto tensor_temp = mgr.tensor(nullptr,
                                  vec_in.size(),
                                  sizeof(decltype(vec_in.back())),
                                  kp::Tensor::TensorDataTypes::eFloat,
                                  kp::Tensor::TensorTypes::eStorage);
    auto tensor_out = mgr.tensor(vec_out_with_wrong_values, kp::Tensor::TensorTypes::eDevice);

    EXPECT_TRUE(tensor_in->isInit());
    EXPECT_TRUE(tensor_temp->isInit());
    EXPECT_TRUE(tensor_out->isInit());

    mgr.sequence()
      ->eval<kp::OpTensorSyncDevice>({tensor_in})
      ->eval<kp::OpTensorCopy>({tensor_in, tensor_temp})
      ->eval<kp::OpTensorCopy>({tensor_temp, tensor_out})
      ->eval<kp::OpTensorSyncLocal>({tensor_out});

    EXPECT_EQ(tensor_in->vector(), tensor_out->vector());
    
}