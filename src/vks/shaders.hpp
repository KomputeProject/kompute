#pragma once

#include <string>
#include <vulkan/vulkan.hpp>

namespace vks { namespace shaders {

vk::ShaderModule loadShaderModule(const vk::Device& device, const std::string& filename);

// Load a SPIR-V shader
vk::PipelineShaderStageCreateInfo loadShader(const vk::Device& device,
                                             const std::string& fileName,
                                             vk::ShaderStageFlagBits stage,
                                             const char* entryPoint = "main");
}}  // namespace vks::shaders
