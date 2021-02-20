
#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"

TEST(TestProcessingIterations, IterateThroughMultipleSumAndCopies)
{
    kp::Manager mgr;

    float TOTAL_ITER = 10;

    std::vector<float> testExpectedOutVec = { TOTAL_ITER,
                                              TOTAL_ITER,
                                              TOTAL_ITER };

    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor({ 0, 0, 0 }) };
    std::shared_ptr<kp::Tensor> tensorB{ new kp::Tensor({ 0, 0, 0 }) };

    std::string shader(R"(
        #version 450

        layout (local_size_x = 1) in;

        layout(set = 0, binding = 0) buffer a { float pa[]; };
        layout(set = 0, binding = 1) buffer b { float pb[]; };

        void main() {
            uint index = gl_GlobalInvocationID.x;
            pb[index] = pa[index] + 1;
        }
    )");

    mgr.rebuild({ tensorA, tensorB }, false);

    {
        std::shared_ptr<kp::Sequence> sq =
          mgr.sequence("default");

        sq->begin();

        sq->record<kp::OpTensorSyncDevice>({ tensorA, tensorB });

        sq->end();

        sq->eval();
    }

    {
        std::shared_ptr<kp::Sequence> sq =
          mgr.sequence("run");

        sq->begin();

        sq->record<kp::OpAlgoBase>(
          { tensorA, tensorB },
          kp::spirv_from_source(shader));

        sq->record<kp::OpTensorCopy>({ tensorB, tensorA });
        sq->end();

        for (size_t i = 0; i < TOTAL_ITER; i++) {
            sq->eval();
        }
    }

    {
        std::shared_ptr<kp::Sequence> sq =
          mgr.sequence("export");

        sq->begin();

        sq->record<kp::OpTensorSyncLocal>({ tensorA, tensorB });

        sq->end();

        sq->eval();
    }

    EXPECT_EQ(tensorA->data(), testExpectedOutVec);
}
