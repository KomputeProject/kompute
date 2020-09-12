
#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"

TEST(TestOpTensorCreate, CreateSingleTensorSingleOp)
{

    kp::Manager mgr;

    std::vector<float> testVecA{ 9, 8, 7 };

    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor(testVecA) };

    mgr.evalOpDefault<kp::OpTensorCreate>({ tensorA });

    EXPECT_TRUE(tensorA->isInit());

    EXPECT_EQ(tensorA->data(), testVecA);

    tensorA->freeMemoryDestroyGPUResources();
    EXPECT_FALSE(tensorA->isInit());
}

TEST(TestOpTensorCreate, CreateMultipleTensorSingleOp)
{

    kp::Manager mgr;

    std::vector<float> testVecA{ 9, 8, 7 };
    std::vector<float> testVecB{ 6, 5, 4 };

    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor(testVecA) };
    std::shared_ptr<kp::Tensor> tensorB{ new kp::Tensor(testVecB) };

    mgr.evalOpDefault<kp::OpTensorCreate>({ tensorA, tensorB });

    EXPECT_TRUE(tensorA->isInit());
    EXPECT_TRUE(tensorB->isInit());

    EXPECT_EQ(tensorA->data(), testVecA);
    EXPECT_EQ(tensorB->data(), testVecB);
}

TEST(TestOpTensorCreate, CreateMultipleTensorMultipleOp)
{

    kp::Manager mgr;

    std::vector<float> testVecA{ 9, 8, 7 };
    std::vector<float> testVecB{ 6, 5, 4 };

    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor(testVecA) };
    std::shared_ptr<kp::Tensor> tensorB{ new kp::Tensor(testVecB) };

    mgr.evalOpDefault<kp::OpTensorCreate>({ tensorA });
    mgr.evalOpDefault<kp::OpTensorCreate>({ tensorB });

    EXPECT_TRUE(tensorA->isInit());
    EXPECT_TRUE(tensorB->isInit());

    EXPECT_EQ(tensorA->data(), testVecA);
    EXPECT_EQ(tensorB->data(), testVecB);
}

TEST(TestOpTensorCreate, ManageTensorMemoryWhenOpTensorCreateDestroyed)
{

    std::vector<float> testVecA{ 9, 8, 7 };
    std::vector<float> testVecB{ 6, 5, 4 };

    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor(testVecA) };
    std::shared_ptr<kp::Tensor> tensorB{ new kp::Tensor(testVecB) };

    {
        kp::Manager mgr;
        mgr.evalOpDefault<kp::OpTensorCreate>({ tensorA });
        mgr.evalOpDefault<kp::OpTensorCreate>({ tensorB });

        EXPECT_TRUE(tensorA->isInit());
        EXPECT_TRUE(tensorB->isInit());

        EXPECT_EQ(tensorA->data(), testVecA);
        EXPECT_EQ(tensorB->data(), testVecB);
    }

    EXPECT_FALSE(tensorA->isInit());
    EXPECT_FALSE(tensorB->isInit());
}

TEST(TestOpTensorCreate, NoErrorIfTensorFreedBefore)
{

    std::vector<float> testVecA{ 9, 8, 7 };
    std::vector<float> testVecB{ 6, 5, 4 };

    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor(testVecA) };
    std::shared_ptr<kp::Tensor> tensorB{ new kp::Tensor(testVecB) };

    kp::Manager mgr;

    mgr.evalOpDefault<kp::OpTensorCreate>({ tensorA });
    mgr.evalOpDefault<kp::OpTensorCreate>({ tensorB });

    EXPECT_TRUE(tensorA->isInit());
    EXPECT_TRUE(tensorB->isInit());

    EXPECT_EQ(tensorA->data(), testVecA);
    EXPECT_EQ(tensorB->data(), testVecB);

    tensorA->freeMemoryDestroyGPUResources();
    tensorB->freeMemoryDestroyGPUResources();
    EXPECT_FALSE(tensorA->isInit());
    EXPECT_FALSE(tensorB->isInit());
}
