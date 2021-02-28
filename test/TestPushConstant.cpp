#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"

#include "fmt/ranges.h"

TEST(TestPushConstants, TestTwoConstants)
{
    {
        std::string shader(R"(
          #version 450
          layout(push_constant) uniform PushConstants {
            float x;
            float y;
            float z;
          } pcs;
          layout (local_size_x = 1) in;
          layout(set = 0, binding = 0) buffer a { float pa[]; };
          void main() {
              pa[0] += pcs.x;
              pa[1] += pcs.y;
              pa[2] += pcs.z;
          })");

        std::vector<uint32_t> spirv = kp::Shader::compile_source(shader);

        std::shared_ptr<kp::Sequence> sq = nullptr;

        {
            kp::Manager mgr;

            std::shared_ptr<kp::Tensor> tensor = mgr.tensor({ 0, 0, 0 });

            std::shared_ptr<kp::Algorithm> algo =
                mgr.algorithm({tensor}, spirv, kp::Workgroup({1}));

            sq = mgr.sequence()
                ->record<kp::OpTensorSyncDevice>({tensor})
                ->record<kp::OpAlgoDispatch>(algo, kp::Constants{0.1, 0.2, 0.3})
                ->record<kp::OpAlgoDispatch>(algo, kp::Constants{0.3, 0.2, 0.1})
                ->record<kp::OpTensorSyncLocal>({tensor})
                ->eval();

            EXPECT_EQ(tensor->data(), kp::Constants({0.4, 0.4, 0.4}));
        }
    }
}
