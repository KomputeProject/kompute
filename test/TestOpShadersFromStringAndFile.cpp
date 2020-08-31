
#include "catch2/catch.hpp"

#include "kompute/Kompute.hpp"

#include <fmt/ranges.h>

#include "kompute_test/shaders/shadertest_op_custom_shader.hpp"

TEST_CASE("test_op_shader_raw_data_from_constructor") {
    kp::Manager mgr;

    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor({ 3, 4, 5 })};
    std::shared_ptr<kp::Tensor> tensorB{ new kp::Tensor({ 0, 0, 0 })};
    mgr.evalOpDefault<kp::OpCreateTensor>({ tensorA, tensorB });

    std::string shader(
        "#version 450\n"
        "layout (local_size_x = 1) in;\n"
        "layout(set = 0, binding = 0) buffer a { float pa[]; };\n"
        "layout(set = 0, binding = 1) buffer b { float pb[]; };\n"
        "void main() {\n"
        "    uint index = gl_GlobalInvocationID.x;\n"
        "    pb[index] = pa[index];\n"
        "    pa[index] = index;\n"
        "}\n"
    );

    mgr.evalOpDefault<kp::OpAlgoBase<>>(
            { tensorA, tensorB }, 
            true, // Whether to copy output from device
            std::vector<char>(shader.begin(), shader.end()));

    REQUIRE(tensorA->data() == std::vector<float>{0, 1, 2});
    REQUIRE(tensorB->data() == std::vector<float>{3, 4, 5});
}

TEST_CASE("test_op_shader_compiled_data_from_constructor") {
    kp::Manager mgr;

    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor({ 3, 4, 5 })};
    std::shared_ptr<kp::Tensor> tensorB{ new kp::Tensor({ 0, 0, 0 })};
    mgr.evalOpDefault<kp::OpCreateTensor>({ tensorA, tensorB });

    mgr.evalOpDefault<kp::OpAlgoBase<>>(
            { tensorA, tensorB }, 
            true, // Whether to copy output from device
            std::vector<char>(
                kp::shader_data::test_shaders_glsl_test_op_custom_shader_comp_spv,
                kp::shader_data::test_shaders_glsl_test_op_custom_shader_comp_spv +
                kp::shader_data::test_shaders_glsl_test_op_custom_shader_comp_spv_len));

    REQUIRE(tensorA->data() == std::vector<float>{0, 1, 2});
    REQUIRE(tensorB->data() == std::vector<float>{3, 4, 5});
}

TEST_CASE("test_op_shader_raw_from_file") {
    kp::Manager mgr;

    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor({ 3, 4, 5 })};
    std::shared_ptr<kp::Tensor> tensorB{ new kp::Tensor({ 0, 0, 0 })};
    mgr.evalOpDefault<kp::OpCreateTensor>({ tensorA, tensorB });

    mgr.evalOpDefault<kp::OpAlgoBase<>>(
            { tensorA, tensorB }, 
            true, // Whether to copy output from device
            "test/shaders/glsl/test_op_custom_shader.comp");

    REQUIRE(tensorA->data() == std::vector<float>{0, 1, 2});
    REQUIRE(tensorB->data() == std::vector<float>{3, 4, 5});
}

TEST_CASE("test_op_shader_compiled_from_file") {
    kp::Manager mgr;

    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor({ 3, 4, 5 })};
    std::shared_ptr<kp::Tensor> tensorB{ new kp::Tensor({ 0, 0, 0 })};
    mgr.evalOpDefault<kp::OpCreateTensor>({ tensorA, tensorB });

    mgr.evalOpDefault<kp::OpAlgoBase<>>(
            { tensorA, tensorB }, 
            true, // Whether to copy output from device
            "test/shaders/glsl/test_op_custom_shader.comp.spv");

    REQUIRE(tensorA->data() == std::vector<float>{0, 1, 2});
    REQUIRE(tensorB->data() == std::vector<float>{3, 4, 5});
}
