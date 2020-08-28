#if defined(_WIN32)
#pragma comment(linker, "/subsystem:console")
#endif

// clang-format: SPDLOG_ACTIVE_LEVEL must be defined before spdlog.h import
#if DEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#endif

#include <vector>

#include <spdlog/spdlog.h>
// clang-format: ranges.h must come after spdlog.h
#include <fmt/ranges.h>

#include "catch2/catch.hpp"

#include "kompute/Kompute.hpp"

TEST_CASE("End to end OpMult Flow should execute correctly from manager") {
    spdlog::info("TEST CASE STARTING");

    spdlog::info("Creating manager");
    kp::Manager mgr;

    spdlog::info("Creating first tensor");
    std::shared_ptr<kp::Tensor> tensorLHS{ new kp::Tensor( { 0, 1, 2 }) };
    mgr.evalOp<kp::OpCreateTensor>({ tensorLHS });

    spdlog::info("Creating second tensor");
    std::shared_ptr<kp::Tensor> tensorRHS{ new kp::Tensor(
      { 2, 4, 6 }) };
    mgr.evalOp<kp::OpCreateTensor>({ tensorRHS });

    // TODO: Add capabilities for just output tensor types
    spdlog::info("Creating output tensor");
    std::shared_ptr<kp::Tensor> tensorOutput{ new kp::Tensor(
      { 0, 0, 0 }) };
    mgr.evalOp<kp::OpCreateTensor>({ tensorOutput });

    spdlog::info("OpCreateTensor success for tensors");
    spdlog::info("Tensor one: {}", tensorLHS->data());
    spdlog::info("Tensor two: {}", tensorRHS->data());
    spdlog::info("Tensor output: {}", tensorOutput->data());

    spdlog::info("Calling op mult");
    mgr.evalOp<kp::OpMult<>>({ tensorLHS, tensorRHS, tensorOutput });

    spdlog::info("OpMult call success");
    spdlog::info("Tensor output: {}", tensorOutput->data());

    REQUIRE(tensorOutput->data() == std::vector<uint32_t>{0, 4, 12});

    spdlog::info("Called manager eval success END PROGRAM");
}

TEST_CASE("End to end OpMult Flow should execute correctly from sequence") {
    spdlog::info("TEST CASE STARTING");

    spdlog::info("Creating manager");

    spdlog::info("Creating first tensor");
    std::shared_ptr<kp::Tensor> tensorLHS{ new kp::Tensor(
      { 0, 1, 2 }) };

    spdlog::info("Creating second tensor");
    std::shared_ptr<kp::Tensor> tensorRHS{ new kp::Tensor(
      { 2, 4, 6 }) };

    // TODO: Add capabilities for just output tensor types
    spdlog::info("Creating output tensor");
    std::shared_ptr<kp::Tensor> tensorOutput{ new kp::Tensor(
      { 0, 0, 0 }) };

    kp::Manager mgr;

    std::weak_ptr<kp::Sequence> sqWeakPtr = mgr.getOrCreateManagedSequence("newSequence");
    if (std::shared_ptr<kp::Sequence> sq = sqWeakPtr.lock()) {
        sq->begin();

        sq->record<kp::OpCreateTensor>({ tensorLHS });
        sq->record<kp::OpCreateTensor>({ tensorRHS });
        sq->record<kp::OpCreateTensor>({ tensorOutput });

        spdlog::info("OpCreateTensor success for tensors");
        spdlog::info("Tensor one: {}", tensorLHS->data());
        spdlog::info("Tensor two: {}", tensorRHS->data());
        spdlog::info("Tensor output: {}", tensorOutput->data());

        spdlog::info("Calling op mult");
        sq->record<kp::OpMult<>>({ tensorLHS, tensorRHS, tensorOutput });

        sq->end();
        sq->eval();
    }
    sqWeakPtr.reset();

    spdlog::info("OpMult call success");
    spdlog::info("Tensor output: {}", tensorOutput->data());

    REQUIRE(tensorOutput->data() == std::vector<uint32_t>{0, 4, 12});

    spdlog::info("Called manager eval success END PROGRAM");
}
