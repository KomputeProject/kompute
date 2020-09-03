
#include "catch2/catch.hpp"

#include "kompute/Kompute.hpp"

TEST_CASE("End to end OpMult Flow should execute correctly from manager") 
{
    kp::Manager mgr;

    std::shared_ptr<kp::Tensor> tensorLHS{ new kp::Tensor({ 0, 1, 2 }) };
    mgr.evalOp<kp::OpCreateTensor>({ tensorLHS });

    std::shared_ptr<kp::Tensor> tensorRHS{ new kp::Tensor(
      { 2, 4, 6 }) };
    mgr.evalOp<kp::OpCreateTensor>({ tensorRHS });

    std::shared_ptr<kp::Tensor> tensorOutput{ new kp::Tensor(
      { 0, 0, 0 }) };
    mgr.evalOp<kp::OpCreateTensor>({ tensorOutput });

    mgr.evalOp<kp::OpMult<>>({ tensorLHS, tensorRHS, tensorOutput });

    REQUIRE(tensorOutput->data() == std::vector<float>{0, 4, 12});
}

TEST_CASE("End to end OpMult Flow should execute correctly from sequence") {

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

        sq->record<kp::OpCreateTensor>({ tensorLHS });
        sq->record<kp::OpCreateTensor>({ tensorRHS });
        sq->record<kp::OpCreateTensor>({ tensorOutput });

        sq->record<kp::OpMult<>>({ tensorLHS, tensorRHS, tensorOutput });

        sq->end();
        sq->eval();
    }
    sqWeakPtr.reset();

    REQUIRE(tensorOutput->data() == std::vector<float>{0, 4, 12});
}

TEST_CASE("Test manager get create functionality for sequences") {
    kp::Manager mgr;

    std::weak_ptr<kp::Sequence> sqWeakPtrOne = 
        mgr.getOrCreateManagedSequence("sqOne");

    std::weak_ptr<kp::Sequence> sqWeakPtrTwo = 
        mgr.getOrCreateManagedSequence("sqTwo");

    std::weak_ptr<kp::Sequence> sqWeakPtrOneRef = 
        mgr.getOrCreateManagedSequence("sqOne");

    std::weak_ptr<kp::Sequence> sqWeakPtrTwoRef = 
        mgr.getOrCreateManagedSequence("sqTwo");

    REQUIRE(sqWeakPtrOne.lock() == sqWeakPtrOneRef.lock());
    REQUIRE(sqWeakPtrTwo.lock() != sqWeakPtrOneRef.lock());
    REQUIRE(sqWeakPtrTwo.lock() == sqWeakPtrTwoRef.lock());
    REQUIRE(sqWeakPtrOneRef.lock() != sqWeakPtrTwoRef.lock());
}

TEST_CASE("End to end OpMult Flow with OpCreateTensor called with multiple tensors") {

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

        sq->record<kp::OpCreateTensor>({ tensorLHS, tensorRHS, tensorOutput });

        REQUIRE(tensorLHS->isInit());
        REQUIRE(tensorRHS->isInit());
        REQUIRE(tensorOutput->isInit());

        sq->record<kp::OpMult<>>({ tensorLHS, tensorRHS, tensorOutput });

        sq->end();
        sq->eval();
    }
    sqWeakPtr.reset();

    REQUIRE(tensorOutput->data() == std::vector<float>{0, 4, 12});
}
