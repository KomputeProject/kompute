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

    std::vector<std::shared_ptr<kp::Memory>> params = { tensorInA, tensorInB, tensorOut };

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

    mgr.sequence()->eval<kp::OpSyncDevice>({ tensorInA });

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

    mgr.sequence()->eval<kp::OpSyncLocal>({ tensorOut });

    EXPECT_EQ(tensorOut->vector(), std::vector<float>(numElems, elemValue * numIter * numOps * numSeqs));

    // Validating significant divergences of performance
    // Currently configured for github actions performance
    EXPECT_LT(totalTime, 50000000);
}

TEST(TestBenchmark, ManagerTensorCreationCPUOverhead)
{
    // Benchmark Manager tensor creation CPU overhead - target area for shared_ptr removal
    const uint32_t numIterations = 1000;
    const uint32_t numTensorsPerIteration = 100;
    const uint32_t tensorSize = 1024;

    auto startTime = std::chrono::high_resolution_clock::now();

    for (uint32_t iter = 0; iter < numIterations; iter++) {
        kp::Manager mgr;

        std::vector<std::shared_ptr<kp::TensorT<float>>> tensors;
        tensors.reserve(numTensorsPerIteration);

        for (uint32_t i = 0; i < numTensorsPerIteration; i++) {
            // Measure CPU overhead of tensor creation, not GPU allocation
            tensors.push_back(mgr.tensorT<float>(tensorSize));
        }

        // Ensure tensors are created and tracked by Manager
        for (const auto& tensor : tensors) {
            EXPECT_TRUE(tensor->isInit());
        }
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto totalTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

    std::cout << "Manager Tensor Creation: " << totalTime << " μs for "
              << (numIterations * numTensorsPerIteration) << " tensors" << std::endl;

    // Performance baseline - should improve 2-10x after shared_ptr removal
    EXPECT_LT(totalTime, 10000000); // 10 seconds baseline
}

TEST(TestBenchmark, AlgorithmParameterBindingCPUOverhead)
{
    // Benchmark Algorithm parameter binding CPU overhead - target area for shared_ptr removal
    const uint32_t numIterations = 500;
    const uint32_t numTensorsPerAlgo = 20;
    const uint32_t tensorSize = 512;

    std::string shader(R"(
        #version 450
        layout(local_size_x = 1) in;
        layout(binding = 0) buffer a { float data[]; };
        void main() {
            uint i = gl_GlobalInvocationID.x;
            data[i] = data[i] + 1.0;
        }
    )");
    std::vector<uint32_t> spirv = compileSource(shader);

    auto startTime = std::chrono::high_resolution_clock::now();

    for (uint32_t iter = 0; iter < numIterations; iter++) {
        kp::Manager mgr;

        // Create tensors
        std::vector<std::shared_ptr<kp::Memory>> memObjects;
        memObjects.reserve(numTensorsPerAlgo);

        for (uint32_t i = 0; i < numTensorsPerAlgo; i++) {
            memObjects.push_back(mgr.tensorT<float>(tensorSize));
        }

        // Measure CPU overhead of algorithm creation with parameter binding
        auto algorithm = mgr.algorithm(memObjects, spirv);

        // Verify algorithm has bound all parameters
        EXPECT_EQ(algorithm->getMemObjects().size(), numTensorsPerAlgo);
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto totalTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

    std::cout << "Algorithm Parameter Binding: " << totalTime << " μs for "
              << numIterations << " algorithms with " << numTensorsPerAlgo << " tensors each" << std::endl;

    // Performance baseline - should improve 2-10x after shared_ptr removal
    EXPECT_LT(totalTime, 5000000); // 5 seconds baseline
}

TEST(TestBenchmark, SequenceOperationRecordingCPUOverhead)
{
    // Benchmark Sequence operation recording CPU overhead - target area for shared_ptr removal
    const uint32_t numIterations = 200;
    const uint32_t numOperationsPerSequence = 50;
    const uint32_t tensorSize = 256;

    std::string shader(R"(
        #version 450
        layout(local_size_x = 1) in;
        layout(binding = 0) buffer a { float data[]; };
        void main() {
            uint i = gl_GlobalInvocationID.x;
            data[i] = data[i] * 2.0;
        }
    )");
    std::vector<uint32_t> spirv = compileSource(shader);

    auto startTime = std::chrono::high_resolution_clock::now();

    for (uint32_t iter = 0; iter < numIterations; iter++) {
        kp::Manager mgr;

        auto tensor = mgr.tensorT<float>(tensorSize);
        auto algorithm = mgr.algorithm({tensor}, spirv);
        auto sequence = mgr.sequence();

        // Measure CPU overhead of operation recording (shared_ptr copying/referencing)
        for (uint32_t op = 0; op < numOperationsPerSequence; op++) {
            sequence->record<kp::OpAlgoDispatch>(algorithm);
        }

        EXPECT_TRUE(sequence->isInit());
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto totalTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

    std::cout << "Sequence Operation Recording: " << totalTime << " μs for "
              << (numIterations * numOperationsPerSequence) << " operations" << std::endl;

    // Performance baseline - should improve 2-10x after shared_ptr removal
    EXPECT_LT(totalTime, 3000000); // 3 seconds baseline
}

TEST(TestBenchmark, MemoryReferenceResolutionCPUOverhead)
{
    // Benchmark Memory reference resolution CPU overhead - target area for shared_ptr removal
    const uint32_t numIterations = 2000;
    const uint32_t numResolutionsPerIteration = 1000;
    const uint32_t tensorSize = 128;

    kp::Manager mgr;
    std::vector<std::shared_ptr<kp::TensorT<float>>> tensors;

    // Pre-create tensors
    for (uint32_t i = 0; i < 100; i++) {
        tensors.push_back(mgr.tensorT<float>(tensorSize));
    }

    auto startTime = std::chrono::high_resolution_clock::now();

    for (uint32_t iter = 0; iter < numIterations; iter++) {
        // Measure CPU overhead of shared_ptr dereferencing and method calls
        for (uint32_t res = 0; res < numResolutionsPerIteration; res++) {
            auto& tensor = tensors[res % tensors.size()];

            // Typical operations that involve shared_ptr overhead
            bool isInit = tensor->isInit();
            size_t size = tensor->size();
            auto memType = tensor->memoryType();

            // Prevent compiler optimization by using variables
            (void)isInit; (void)size; (void)memType;
        }
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto totalTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

    std::cout << "Memory Reference Resolution: " << totalTime << " μs for "
              << (numIterations * numResolutionsPerIteration) << " resolutions" << std::endl;

    // Performance baseline - should improve 2-10x after shared_ptr removal
    EXPECT_LT(totalTime, 2000000); // 2 seconds baseline
}

TEST(TestBenchmark, CrossComponentReferenceCPUOverhead)
{
    // Benchmark cross-component reference CPU overhead - target area for shared_ptr removal
    const uint32_t numIterations = 300;
    const uint32_t numCrossReferences = 200;

    std::string shader(R"(
        #version 450
        layout(local_size_x = 1) in;
        layout(binding = 0) buffer a { float data[]; };
        void main() {
            uint i = gl_GlobalInvocationID.x;
            data[i] = data[i] + 0.5;
        }
    )");
    std::vector<uint32_t> spirv = compileSource(shader);

    auto startTime = std::chrono::high_resolution_clock::now();

    for (uint32_t iter = 0; iter < numIterations; iter++) {
        kp::Manager mgr;

        // Create interconnected objects
        auto tensor = mgr.tensorT<float>(1024);
        auto algorithm = mgr.algorithm({tensor}, spirv);
        auto sequence = mgr.sequence();

        // Measure CPU overhead of cross-component shared_ptr operations
        for (uint32_t ref = 0; ref < numCrossReferences; ref++) {
            // Algorithm accessing its memory objects (shared_ptr vector copy)
            auto memObjects = algorithm->getMemObjects();

            // Sequence recording operation (shared_ptr assignment)
            sequence->record<kp::OpAlgoDispatch>(algorithm);

            // Clear and re-record (shared_ptr cleanup and recreation)
            sequence->clear();
        }
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto totalTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

    std::cout << "Cross-Component Reference: " << totalTime << " μs for "
              << (numIterations * numCrossReferences) << " cross-references" << std::endl;

    // Performance baseline - should improve 2-10x after shared_ptr removal
    EXPECT_LT(totalTime, 4000000); // 4 seconds baseline
}

