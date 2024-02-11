// SPDX-License-Identifier: Apache-2.0

#include "gtest/gtest.h"

#include <chrono>

#include "kompute/Kompute.hpp"
#include "kompute/logger/Logger.hpp"
#include "shaders/Utils.hpp"

// Opt: Compile with -DKOMPUTE_OPT_LOG_LEVEL=Info or above to optimize operations

TEST(TestBenchmark, TestMultipleSequenceOperationMostlyGPU)
{
    // num<> parameters below can be tweaked for benchmark
    uint32_t numIter = 10000;

    uint32_t numSeqs = 10;
    uint32_t numOps = 10;
    uint32_t numElems = 1024 * 100;

    uint32_t elemValue = 1;

    std::string shader(R"(
        #version 450

        layout(local_size_x = 1) in;

        layout(binding = 0) buffer restrict readonly  tensorInA { float inA[]; };
        layout(binding = 1) buffer restrict readonly  tensorInB { int   inB[]; };
        layout(binding = 2) buffer restrict writeonly tensorOut { float out_[]; };

        void main() {
            const uint baseIndex = gl_WorkGroupID.x * 4;

            for (uint x = 0; x < 4; x++) {
                const uint i = baseIndex + x;
                out_[i] += inA[i] + inB[i];
            }
        }
    )");

    std::vector<uint32_t> spirv = compileSource(shader);

    kp::Manager mgr;

    std::shared_ptr<kp::TensorT<float>> tensorInA = mgr.tensor(std::vector<float>(numElems, elemValue));
    std::shared_ptr<kp::TensorT<uint32_t>> tensorInB = mgr.tensorT<uint32_t>(std::vector<uint32_t>(numElems, elemValue));
    std::shared_ptr<kp::TensorT<float>> tensorOut = mgr.tensor(std::vector<float>(numElems, 0));

    std::vector<std::shared_ptr<kp::Tensor>> params = { tensorInA, tensorInB, tensorOut };

    // Opt: Avoiding using anonimous sequences when we will reuse
    std::vector<std::shared_ptr<kp::Sequence>> sequences(numSeqs);

    for (auto& sequence : sequences) {
        sequence = mgr.sequence();
        for (uint32_t i = 0; i < numOps; i++) {
            // Opt: Creating workgroup of length / 4 to introduce optimisations
            kp::Workgroup wgroup = { numElems / 4 };
            // Opt: Record the operations ahead of computation
            sequence->record<kp::OpAlgoDispatch>(mgr.algorithm(params, spirv, wgroup));
        }
    }

    mgr.sequence()->eval<kp::OpTensorSyncDevice>({ tensorInA });

    auto startTime = std::chrono::high_resolution_clock::now();

    for (uint32_t i = 0; i < numIter; i++) {
        for (auto& sequence : sequences) {
            // Opt: Run all queues async
            // (Wait only for the last one)
            sequence->evalAsync();
        }
        for (auto& sequence : sequences) {
            sequence->evalAwait();
        }
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto totalTime =
      std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime)
        .count();

    mgr.sequence()->eval<kp::OpTensorSyncLocal>({ tensorOut });

    EXPECT_EQ(tensorOut->vector(), std::vector<float>(numElems, elemValue * numIter * numOps * numSeqs));

    // Validating significant divergences of performance
    // Currently configured for github actions performance
    EXPECT_LT(totalTime, 50000000);
}

