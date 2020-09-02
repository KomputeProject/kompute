#pragma once

#include "kompute/Core.hpp"

#include "kompute/Tensor.hpp"

namespace kp {

/**
    Abstraction for compute shaders that are run on top of tensors grouped via
   ParameterGroups (which group descriptorsets)
*/
class Algorithm
{
  public:
    /**
        Base constructor for Algorithm. Should not be used unless explicit
       intended.
    */
    Algorithm();

    /**
     *  Default constructor for Algorithm
     *
     *  @param device The Vulkan device to use for creating resources
     *  @param commandBuffer The vulkan command buffer to bind the pipeline and
     * shaders
     */
    Algorithm(std::shared_ptr<vk::Device> device,
              std::shared_ptr<vk::CommandBuffer> commandBuffer);

    /**
     * Initialiser for the shader data provided to the algoithm as well as
     * tensor parameters that will be used in shader.
     *
     * @param shaderFileData The bytes in spir-v format of the shader
     * @tensorParams The Tensors to be used in the Algorithm / shader for
     * processing
     */
    void init(const std::vector<char>& shaderFileData,
              std::vector<std::shared_ptr<Tensor>> tensorParams);

    /**
     * Destructor for Algorithm which is responsible for freeing and desroying
     * respective pipelines and owned parameter groups.
     */
    ~Algorithm();

    /**
     * Records the dispatch function with the provided template parameters or
     * alternatively using the size of the tensor by default.
     *
     * @param x Layout X dispatch value
     * @param y Layout Y dispatch value
     * @param z Layout Z dispatch value
     */
    void recordDispatch(uint32_t x = 1, uint32_t y = 1, uint32_t z = 1);

  private:
    // -------------- NEVER OWNED RESOURCES
    std::shared_ptr<vk::Device> mDevice;
    std::shared_ptr<vk::CommandBuffer> mCommandBuffer;

    // -------------- OPTIONALLY OWNED RESOURCES
    std::shared_ptr<vk::DescriptorSetLayout> mDescriptorSetLayout;
    bool mFreeDescriptorSetLayout = false;
    std::shared_ptr<vk::DescriptorPool> mDescriptorPool;
    bool mFreeDescriptorPool = false;
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
    void createShaderModule(const std::vector<char>& shaderFileData);
    void createPipeline(std::vector<uint32_t> specializationData = {});

    // Parameters
    void createParameters(std::vector<std::shared_ptr<Tensor>>& tensorParams);
    void createDescriptorPool();
};

} // End namespace kp
