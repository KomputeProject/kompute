#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>

// SPDLOG_ACTIVE_LEVEL must be defined before spdlog.h import
#if DEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#endif

#include <spdlog/spdlog.h>

#include "Tensor.hpp"

namespace kp {

class Algorithm
{
  public:
    Algorithm();

    Algorithm(std::shared_ptr<vk::Device> device,
              std::shared_ptr<vk::CommandBuffer> commandBuffer);

    // TODO: Add specialisation data
    // TODO: Explore other ways of passing shader (ie raw bytes)
    void init(std::string shaderFilePath,
              std::vector<std::shared_ptr<Tensor>> tensorParams);

    ~Algorithm();

    // Record commands
    void recordDispatch(uint32_t x = 1, uint32_t y = 1, uint32_t z = 1);

  private:
    // Shared resources
    std::shared_ptr<vk::Device> mDevice;
    std::shared_ptr<vk::CommandBuffer> mCommandBuffer;

    // Resources owned by default
    std::shared_ptr<vk::DescriptorSetLayout> mDescriptorSetLayout;
    bool mFreeDescriptorSetLayout = false;
    std::shared_ptr<vk::DescriptorPool> mDescriptorPool;
    bool mFreeDescriptorPool = false;
    // TODO: Explore design for multiple descriptor sets
    std::shared_ptr<vk::DescriptorSet> mDescriptorSet;
    bool mFreeDescriptorSet = false;
    std::shared_ptr<vk::ShaderModule> mShaderModule;
    bool mFreeShaderModule = false;
    std::shared_ptr<vk::PipelineLayout> mPipelineLayout;
    bool mFreePipelineLayout = false;
    std::shared_ptr<vk::PipelineCache> mPipelineCache;
    bool mFreePipelineCache = false;
    std::shared_ptr<vk::Pipeline> mPipeline;
    bool mFreePipeline = false;

    // Create util functions
    void createShaderModule(std::string shaderFilePath);
    void createPipeline();
    // Parameters
    void createParameters(std::vector<std::shared_ptr<Tensor>>& tensorParams);
    void createDescriptorPool();

};

} // End namespace kp
