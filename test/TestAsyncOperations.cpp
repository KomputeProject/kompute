
#include "gtest/gtest.h"

#include <chrono>

#include "kompute/Kompute.hpp"

TEST(TestAsyncOperations, TestManagerAsync)
{
    uint32_t size = 100000;

    std::vector<float> data(size, 0.0);
    std::vector<float> resultSync(size, 100000);
    std::vector<float> resultAsync(size, 200000);

    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor(data) };
    std::shared_ptr<kp::Tensor> tensorB{ new kp::Tensor(data) };
    std::shared_ptr<kp::Tensor> tensorC{ new kp::Tensor(data) };
    std::shared_ptr<kp::Tensor> tensorD{ new kp::Tensor(data) };
    std::shared_ptr<kp::Tensor> tensorE{ new kp::Tensor(data) };
    std::shared_ptr<kp::Tensor> tensorF{ new kp::Tensor(data) };

    kp::Manager mgr;

    mgr.evalOpDefault<kp::OpTensorCreate>({ tensorA, tensorB, tensorC, tensorD, tensorE, tensorF });

    std::string shader(R"(
        #version 450

        layout (local_size_x = 1) in;

        layout(set = 0, binding = 0) buffer a { float pa[]; };
        layout(set = 0, binding = 1) buffer b { float pb[]; };

        void main() {
            uint index = gl_GlobalInvocationID.x;

            for (int i = 0; i < 100000; i++)
            {
                pa[index] += 1.0;
            }

            pb[index] = pa[index];
        }
    )");

    auto startSync = std::chrono::high_resolution_clock::now();

    mgr.evalOpDefault<kp::OpAlgoBase<>>(
      { tensorA, tensorB }, std::vector<char>(shader.begin(), shader.end()));

    mgr.evalOpDefault<kp::OpAlgoBase<>>(
      { tensorC, tensorD }, std::vector<char>(shader.begin(), shader.end()));

    mgr.evalOpDefault<kp::OpAlgoBase<>>(
      { tensorE, tensorF }, std::vector<char>(shader.begin(), shader.end()));

    auto endSync = std::chrono::high_resolution_clock::now();

    mgr.evalOpDefault<kp::OpTensorSyncLocal>({ tensorB, tensorD, tensorF });

    EXPECT_EQ(tensorB->data(), resultSync);
    EXPECT_EQ(tensorD->data(), resultSync);
    EXPECT_EQ(tensorF->data(), resultSync);

    auto durationSync = std::chrono::duration_cast<std::chrono::microseconds>(endSync - startSync).count();

    auto startAsync = std::chrono::high_resolution_clock::now();

    mgr.evalOpAsync<kp::OpAlgoBase<>>(
      { tensorA, tensorB }, "asyncOne", std::vector<char>(shader.begin(), shader.end()));

    mgr.evalOpAsync<kp::OpAlgoBase<>>(
      { tensorC, tensorD }, "asyncTwo", std::vector<char>(shader.begin(), shader.end()));

    mgr.evalOpAsync<kp::OpAlgoBase<>>(
      { tensorE, tensorF }, "asyncThree", std::vector<char>(shader.begin(), shader.end()));

    mgr.evalOpAwait("asyncOne");
    mgr.evalOpAwait("asyncTwo");
    mgr.evalOpAwait("asyncThree");

    auto endAsync = std::chrono::high_resolution_clock::now();

    auto durationAsync = std::chrono::duration_cast<std::chrono::microseconds>(endAsync - startAsync).count();

    mgr.evalOpDefault<kp::OpTensorSyncLocal>({ tensorB, tensorD, tensorF });

    EXPECT_EQ(tensorB->data(), resultAsync);
    EXPECT_EQ(tensorD->data(), resultAsync);
    EXPECT_EQ(tensorF->data(), resultAsync);

    SPDLOG_DEBUG("Total Sync: {}", durationSync);
    SPDLOG_DEBUG("Total Async: {}", durationAsync);
}
