#if defined(_WIN32)
#pragma comment(linker, "/subsystem:console")
#endif

// SPDLOG_ACTIVE_LEVEL must be defined before spdlog.h import
#if DEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#endif

#include <vector>

#include <spdlog/spdlog.h>
// ranges.h must come after spdlog.h
#include <spdlog/fmt/bundled/ranges.h>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>

#include "Manager.hpp"
#include "OpCreateTensor.hpp"
#include "OpMult.hpp"
#include "Tensor.hpp"

int
main()
{
#if DEBUG
    spdlog::set_level(spdlog::level::debug);
#else
    spdlog::set_level(spdlog::level::info);
#endif

    try {
        // VulkanCompute* vulkanExample = new VulkanCompute();
        // spdlog::info("Finished.");
        // delete (vulkanExample);

        // Run Kompute

        {
            spdlog::info("Creating manager");
            kp::Manager mgr;

            spdlog::info("Creating first tensor");
            std::shared_ptr<kp::Tensor> tensorLHS{ new kp::Tensor( { 0.0, 1.0, 2.0 }) };
            mgr.evalOp<kp::OpCreateTensor>({ tensorLHS });

            spdlog::info("Creating second tensor");
            std::shared_ptr<kp::Tensor> tensorRHS{ new kp::Tensor(
              { 2.0, 4.0, 6.0 }) };
            mgr.evalOp<kp::OpCreateTensor>({ tensorRHS });

            // TODO: Add capabilities for just output tensor types
            spdlog::info("Creating output tensor");
            std::shared_ptr<kp::Tensor> tensorOutput{ new kp::Tensor(
              { 0.0, 0.0, 0.0 }) };
            mgr.evalOp<kp::OpCreateTensor>({ tensorOutput });

            spdlog::info("OpCreateTensor success for tensors");
            spdlog::info("Tensor one: {}", tensorLHS->data());
            spdlog::info("Tensor two: {}", tensorRHS->data());
            spdlog::info("Tensor output: {}", tensorOutput->data());

            spdlog::info("Calling op mult");
            mgr.evalOp<kp::OpMult<>>({ tensorLHS, tensorRHS, tensorOutput });

            spdlog::info("OpMult call success");
            spdlog::info("Tensor output: {}", tensorOutput->data());

            spdlog::info("Called manager eval success END PROGRAM");
        }
        {
            spdlog::info("Creating manager");
            kp::Manager mgr;
            kp::Sequence sq = mgr.constructSequence();
            sq.begin();

            spdlog::info("Creating first tensor");
            std::shared_ptr<kp::Tensor> tensorLHS{ new kp::Tensor(
              { 0.0, 1.0, 2.0 }) };

            spdlog::info("Creating second tensor");
            std::shared_ptr<kp::Tensor> tensorRHS{ new kp::Tensor(
              { 2.0, 4.0, 6.0 }) };

            // TODO: Add capabilities for just output tensor types
            spdlog::info("Creating output tensor");
            std::shared_ptr<kp::Tensor> tensorOutput{ new kp::Tensor(
              { 0.0, 0.0, 0.0 }) };

            sq.record<kp::OpCreateTensor>({ tensorLHS });
            sq.record<kp::OpCreateTensor>({ tensorRHS });
            sq.record<kp::OpCreateTensor>({ tensorOutput });

            spdlog::info("OpCreateTensor success for tensors");
            spdlog::info("Tensor one: {}", tensorLHS->data());
            spdlog::info("Tensor two: {}", tensorRHS->data());
            spdlog::info("Tensor output: {}", tensorOutput->data());

            spdlog::info("Calling op mult");
            sq.record<kp::OpMult<>>({ tensorLHS, tensorRHS, tensorOutput });

            sq.end();
            sq.eval();

            spdlog::info("OpMult call success");
            spdlog::info("Tensor output: {}", tensorOutput->data());

            spdlog::info("Called manager eval success END PROGRAM");
        }

        return 0;
    } catch (const std::exception& exc) {
        spdlog::error("Exception caught: {}", exc.what());
        return 1;
    } catch (...) {
        spdlog::error("Uncaught exception");
        return 1;
    }
}
