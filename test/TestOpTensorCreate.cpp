
#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"

//TEST(TestOpTensorCreate, CreateSingleTensorSingleOp)
//{
//    std::vector<float> testVecA{ 9, 8, 7 };
//    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor(testVecA) };
//
//    {
//        kp::Manager mgr;
//
//        mgr.rebuild({ tensorA });
//
//        EXPECT_TRUE(tensorA->isInit());
//
//        EXPECT_EQ(tensorA->data(), testVecA);
//    }
//
//    EXPECT_FALSE(tensorA->isInit());
//}
//
//TEST(TestOpTensorCreate, CreateMultipleTensorSingleOp)
//{
//
//    kp::Manager mgr;
//
//    std::vector<float> testVecA{ 9, 8, 7 };
//    std::vector<float> testVecB{ 6, 5, 4 };
//
//    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor(testVecA) };
//    std::shared_ptr<kp::Tensor> tensorB{ new kp::Tensor(testVecB) };
//
//    mgr.rebuild({ tensorA, tensorB });
//
//    EXPECT_TRUE(tensorA->isInit());
//    EXPECT_TRUE(tensorB->isInit());
//
//    EXPECT_EQ(tensorA->data(), testVecA);
//    EXPECT_EQ(tensorB->data(), testVecB);
//}
//
//TEST(TestOpTensorCreate, CreateMultipleTensorMultipleOp)
//{
//
//    kp::Manager mgr;
//
//    std::vector<float> testVecA{ 9, 8, 7 };
//    std::vector<float> testVecB{ 6, 5, 4 };
//
//    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor(testVecA) };
//    std::shared_ptr<kp::Tensor> tensorB{ new kp::Tensor(testVecB) };
//
//    mgr.rebuild({ tensorA });
//    mgr.rebuild({ tensorB });
//
//    EXPECT_TRUE(tensorA->isInit());
//    EXPECT_TRUE(tensorB->isInit());
//
//    EXPECT_EQ(tensorA->data(), testVecA);
//    EXPECT_EQ(tensorB->data(), testVecB);
//}
//
//TEST(TestOpTensorCreate, TestTensorMemoryManagedByManagerDestroyed)
//{
//
//    std::vector<float> testVecA{ 9, 8, 7 };
//    std::vector<float> testVecB{ 6, 5, 4 };
//
//    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor(testVecA) };
//    std::shared_ptr<kp::Tensor> tensorB{ new kp::Tensor(testVecB) };
//
//    {
//        kp::Manager mgr;
//        mgr.rebuild({ tensorA });
//        mgr.rebuild({ tensorB });
//
//        EXPECT_TRUE(tensorA->isInit());
//        EXPECT_TRUE(tensorB->isInit());
//
//        EXPECT_EQ(tensorA->data(), testVecA);
//        EXPECT_EQ(tensorB->data(), testVecB);
//    }
//
//    EXPECT_FALSE(tensorA->isInit());
//    EXPECT_FALSE(tensorB->isInit());
//}
//
//TEST(TestOpTensorCreate, TestTensorMemoryManagedByManagerNOTDestroyed)
//{
//
//    std::vector<float> testVecA{ 9, 8, 7 };
//    std::vector<float> testVecB{ 6, 5, 4 };
//
//    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor(testVecA) };
//    std::shared_ptr<kp::Tensor> tensorB{ new kp::Tensor(testVecB) };
//
//    kp::Manager mgr;
//
//    {
//        mgr.rebuild({ tensorA });
//        mgr.rebuild({ tensorB });
//
//        EXPECT_TRUE(tensorA->isInit());
//        EXPECT_TRUE(tensorB->isInit());
//
//        EXPECT_EQ(tensorA->data(), testVecA);
//        EXPECT_EQ(tensorB->data(), testVecB);
//    }
//
//    EXPECT_TRUE(tensorA->isInit());
//    EXPECT_TRUE(tensorB->isInit());
//}
//
//TEST(TestOpTensorCreate, NoErrorIfTensorFreedBefore)
//{
//
//    std::vector<float> testVecA{ 9, 8, 7 };
//    std::vector<float> testVecB{ 6, 5, 4 };
//
//    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor(testVecA) };
//    std::shared_ptr<kp::Tensor> tensorB{ new kp::Tensor(testVecB) };
//
//    kp::Manager mgr;
//
//    mgr.rebuild({ tensorA });
//    mgr.rebuild({ tensorB });
//
//    EXPECT_TRUE(tensorA->isInit());
//    EXPECT_TRUE(tensorB->isInit());
//
//    EXPECT_EQ(tensorA->data(), testVecA);
//    EXPECT_EQ(tensorB->data(), testVecB);
//
//    tensorA->freeMemoryDestroyGPUResources();
//    tensorB->freeMemoryDestroyGPUResources();
//    EXPECT_FALSE(tensorA->isInit());
//    EXPECT_FALSE(tensorB->isInit());
//}
//
//TEST(TestOpTensorCreate, ExceptionOnZeroSizeTensor)
//{
//    std::vector<float> testVecA;
//
//    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor(testVecA) };
//
//    kp::Manager mgr;
//
//    try {
//        mgr.rebuild({ tensorA });
//    } catch (const std::runtime_error& err) {
//        // check exception
//        ASSERT_TRUE(std::string(err.what()).find("zero-sized") !=
//                    std::string::npos);
//    }
//}
