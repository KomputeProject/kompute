
#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"

TEST(TestOpTensorCopy, CopyDeviceToDeviceTensor)
{

    kp::Manager mgr;

    std::vector<float> testVecA{ 9, 8, 7 };
    std::vector<float> testVecB{ 0, 0, 0 };

    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor(testVecA) };
    std::shared_ptr<kp::Tensor> tensorB{ new kp::Tensor(testVecB) };

    mgr.rebuild({ tensorA, tensorB });

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

    mgr.rebuild({ tensorA, tensorB, tensorC });

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

TEST(TestOpTensorCopy, CopyDeviceToHostTensor)
{

    kp::Manager mgr;

    std::vector<float> testVecA{ 9, 8, 7 };
    std::vector<float> testVecB{ 0, 0, 0 };

    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor(testVecA) };
    std::shared_ptr<kp::Tensor> tensorB{ new kp::Tensor(
      testVecB, kp::Tensor::TensorTypes::eHost) };

    mgr.rebuild({ tensorA, tensorB }, false);

    //  Only calling sync on device type tensor
    mgr.evalOpDefault<kp::OpTensorSyncDevice>({ tensorA });

    EXPECT_TRUE(tensorA->isInit());
    EXPECT_TRUE(tensorB->isInit());

    mgr.evalOpDefault<kp::OpTensorCopy>({ tensorA, tensorB });

    EXPECT_EQ(tensorA->data(), tensorB->data());

    // Making sure the GPU holds the same data
    mgr.evalOpDefault<kp::OpTensorSyncLocal>({ tensorB });
    EXPECT_EQ(tensorA->data(), tensorB->data());
}

TEST(TestOpTensorCopy, CopyHostToDeviceTensor)
{

    kp::Manager mgr;

    std::vector<float> testVecA{ 9, 8, 7 };
    std::vector<float> testVecB{ 0, 0, 0 };

    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor(
      testVecA, kp::Tensor::TensorTypes::eHost) };
    std::shared_ptr<kp::Tensor> tensorB{ new kp::Tensor(testVecB) };

    mgr.rebuild({ tensorA, tensorB }, false);

    //  Only calling sync on device type tensor
    mgr.evalOpDefault<kp::OpTensorSyncDevice>({ tensorB });

    EXPECT_TRUE(tensorA->isInit());
    EXPECT_TRUE(tensorB->isInit());

    mgr.evalOpDefault<kp::OpTensorCopy>({ tensorA, tensorB });

    EXPECT_EQ(tensorA->data(), tensorB->data());

    // Making sure the GPU holds the same data
    mgr.evalOpDefault<kp::OpTensorSyncLocal>({ tensorB });
    EXPECT_EQ(tensorA->data(), tensorB->data());
}

TEST(TestOpTensorCopy, CopyHostToHostTensor)
{

    kp::Manager mgr;

    std::vector<float> testVecA{ 9, 8, 7 };
    std::vector<float> testVecB{ 0, 0, 0 };

    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor(
      testVecA, kp::Tensor::TensorTypes::eHost) };
    std::shared_ptr<kp::Tensor> tensorB{ new kp::Tensor(
      testVecB, kp::Tensor::TensorTypes::eHost) };

    mgr.rebuild({ tensorA, tensorB });

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
      testVecA, kp::Tensor::TensorTypes::eHost) };

    mgr.rebuild({ tensorA }, false);

    EXPECT_TRUE(tensorA->isInit());

    EXPECT_THROW(mgr.evalOpDefault<kp::OpTensorCopy>({ tensorA }),
                 std::runtime_error);
}
