
#include "gtest/gtest.h"

#include <chrono>

#include "kompute/Kompute.hpp"

TEST(TestAsyncOperations, TestManagerAsync)
{
    uint32_t size = 100000;

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

    std::vector<float> data(size, 0.0);
    std::vector<float> resultSync(size, 100000);
    std::vector<float> resultAsync(size, 100000);

    std::shared_ptr<kp::Tensor> tensorSyncA{ new kp::Tensor(data) };
    std::shared_ptr<kp::Tensor> tensorSyncB{ new kp::Tensor(data) };
    std::shared_ptr<kp::Tensor> tensorSyncC{ new kp::Tensor(data) };
    std::shared_ptr<kp::Tensor> tensorSyncD{ new kp::Tensor(data) };
    std::shared_ptr<kp::Tensor> tensorSyncE{ new kp::Tensor(data) };
    std::shared_ptr<kp::Tensor> tensorSyncF{ new kp::Tensor(data) };

    kp::Manager mgr;

    mgr.evalOpDefault<kp::OpTensorCreate>({ tensorSyncA, tensorSyncB, tensorSyncC, tensorSyncD, tensorSyncE, tensorSyncF });

    auto startSync = std::chrono::high_resolution_clock::now();

    mgr.evalOpDefault<kp::OpAlgoBase<>>(
      { tensorSyncA, tensorSyncB }, std::vector<char>(shader.begin(), shader.end()));

    mgr.evalOpDefault<kp::OpAlgoBase<>>(
      { tensorSyncC, tensorSyncD }, std::vector<char>(shader.begin(), shader.end()));

    mgr.evalOpDefault<kp::OpAlgoBase<>>(
      { tensorSyncE, tensorSyncF }, std::vector<char>(shader.begin(), shader.end()));

    mgr.evalOpDefault<kp::OpTensorSyncLocal>({ tensorSyncB, tensorSyncD, tensorSyncF });

    auto endSync = std::chrono::high_resolution_clock::now();
    auto durationSync = std::chrono::duration_cast<std::chrono::microseconds>(endSync - startSync).count();

    EXPECT_EQ(tensorSyncB->data(), resultSync);
    EXPECT_EQ(tensorSyncD->data(), resultSync);
    EXPECT_EQ(tensorSyncF->data(), resultSync);

    //std::shared_ptr<kp::Tensor> tensorAsyncA{ new kp::Tensor(data) };
    //std::shared_ptr<kp::Tensor> tensorAsyncB{ new kp::Tensor(data) };
    //std::shared_ptr<kp::Tensor> tensorAsyncC{ new kp::Tensor(data) };
    //std::shared_ptr<kp::Tensor> tensorAsyncD{ new kp::Tensor(data) };
    //std::shared_ptr<kp::Tensor> tensorAsyncE{ new kp::Tensor(data) };
    //std::shared_ptr<kp::Tensor> tensorAsyncF{ new kp::Tensor(data) };

    //kp::Manager mgrAsync(0, 1);

    //mgrAsync.evalOpDefault<kp::OpTensorCreate>({ tensorAsyncA, tensorAsyncB, tensorAsyncC, tensorAsyncD, tensorAsyncE, tensorAsyncF });

    //mgrAsync.createManagedSequence("async0", 0);
    ////mgrAsync.createManagedSequence("async1", 1);
    ////mgrAsync.createManagedSequence("async2", 2);

    //auto startAsync = std::chrono::high_resolution_clock::now();

    //mgrAsync.evalOpAsync<kp::OpAlgoBase<>>(
    //  { tensorAsyncA, tensorAsyncB }, "async0", std::vector<char>(shader.begin(), shader.end()));

    ////mgrAsync.evalOpAsync<kp::OpAlgoBase<>>(
    ////  { tensorAsyncC, tensorAsyncD }, "async1", std::vector<char>(shader.begin(), shader.end()));

    ////mgrAsync.evalOpAsync<kp::OpAlgoBase<>>(
    ////  { tensorAsyncE, tensorAsyncF }, "async2", std::vector<char>(shader.begin(), shader.end()));

    //mgrAsync.evalOpAwait("async0");
    ////mgrAsync.evalOpAwait("async1");
    ////mgrAsync.evalOpAwait("async2");

    //mgrAsync.evalOpDefault<kp::OpTensorSyncLocal>({ tensorAsyncB });
    ////mgrAsync.evalOpDefault<kp::OpTensorSyncLocal>({ tensorAsyncD });
    ////mgrAsync.evalOpDefault<kp::OpTensorSyncLocal>({ tensorAsyncF });

    //auto endAsync = std::chrono::high_resolution_clock::now();
    //auto durationAsync = std::chrono::duration_cast<std::chrono::microseconds>(endAsync - startAsync).count();

    //EXPECT_EQ(tensorAsyncB->data(), resultAsync);
    ////EXPECT_EQ(tensorAsyncD->data(), resultAsync);
    ////EXPECT_EQ(tensorAsyncF->data(), resultAsync);

    ////SPDLOG_DEBUG("Total Sync: {}", durationSync);
    //SPDLOG_DEBUG("Total Async: {}", durationAsync);
}
