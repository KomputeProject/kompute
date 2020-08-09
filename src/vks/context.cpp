#include "context.hpp"

using namespace vks;

#ifdef WIN32
__declspec(thread) vk::CommandPool Context::s_cmdPool;
#else
thread_local vk::CommandPool Context::s_cmdPool;
#endif

#if 0
#if defined(__ANDROID__)
requireExtension(VK_KHR_SURFACE_EXTENSION_NAME);
requireExtension(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#else
requireExtensions(glfw::getRequiredInstanceExtensions());
#endif

requireDeviceExtensions({ VK_KHR_SWAPCHAIN_EXTENSION_NAME });



// Load a SPIR-V shader
inline vk::PipelineShaderStageCreateInfo loadShader(const std::string& fileName, vk::ShaderStageFlagBits stage) const {
    vk::PipelineShaderStageCreateInfo shaderStage;
    shaderStage.stage = stage;
    shaderStage.module = vkx::loadShader(fileName, device, stage);
    shaderStage.pName = "main"; // todo : make param
    assert(shaderStage.module);
    shaderModules.push_back(shaderStage.module);
    return shaderStage;
}

#endif
