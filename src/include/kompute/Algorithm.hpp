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
     *  Default constructor for Algorithm
     *
     *  @param device The Vulkan device to use for creating resources
     *  @param commandBuffer The vulkan command buffer to bind the pipeline and
     * shaders
     */
    Algorithm(
            std::shared_ptr<vk::Device> device,
            const std::vector<std::shared_ptr<Tensor>>& tensors = {},
            const std::vector<uint32_t>& spirv = {},
            const Workgroup& workgroup = {},
            const Constants& specializationConstants = {},
            const Constants& pushConstants = {});

    /**
     * Initialiser for the shader data provided to the algorithm as well as
     * tensor parameters that will be used in shader.
     *
     * @param shaderFileData The bytes in spir-v format of the shader
     * @tensorParams The Tensors to be used in the Algorithm / shader for
     * @specalizationInstalces The specialization parameters to pass to the function
     * processing
     */
    void rebuild(
            const std::vector<std::shared_ptr<Tensor>>& tensors = {},
            const std::vector<uint32_t>& spirv = {},
            const Workgroup& workgroup = {},
            const Constants& specializationConstants = {},
            const Constants& pushConstants = {});

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
    void recordDispatch(std::shared_ptr<vk::CommandBuffer> commandBuffer);

    bool isInit();

    void setWorkgroup(const Workgroup& workgroup, uint32_t minSize = 1);

    const Workgroup& getWorkgroup();
    const Constants& getSpecializationConstants();
    const Constants& getPushConstants();
    const std::vector<std::shared_ptr<Tensor>>& getTensors();

    void destroy();

private:
    // -------------- NEVER OWNED RESOURCES
    std::shared_ptr<vk::Device> mDevice;
    std::vector<std::shared_ptr<Tensor>> mTensors;

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

    // -------------- ALWAYS OWNED RESOURCES
    std::vector<uint32_t> mSpirv;
    Constants mSpecializationConstants;
    Constants mPushConstants;
    Workgroup mWorkgroup;

    bool mIsInit;

    // Create util functions
    void createShaderModule();
    void createPipeline();

    // Parameters
    void createParameters();
};

} // End namespace kp
