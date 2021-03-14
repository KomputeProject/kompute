#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"

TEST(TestSpecializationConstants, TestTwoConstants)
{
    {
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

        std::vector<uint32_t> spirv = kp::Shader::compileSource(shader);

        std::shared_ptr<kp::Sequence> sq = nullptr;

        {
            kp::Manager mgr;

            std::shared_ptr<kp::TensorT<float>> tensorA =
              mgr.tensor({ 0, 0, 0 });
            std::shared_ptr<kp::TensorT<float>> tensorB =
              mgr.tensor({ 0, 0, 0 });

            std::vector<std::shared_ptr<kp::Tensor>> params = { tensorA,
                                                                tensorB };

            kp::Constants spec = kp::Constants({ 5.0, 0.3 });

            std::shared_ptr<kp::Algorithm> algo =
              mgr.algorithm(params, spirv, {}, spec);

            sq = mgr.sequence()
                   ->record<kp::OpTensorSyncDevice>(params)
                   ->record<kp::OpAlgoDispatch>(algo)
                   ->record<kp::OpTensorSyncLocal>(params)
                   ->eval();

            EXPECT_EQ(tensorA->vector(), std::vector<float>({ 5, 5, 5 }));
            EXPECT_EQ(tensorB->vector(), std::vector<float>({ 0.3, 0.3, 0.3 }));
        }
    }
}
