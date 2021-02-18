
#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"


TEST(TestSpecializationConstants, TestTwoConstants)
{
    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor({ 0, 0, 0 }) };
    std::shared_ptr<kp::Tensor> tensorB{ new kp::Tensor({ 0, 0, 0 }) };

    std::string shader(R"(
      #version 450
      layout (constant_id = 0) const float cOne = 1;
      layout (constant_id = 1) const float cTwo = 1;
      layout (local_size_x = 1) in;
      layout(set = 0, binding = 0) buffer a { float pa[]; };
      layout(set = 0, binding = 1) buffer b { float pb[]; };
      void main() {
          uint index = gl_GlobalInvocationID.x;
          pa[index] = cOne;
          pb[index] = cTwo;
      })");

    {
        std::shared_ptr<kp::Sequence> sq = nullptr;

        {
            kp::Manager mgr;

            mgr.rebuild({ tensorA, tensorB });

            sq = mgr.sequence();

            auto spec = kp::Constants({5.0, 0.3});

            sq->begin();
            sq->record<kp::OpAlgoBase>(
              { tensorA, tensorB },
              std::vector<char>(shader.begin(), shader.end()),
              kp::Workgroup(), spec);
            sq->end();

            sq->eval();

            mgr.evalOpDefault<kp::OpTensorSyncLocal>({ tensorA, tensorB });
        }
    }
    EXPECT_EQ(tensorA->data(), std::vector<float>({ 5, 5, 5 }));
    EXPECT_EQ(tensorB->data(), std::vector<float>({ 0.3, 0.3, 0.3 }));
}
