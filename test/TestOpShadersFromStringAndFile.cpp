// SPDX-License-Identifier: Apache-2.0

#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"
#include "kompute/logger/Logger.hpp"

#include "shaders/Utils.hpp"
#include "test_op_custom_shader.hpp"
#include "test_shader.hpp"

// Introducing custom struct that can be used for tensors
struct TestStruct {
    float x;
    uint32_t y;
    int32_t z;

    // Creating an == operator overload for the comparison below
    bool operator==(const TestStruct rhs) const {
        return this->x == rhs.x && this->y == rhs.y && this->z == rhs.z;
    }
};
// Custom struct needs to be mapped the eCustom datatype
template<>
kp::Tensor::TensorDataTypes
kp::TensorT<TestStruct>::dataType()
{
    return Tensor::TensorDataTypes::eCustom;
}

TEST(TestShader, ShaderRawDataFromConstructorCustomDataType)
{
    std::string shader(R"(
        #version 450

        layout (local_size_x = 1) in;

        layout(std140, binding = 0) buffer a {
            float ax;
            uint ay;
            int az;
          };
        layout(std140, binding = 1) buffer b {
            float bx;
            uint by;
            int bz;
          };

        void main() {
            bx = ax;
            by = ay;
            bz = az;
        }
    )");

    kp::Manager mgr;

    std::shared_ptr<kp::TensorT<TestStruct>> tensorA = mgr.tensorT<TestStruct>({ { 0.1, 2, 3} });
    std::shared_ptr<kp::TensorT<TestStruct>> tensorB = mgr.tensorT<TestStruct>({ { 0.0, 0, 0} });

    std::vector<uint32_t> spirv = compileSource(shader);

    std::vector<std::shared_ptr<kp::Tensor>> params = { tensorA, tensorB };

    mgr.sequence()
      ->eval<kp::OpTensorSyncDevice>(params)
      ->eval<kp::OpAlgoDispatch>(mgr.algorithm(params, spirv))
      ->eval<kp::OpTensorSyncLocal>(params);

    EXPECT_EQ(tensorA->vector(), std::vector<TestStruct>({ TestStruct{0.1, 2, 3} }));
    EXPECT_EQ(tensorB->vector(), std::vector<TestStruct>({ TestStruct{0.1, 2, 3} }));
}

TEST(TestShaderEndianness, ShaderRawDataFromConstructor)
{
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

    std::vector<uint32_t> spirv = compileSource(shader);
    std::vector<uint32_t> spirv2(kp::TEST_SHADER_COMP_SPV.begin(),
                                 kp::TEST_SHADER_COMP_SPV.end());
    EXPECT_EQ(spirv.size(), spirv2.size());
    for (size_t i = 0; i < spirv.size(); i++) {
        EXPECT_EQ(spirv[i], spirv2[i]);
    }
}

TEST(TestOpAlgoCreate, ShaderRawDataFromConstructor)
{
    kp::Manager mgr;

    std::shared_ptr<kp::TensorT<float>> tensorA = mgr.tensor({ 3, 4, 5 });
    std::shared_ptr<kp::TensorT<float>> tensorB = mgr.tensor({ 0, 0, 0 });

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

    std::vector<uint32_t> spirv = compileSource(shader);

    std::vector<std::shared_ptr<kp::Tensor>> params = { tensorA, tensorB };

    mgr.sequence()
      ->eval<kp::OpTensorSyncDevice>(params)
      ->eval<kp::OpAlgoDispatch>(mgr.algorithm(params, spirv))
      ->eval<kp::OpTensorSyncLocal>(params);

    EXPECT_EQ(tensorA->vector(), std::vector<float>({ 0, 1, 2 }));
    EXPECT_EQ(tensorB->vector(), std::vector<float>({ 3, 4, 5 }));
}

TEST(TestOpAlgoCreate, ShaderCompiledDataFromConstructor)
{
    kp::Manager mgr;

    std::shared_ptr<kp::TensorT<float>> tensorA = mgr.tensor({ 3, 4, 5 });
    std::shared_ptr<kp::TensorT<float>> tensorB = mgr.tensor({ 0, 0, 0 });

    std::vector<uint32_t> spirv(kp::TEST_OP_CUSTOM_SHADER_COMP_SPV.begin(),
                                kp::TEST_OP_CUSTOM_SHADER_COMP_SPV.end());
    std::vector<std::shared_ptr<kp::Tensor>> params = { tensorA, tensorB };

    mgr.sequence()
      ->eval<kp::OpTensorSyncDevice>(params)
      ->eval<kp::OpAlgoDispatch>(mgr.algorithm(params, spirv))
      ->eval<kp::OpTensorSyncLocal>(params);

    EXPECT_EQ(tensorA->vector(), std::vector<float>({ 0, 1, 2 }));
    EXPECT_EQ(tensorB->vector(), std::vector<float>({ 3, 4, 5 }));
}

