
#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"

TEST(TestManager, EndToEndOpMultFlow)
{
    kp::Manager mgr;

    std::shared_ptr<kp::Tensor> tensorLHS{ new kp::Tensor({ 0, 1, 2 }) };
    mgr.rebuild({ tensorLHS });

    std::shared_ptr<kp::Tensor> tensorRHS{ new kp::Tensor({ 2, 4, 6 }) };
    mgr.rebuild({ tensorRHS });

    std::shared_ptr<kp::Tensor> tensorOutput{ new kp::Tensor({ 0, 0, 0 }) };

    mgr.rebuild({ tensorOutput });

    mgr.evalOpDefault<kp::OpMult>({ tensorLHS, tensorRHS, tensorOutput });

    mgr.evalOpDefault<kp::OpTensorSyncLocal>({ tensorOutput });

    EXPECT_EQ(tensorOutput->data(), std::vector<float>({ 0, 4, 12 }));
}

TEST(TestManager, OpMultSequenceFlow)
{

    std::shared_ptr<kp::Tensor> tensorLHS{ new kp::Tensor({ 0, 1, 2 }) };

    std::shared_ptr<kp::Tensor> tensorRHS{ new kp::Tensor({ 2, 4, 6 }) };

    std::shared_ptr<kp::Tensor> tensorOutput{ new kp::Tensor({ 0, 0, 0 }) };

    kp::Manager mgr;

    {
        mgr.rebuild({ tensorLHS, tensorRHS, tensorOutput });

        std::shared_ptr<kp::Sequence> sq =
          mgr.sequence("newSequence");

        sq->begin();

        sq->record<kp::OpMult>({ tensorLHS, tensorRHS, tensorOutput });

        sq->record<kp::OpTensorSyncLocal>({ tensorOutput });

        sq->end();
        sq->eval();
    }

    EXPECT_EQ(tensorOutput->data(), std::vector<float>({ 0, 4, 12 }));
}

TEST(TestManager, TestMultipleSequences)
{
    kp::Manager mgr;

    std::shared_ptr<kp::Sequence> sqOne =
      mgr.sequence("sqOne");

    std::shared_ptr<kp::Sequence> sqTwo =
      mgr.sequence("sqTwo");

    std::shared_ptr<kp::Sequence> sqOneRef =
      mgr.sequence("sqOne");

    std::shared_ptr<kp::Sequence> sqTwoRef =
      mgr.sequence("sqTwo");

    EXPECT_EQ(sqOne, sqOneRef);
    EXPECT_NE(sqTwo, sqOneRef);
    EXPECT_EQ(sqTwo, sqTwoRef);
    EXPECT_NE(sqOneRef, sqTwoRef);
}

TEST(TestManager, TestMultipleTensorsAtOnce)
{

    std::shared_ptr<kp::Tensor> tensorLHS{ new kp::Tensor({ 0, 1, 2 }) };

    std::shared_ptr<kp::Tensor> tensorRHS{ new kp::Tensor({ 2, 4, 6 }) };

    std::shared_ptr<kp::Tensor> tensorOutput{ new kp::Tensor({ 0, 0, 0 }) };

    kp::Manager mgr;

    std::shared_ptr<kp::Sequence> sq =
      mgr.sequence("newSequence");

    {
        mgr.rebuild({ tensorLHS, tensorRHS, tensorOutput });

        EXPECT_TRUE(tensorLHS->isInit());
        EXPECT_TRUE(tensorRHS->isInit());
        EXPECT_TRUE(tensorOutput->isInit());

        sq->begin();

        sq->record<kp::OpMult>({ tensorLHS, tensorRHS, tensorOutput });

        sq->record<kp::OpTensorSyncLocal>({ tensorOutput });

        sq->end();
        sq->eval();
    }

    EXPECT_EQ(tensorOutput->data(), std::vector<float>({ 0, 4, 12 }));
}

TEST(TestManager, TestCreateInitTensor)
{
    kp::Manager mgr;

    std::shared_ptr<kp::Tensor> tensorA = mgr.tensor({ 0, 1, 2 });
    std::shared_ptr<kp::Tensor> tensorB = mgr.tensor({ 0, 0, 0 });

    mgr.evalOpDefault<kp::OpTensorCopy>({ tensorA, tensorB });

    mgr.evalOpDefault<kp::OpTensorSyncLocal>({ tensorB });

    EXPECT_EQ(tensorB->data(), std::vector<float>({ 0, 1, 2 }));

    std::shared_ptr<kp::Tensor> tensorC =
      mgr.tensor({ 0, 0, 0 }, kp::Tensor::TensorTypes::eHost);

    mgr.evalOpDefault<kp::OpTensorCopy>({ tensorA, tensorC });

    EXPECT_EQ(tensorC->data(), std::vector<float>({ 0, 1, 2 }));
}
