#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"

#include "kompute_test/Shader.hpp"

static const std::string shaderString = (R"(
    #version 450

    layout (local_size_x = 1) in;

    // The input tensors bind index is relative to index in parameter passed
    layout(set = 0, binding = 0) buffer bina { float tina[]; };
    layout(set = 0, binding = 1) buffer binb { float tinb[]; };
    layout(set = 0, binding = 2) buffer bout { float tout[]; };

    void main() {
        uint index = gl_GlobalInvocationID.x;
        
        int i = 1;
        
        while(i < 200) {
            tout[index] += tina[index] * tinb[index];
            i++;
        }
    }
)");

void
compileShaderWithGivenResources(const std::string shaderString,
                                const TBuiltInResource resources)
{
    kp_test_utils::Shader::compileSource(
      shaderString,
      std::string("main"),
      std::vector<std::pair<std::string, std::string>>({}),
      resources);
}

TEST(TestShaderResources, TestNoMaxLight)
{
    TBuiltInResource noMaxLightResources = kp_test_utils::Shader::defaultResource;
    noMaxLightResources.maxLights = 0;

    EXPECT_NO_THROW(
      compileShaderWithGivenResources(shaderString, noMaxLightResources));
}

TEST(TestShaderResources, TestSmallComputeWorkGroupSizeX)
{
    TBuiltInResource smallComputeWorkGroupSizeXResources =
      kp_test_utils::Shader::defaultResource;
    smallComputeWorkGroupSizeXResources.maxComputeWorkGroupSizeX = 0;

    ASSERT_THROW(compileShaderWithGivenResources(
                   shaderString, smallComputeWorkGroupSizeXResources),
                 std::runtime_error);
}

TEST(TestShaderResources, TestNoWhileLoopLimit)
{
    TBuiltInResource noWhileLoopLimitResources = kp_test_utils::Shader::defaultResource;
    noWhileLoopLimitResources.limits.whileLoops = 0;

    ASSERT_THROW(
      compileShaderWithGivenResources(shaderString, noWhileLoopLimitResources),
      std::runtime_error);
}
