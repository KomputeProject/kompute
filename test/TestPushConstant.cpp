#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"

#include "fmt/ranges.h"

TEST(TestPushConstants, TestConstantsAlgoDispatchOverride)
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

        std::vector<uint32_t> spirv = kp::Shader::compileSource(shader);

        std::shared_ptr<kp::Sequence> sq = nullptr;

        {
            kp::Manager mgr;

            std::shared_ptr<kp::TensorT<float>> tensor =
              mgr.tensor({ 0, 0, 0 });

            std::shared_ptr<kp::Algorithm> algo = mgr.algorithm(
              { tensor }, spirv, kp::Workgroup({ 1 }), {}, { 0.0, 0.0, 0.0 });

            sq = mgr.sequence()->eval<kp::OpTensorSyncDevice>({ tensor });

            // We need to run this in sequence to avoid race condition
            // We can't use atomicAdd as swiftshader doesn't support it for
            // float
            sq->eval<kp::OpAlgoDispatch>(algo, kp::Constants{ 0.1, 0.2, 0.3 });
            sq->eval<kp::OpAlgoDispatch>(algo, kp::Constants{ 0.3, 0.2, 0.1 });
            sq->eval<kp::OpTensorSyncLocal>({ tensor });

            EXPECT_EQ(tensor->vector(), kp::Constants({ 0.4, 0.4, 0.4 }));
        }
    }
}

TEST(TestPushConstants, TestConstantsAlgoDispatchNoOverride)
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

        std::vector<uint32_t> spirv = kp::Shader::compileSource(shader);

        std::shared_ptr<kp::Sequence> sq = nullptr;

        {
            kp::Manager mgr;

            std::shared_ptr<kp::TensorT<float>> tensor =
              mgr.tensor({ 0, 0, 0 });

            std::shared_ptr<kp::Algorithm> algo = mgr.algorithm(
              { tensor }, spirv, kp::Workgroup({ 1 }), {}, { 0.1, 0.2, 0.3 });

            sq = mgr.sequence()->eval<kp::OpTensorSyncDevice>({ tensor });

            // We need to run this in sequence to avoid race condition
            // We can't use atomicAdd as swiftshader doesn't support it for
            // float
            sq->eval<kp::OpAlgoDispatch>(algo);
            sq->eval<kp::OpAlgoDispatch>(algo, kp::Constants{ 0.3, 0.2, 0.1 });
            sq->eval<kp::OpTensorSyncLocal>({ tensor });

            EXPECT_EQ(tensor->vector(), kp::Constants({ 0.4, 0.4, 0.4 }));
        }
    }
}

TEST(TestPushConstants, TestConstantsWrongSize)
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

        std::vector<uint32_t> spirv = kp::Shader::compileSource(shader);

        std::shared_ptr<kp::Sequence> sq = nullptr;

        {
            kp::Manager mgr;

            std::shared_ptr<kp::TensorT<float>> tensor =
              mgr.tensor({ 0, 0, 0 });

            std::shared_ptr<kp::Algorithm> algo = mgr.algorithm(
              { tensor }, spirv, kp::Workgroup({ 1 }), {}, { 0.0 });

            sq = mgr.sequence()->record<kp::OpTensorSyncDevice>({ tensor });

            EXPECT_THROW(sq->record<kp::OpAlgoDispatch>(
                           algo, kp::Constants{ 0.1, 0.2, 0.3 }),
                         std::runtime_error);
        }
    }
}
