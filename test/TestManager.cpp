
#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"

TEST(TestManager, EndToEndOpMultFlow)
{
    kp::Manager mgr;

    std::shared_ptr<kp::Tensor> tensorLHS{ new kp::Tensor({ 0, 1, 2 }) };
    mgr.evalOp<kp::OpTensorCreate>({ tensorLHS });

    std::shared_ptr<kp::Tensor> tensorRHS{ new kp::Tensor( { 2, 4, 6 }) };
    mgr.evalOp<kp::OpTensorCreate>({ tensorRHS });

    std::shared_ptr<kp::Tensor> tensorOutput{ new kp::Tensor( { 0, 0, 0 }) };

    mgr.evalOp<kp::OpTensorCreate>({ tensorOutput });

    mgr.evalOp<kp::OpMult<>>({ tensorLHS, tensorRHS, tensorOutput });

    mgr.evalOp<kp::OpTensorSyncLocal>({ tensorOutput });

    EXPECT_EQ(tensorOutput->data(), std::vector<float>({0, 4, 12}));
}

TEST(TestManager, OpMultSequenceFlow) {

    std::shared_ptr<kp::Tensor> tensorLHS{ new kp::Tensor(
      { 0, 1, 2 }) };

    std::shared_ptr<kp::Tensor> tensorRHS{ new kp::Tensor(
      { 2, 4, 6 }) };

    std::shared_ptr<kp::Tensor> tensorOutput{ new kp::Tensor(
      { 0, 0, 0 }) };

    kp::Manager mgr;

    std::weak_ptr<kp::Sequence> sqWeakPtr = mgr.getOrCreateManagedSequence("newSequence");
    if (std::shared_ptr<kp::Sequence> sq = sqWeakPtr.lock()) {
        sq->begin();

        sq->record<kp::OpTensorCreate>({ tensorLHS });
        sq->record<kp::OpTensorCreate>({ tensorRHS });
        sq->record<kp::OpTensorCreate>({ tensorOutput });

        sq->record<kp::OpMult<>>({ tensorLHS, tensorRHS, tensorOutput });

        sq->record<kp::OpTensorSyncLocal>({ tensorOutput });

        sq->end();
        sq->eval();
    }
    sqWeakPtr.reset();

    EXPECT_EQ(tensorOutput->data(), std::vector<float>({0, 4, 12}));
}

TEST(TestManager, TestMultipleSequences) {
    kp::Manager mgr;

    std::weak_ptr<kp::Sequence> sqWeakPtrOne = 
        mgr.getOrCreateManagedSequence("sqOne");

    std::weak_ptr<kp::Sequence> sqWeakPtrTwo = 
        mgr.getOrCreateManagedSequence("sqTwo");

    std::weak_ptr<kp::Sequence> sqWeakPtrOneRef = 
        mgr.getOrCreateManagedSequence("sqOne");

    std::weak_ptr<kp::Sequence> sqWeakPtrTwoRef = 
        mgr.getOrCreateManagedSequence("sqTwo");

    EXPECT_EQ(sqWeakPtrOne.lock(), sqWeakPtrOneRef.lock());
    EXPECT_NE(sqWeakPtrTwo.lock(), sqWeakPtrOneRef.lock());
    EXPECT_EQ(sqWeakPtrTwo.lock(), sqWeakPtrTwoRef.lock());
    EXPECT_NE(sqWeakPtrOneRef.lock(), sqWeakPtrTwoRef.lock());
}

TEST(TestManager, TestMultipleTensorsAtOnce) {

    std::shared_ptr<kp::Tensor> tensorLHS{ new kp::Tensor(
      { 0, 1, 2 }) };

    std::shared_ptr<kp::Tensor> tensorRHS{ new kp::Tensor(
      { 2, 4, 6 }) };

    std::shared_ptr<kp::Tensor> tensorOutput{ new kp::Tensor(
      { 0, 0, 0 }) };

    kp::Manager mgr;

    std::weak_ptr<kp::Sequence> sqWeakPtr = mgr.getOrCreateManagedSequence("newSequence");
    if (std::shared_ptr<kp::Sequence> sq = sqWeakPtr.lock()) {
        sq->begin();

        sq->record<kp::OpTensorCreate>({ tensorLHS, tensorRHS, tensorOutput });

        EXPECT_TRUE(tensorLHS->isInit());
        EXPECT_TRUE(tensorRHS->isInit());
        EXPECT_TRUE(tensorOutput->isInit());

        sq->record<kp::OpMult<>>({ tensorLHS, tensorRHS, tensorOutput });

        sq->record<kp::OpTensorSyncLocal>({ tensorOutput });

        sq->end();
        sq->eval();
    }
    sqWeakPtr.reset();

    EXPECT_EQ(tensorOutput->data(), std::vector<float>({0, 4, 12}));
}

TEST(TestManager, TestCreateInitTensor) {
    kp::Manager mgr;

    std::shared_ptr<kp::Tensor> tensorA = mgr.buildTensor({0,1,2});
    std::shared_ptr<kp::Tensor> tensorB = mgr.buildTensor({0,0,0});

    mgr.evalOpDefault<kp::OpTensorCopy>({tensorA, tensorB});

    mgr.evalOpDefault<kp::OpTensorSyncLocal>({tensorB});

    EXPECT_EQ(tensorB->data(), std::vector<float>({0,1,2}));

    std::shared_ptr<kp::Tensor> tensorC = mgr.buildTensor({0,0,0}, kp::Tensor::TensorTypes::eStaging);

    mgr.evalOpDefault<kp::OpTensorCopy>({tensorA, tensorC});

    EXPECT_EQ(tensorC->data(), std::vector<float>({0,1,2}));
}

