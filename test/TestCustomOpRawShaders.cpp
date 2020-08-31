
#include "catch2/catch.hpp"

#include "kompute/Kompute.hpp"

#include <fmt/ranges.h>

TEST_CASE("op_custom_simple_raw_shader") {
    kp::Manager mgr;

    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor({ 3, 4, 5 })};
    std::shared_ptr<kp::Tensor> tensorB{ new kp::Tensor({ 0, 0, 0 })};
    mgr.evalOpDefault<kp::OpCreateTensor>({ tensorA, tensorB });

    std::string shader(
        "#version 450\n"
        "layout (local_size_x = 1) in;\n"
        "layout(set = 0, binding = 0) buffer a { uint pa[]; };\n"
        "layout(set = 0, binding = 1) buffer b { uint pb[]; };\n"
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

    REQUIRE(tensorA->data() == std::vector<uint32_t>{0, 1, 2});
    REQUIRE(tensorB->data() == std::vector<uint32_t>{3, 4, 5});
}
