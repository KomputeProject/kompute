#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"

#include "fmt/ranges.h"

TEST(TestPushConstants, TestConstantsAlgoDispatchOverride)
{
    {
        std::string shader(R"(
          #version 450
          #extension GL_EXT_shader_atomic_float: enable
          layout(push_constant) uniform PushConstants {
            float x;
            float y;
            float z;
          } pcs;
          layout (local_size_x = 1) in;
          layout(set = 0, binding = 0) buffer a { float pa[]; };
          void main() {
              atomicAdd(pa[0], pcs.x);
              atomicAdd(pa[1], pcs.y);
              atomicAdd(pa[2], pcs.z);
          })");

        std::vector<uint32_t> spirv = kp::Shader::compile_source(shader);

        std::shared_ptr<kp::Sequence> sq = nullptr;

        {
            kp::Manager mgr(0, {}, { "VK_EXT_shader_atomic_float", "SPV_EXT_shader_atomic_float_add" });

            std::shared_ptr<kp::Tensor> tensor = mgr.tensor({ 0, 0, 0 });

            std::shared_ptr<kp::Algorithm> algo =
              mgr.algorithm({ tensor }, spirv, kp::Workgroup({ 1 }), {}, { 0.0, 0.0, 0.0 });

            sq = mgr.sequence()
                   ->record<kp::OpTensorSyncDevice>({ tensor })
                   ->record<kp::OpAlgoDispatch>(algo,
                                                kp::Constants{ 0.1, 0.2, 0.3 })
                   ->record<kp::OpAlgoDispatch>(algo,
                                                kp::Constants{ 0.3, 0.2, 0.1 })
                   ->record<kp::OpTensorSyncLocal>({ tensor })
                   ->eval();

            EXPECT_EQ(tensor->data(), kp::Constants({ 0.4, 0.4, 0.4 }));
        }
    }
}

TEST(TestPushConstants, TestConstantsAlgoDispatchNoOverride)
{
    {
        std::string shader(R"(
          #version 450
          #extension GL_EXT_shader_atomic_float: enable
          layout(push_constant) uniform PushConstants {
            float x;
            float y;
            float z;
          } pcs;
          layout (local_size_x = 1) in;
          layout(set = 0, binding = 0) buffer a { float pa[]; };
          void main() {
             atomicAdd(pa[0], pcs.x);
             atomicAdd(pa[1], pcs.y);
             atomicAdd(pa[2], pcs.z);
          })");

        std::vector<uint32_t> spirv = kp::Shader::compile_source(shader);

        std::shared_ptr<kp::Sequence> sq = nullptr;

        {
            kp::Manager mgr(0, {}, { "VK_EXT_shader_atomic_float", "SPV_EXT_shader_atomic_float_add" });

            std::shared_ptr<kp::Tensor> tensor = mgr.tensor({ 0, 0, 0 });

            std::shared_ptr<kp::Algorithm> algo =
              mgr.algorithm({ tensor }, spirv, kp::Workgroup({ 1 }), {}, { 0.1, 0.2, 0.3 });

            sq = mgr.sequence()
                   ->record<kp::OpTensorSyncDevice>({ tensor })
                   ->record<kp::OpAlgoDispatch>(algo)
                   ->record<kp::OpAlgoDispatch>(algo,
                                                kp::Constants{ 0.3, 0.2, 0.1 })
                   ->record<kp::OpTensorSyncLocal>({ tensor })
                   ->eval();

            EXPECT_EQ(tensor->data(), kp::Constants({ 0.4, 0.4, 0.4 }));
        }
    }
}

TEST(TestPushConstants, TestConstantsWrongSize)
{
    {
        std::string shader(R"(
          #version 450
          #extension GL_EXT_shader_atomic_float: enable
          layout(push_constant) uniform PushConstants {
            float x;
            float y;
            float z;
          } pcs;
          layout (local_size_x = 1) in;
          layout(set = 0, binding = 0) buffer a { float pa[]; };
          void main() {
             atomicAdd(pa[0], pcs.x);
             atomicAdd(pa[1], pcs.y);
             atomicAdd(pa[2], pcs.z);
          })");

        std::vector<uint32_t> spirv = kp::Shader::compile_source(shader);

        std::shared_ptr<kp::Sequence> sq = nullptr;

        {
            kp::Manager mgr(0, {}, { "VK_EXT_shader_atomic_float", "SPV_EXT_shader_atomic_float_add" });

            std::shared_ptr<kp::Tensor> tensor = mgr.tensor({ 0, 0, 0 });

            std::shared_ptr<kp::Algorithm> algo =
              mgr.algorithm({ tensor }, spirv, kp::Workgroup({ 1 }), {}, { 0.0 });

            sq = mgr.sequence()
                   ->record<kp::OpTensorSyncDevice>({ tensor });

            EXPECT_THROW(sq->record<kp::OpAlgoDispatch>(algo, kp::Constants{ 0.1, 0.2, 0.3 }), std::runtime_error);
        }
    }
}
