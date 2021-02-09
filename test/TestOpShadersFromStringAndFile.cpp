
#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"

#include "kompute_test/shaders/shadertest_op_custom_shader.hpp"

TEST(TestOpAlgoBase, ShaderRawDataFromConstructor)
{
    kp::Manager mgr;

    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor({ 3, 4, 5 }) };
    std::shared_ptr<kp::Tensor> tensorB{ new kp::Tensor({ 0, 0, 0 }) };
    mgr.rebuild({ tensorA, tensorB });

    std::string shader(R"(
        #version 450

        layout (local_size_x = 1) in;

        layout(set = 0, binding = 0) buffer a { float pa[]; };
        layout(set = 0, binding = 1) buffer b { float pb[]; };

        void main() {
            uint index = gl_GlobalInvocationID.x;
            pb[index] = pa[index];
            pa[index] = index;
        }
    )");

    mgr.evalOpDefault<kp::OpAlgoBase>(
      { tensorA, tensorB }, std::vector<char>(shader.begin(), shader.end()));

    mgr.evalOpDefault<kp::OpTensorSyncLocal>({ tensorA, tensorB });

    EXPECT_EQ(tensorA->data(), std::vector<float>({ 0, 1, 2 }));
    EXPECT_EQ(tensorB->data(), std::vector<float>({ 3, 4, 5 }));
}

TEST(TestOpAlgoBase, ShaderCompiledDataFromConstructor)
{
    kp::Manager mgr;

    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor({ 3, 4, 5 }) };
    std::shared_ptr<kp::Tensor> tensorB{ new kp::Tensor({ 0, 0, 0 }) };
    mgr.rebuild({ tensorA, tensorB });

    mgr.evalOpDefault<kp::OpAlgoBase>(
      { tensorA, tensorB },
      std::vector<char>(
        kp::shader_data::test_shaders_glsl_test_op_custom_shader_comp_spv,
        kp::shader_data::test_shaders_glsl_test_op_custom_shader_comp_spv +
          kp::shader_data::
            test_shaders_glsl_test_op_custom_shader_comp_spv_len));

    mgr.evalOpDefault<kp::OpTensorSyncLocal>({ tensorA, tensorB });

    EXPECT_EQ(tensorA->data(), std::vector<float>({ 0, 1, 2 }));
    EXPECT_EQ(tensorB->data(), std::vector<float>({ 3, 4, 5 }));
}

TEST(TestOpAlgoBase, ShaderRawDataFromFile)
{
    kp::Manager mgr;

    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor({ 3, 4, 5 }) };
    std::shared_ptr<kp::Tensor> tensorB{ new kp::Tensor({ 0, 0, 0 }) };
    mgr.rebuild({ tensorA, tensorB });

    mgr.evalOpDefault<kp::OpAlgoBase>(
      { tensorA, tensorB }, "test/shaders/glsl/test_op_custom_shader.comp");

    mgr.evalOpDefault<kp::OpTensorSyncLocal>({ tensorA, tensorB });

    EXPECT_EQ(tensorA->data(), std::vector<float>({ 0, 1, 2 }));
    EXPECT_EQ(tensorB->data(), std::vector<float>({ 3, 4, 5 }));
}

TEST(TestOpAlgoBase, ShaderCompiledDataFromFile)
{
    kp::Manager mgr;

    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor({ 3, 4, 5 }) };
    std::shared_ptr<kp::Tensor> tensorB{ new kp::Tensor({ 0, 0, 0 }) };
    mgr.rebuild({ tensorA, tensorB });

    mgr.evalOpDefault<kp::OpAlgoBase>(
      { tensorA, tensorB }, "test/shaders/glsl/test_op_custom_shader.comp.spv");

    mgr.evalOpDefault<kp::OpTensorSyncLocal>({ tensorA, tensorB });

    EXPECT_EQ(tensorA->data(), std::vector<float>({ 0, 1, 2 }));
    EXPECT_EQ(tensorB->data(), std::vector<float>({ 3, 4, 5 }));
}
