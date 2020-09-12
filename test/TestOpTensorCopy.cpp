
#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"

TEST(TestOpTensorCopy, CopyDeviceToDeviceTensor)
{

    kp::Manager mgr;

    std::vector<float> testVecA{ 9, 8, 7 };
    std::vector<float> testVecB{ 0, 0, 0 };

    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor(testVecA) };
    std::shared_ptr<kp::Tensor> tensorB{ new kp::Tensor(testVecB) };

    mgr.evalOpDefault<kp::OpTensorCreate>({ tensorA, tensorB });

    EXPECT_TRUE(tensorA->isInit());
    EXPECT_TRUE(tensorB->isInit());

    mgr.evalOpDefault<kp::OpTensorCopy>({ tensorA, tensorB });

    EXPECT_EQ(tensorA->data(), tensorB->data());

    // Making sure the GPU holds the same data
    mgr.evalOpDefault<kp::OpTensorSyncLocal>({ tensorB });
    EXPECT_EQ(tensorA->data(), tensorB->data());
}

TEST(TestOpTensorCopy, CopyDeviceToDeviceTensorMulti)
{

    kp::Manager mgr;

    std::vector<float> testVecA{ 9, 8, 7 };
    std::vector<float> testVecB{ 0, 0, 0 };
    std::vector<float> testVecC{ 0, 0, 0 };

    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor(testVecA) };
    std::shared_ptr<kp::Tensor> tensorB{ new kp::Tensor(testVecB) };
    std::shared_ptr<kp::Tensor> tensorC{ new kp::Tensor(testVecC) };

    mgr.evalOpDefault<kp::OpTensorCreate>({ tensorA, tensorB, tensorC });

    EXPECT_TRUE(tensorA->isInit());
    EXPECT_TRUE(tensorB->isInit());
    EXPECT_TRUE(tensorC->isInit());

    mgr.evalOpDefault<kp::OpTensorCopy>({ tensorA, tensorB, tensorC });

    EXPECT_EQ(tensorA->data(), tensorB->data());
    EXPECT_EQ(tensorA->data(), tensorC->data());

    // Making sure the GPU holds the same data
    mgr.evalOpDefault<kp::OpTensorSyncLocal>({ tensorB, tensorC });
    EXPECT_EQ(tensorA->data(), tensorB->data());
    EXPECT_EQ(tensorA->data(), tensorC->data());
}

TEST(TestOpTensorCopy, CopyDeviceToStagingTensor)
{

    kp::Manager mgr;

    std::vector<float> testVecA{ 9, 8, 7 };
    std::vector<float> testVecB{ 0, 0, 0 };

    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor(testVecA) };
    std::shared_ptr<kp::Tensor> tensorB{ new kp::Tensor(
      testVecB, kp::Tensor::TensorTypes::eStaging) };

    mgr.evalOpDefault<kp::OpTensorCreate>({ tensorA, tensorB });

    EXPECT_TRUE(tensorA->isInit());
    EXPECT_TRUE(tensorB->isInit());

    mgr.evalOpDefault<kp::OpTensorCopy>({ tensorA, tensorB });

    EXPECT_EQ(tensorA->data(), tensorB->data());

    // Making sure the GPU holds the same data
    mgr.evalOpDefault<kp::OpTensorSyncLocal>({ tensorB });
    EXPECT_EQ(tensorA->data(), tensorB->data());
}

TEST(TestOpTensorCopy, CopyStagingToDeviceTensor)
{

    kp::Manager mgr;

    std::vector<float> testVecA{ 9, 8, 7 };
    std::vector<float> testVecB{ 0, 0, 0 };

    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor(
      testVecA, kp::Tensor::TensorTypes::eStaging) };
    std::shared_ptr<kp::Tensor> tensorB{ new kp::Tensor(testVecB) };

    mgr.evalOpDefault<kp::OpTensorCreate>({ tensorA, tensorB });

    EXPECT_TRUE(tensorA->isInit());
    EXPECT_TRUE(tensorB->isInit());

    mgr.evalOpDefault<kp::OpTensorCopy>({ tensorA, tensorB });

    EXPECT_EQ(tensorA->data(), tensorB->data());

    // Making sure the GPU holds the same data
    mgr.evalOpDefault<kp::OpTensorSyncLocal>({ tensorB });
    EXPECT_EQ(tensorA->data(), tensorB->data());
}

TEST(TestOpTensorCopy, CopyStagingToStagingTensor)
{

    kp::Manager mgr;

    std::vector<float> testVecA{ 9, 8, 7 };
    std::vector<float> testVecB{ 0, 0, 0 };

    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor(
      testVecA, kp::Tensor::TensorTypes::eStaging) };
    std::shared_ptr<kp::Tensor> tensorB{ new kp::Tensor(
      testVecB, kp::Tensor::TensorTypes::eStaging) };

    mgr.evalOpDefault<kp::OpTensorCreate>({ tensorA, tensorB });

    EXPECT_TRUE(tensorA->isInit());
    EXPECT_TRUE(tensorB->isInit());

    mgr.evalOpDefault<kp::OpTensorCopy>({ tensorA, tensorB });

    EXPECT_EQ(tensorA->data(), tensorB->data());

    // Making sure the GPU holds the same data
    mgr.evalOpDefault<kp::OpTensorSyncLocal>({ tensorB });
    EXPECT_EQ(tensorA->data(), tensorB->data());
}

TEST(TestOpTensorCopy, SingleTensorShouldFail)
{

    kp::Manager mgr;

    std::vector<float> testVecA{ 9, 8, 7 };

    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor(
      testVecA, kp::Tensor::TensorTypes::eStaging) };

    mgr.evalOpDefault<kp::OpTensorCreate>({ tensorA });

    EXPECT_TRUE(tensorA->isInit());

    EXPECT_THROW(mgr.evalOpDefault<kp::OpTensorCopy>({ tensorA }),
                 std::runtime_error);
}
