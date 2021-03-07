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
     *  Main constructor for algorithm with configuration parameters to create
     *  the underlying resources.
     *
     *  @param device The Vulkan device to use for creating resources
     *  @param tensors (optional) The tensors to use to create the descriptor
     * resources
     *  @param spirv (optional) The spirv code to use to create the algorithm
     *  @param workgroup (optional) The kp::Workgroup to use for the dispatch
     * which defaults to kp::Workgroup(tensor[0].size(), 1, 1) if not set.
     *  @param specializationConstants (optional) The kp::Constants to use to
     * initialize the specialization constants which cannot be changed once set.
     *  @param pushConstants (optional) The kp::Constants to use when
     * initializing the pipeline, which set the size of the push constants -
     * these can be modified but all new values must have the same vector size
     * as this initial value.
     */
    Algorithm(std::shared_ptr<vk::Device> device,
              const std::vector<std::shared_ptr<Tensor>>& tensors = {},
              const std::vector<uint32_t>& spirv = {},
              const Workgroup& workgroup = {},
              const Constants& specializationConstants = {},
              const Constants& pushConstants = {});

    /**
     *  Rebuild function to reconstruct algorithm with configuration parameters
     * to create the underlying resources.
     *
     *  @param tensors The tensors to use to create the descriptor resources
     *  @param spirv The spirv code to use to create the algorithm
     *  @param workgroup (optional) The kp::Workgroup to use for the dispatch
     * which defaults to kp::Workgroup(tensor[0].size(), 1, 1) if not set.
     *  @param specializationConstants (optional) The kp::Constants to use to
     * initialize the specialization constants which cannot be changed once set.
     *  @param pushConstants (optional) The kp::Constants to use when
     * initializing the pipeline, which set the size of the push constants -
     * these can be modified but all new values must have the same vector size
     * as this initial value.
     */
    void rebuild(const std::vector<std::shared_ptr<Tensor>>& tensors,
                 const std::vector<uint32_t>& spirv,
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
     * @param commandBuffer Command buffer to record the algorithm resources to
     */
    void recordDispatch(const vk::CommandBuffer& commandBuffer);

    /**
     * Records command that binds the "core" algorithm components which consist
     * of binding the pipeline and binding the descriptorsets.
     *
     * @param commandBuffer Command buffer to record the algorithm resources to
     */
    void recordBindCore(const vk::CommandBuffer& commandBuffer);

    /**
     * Records command that binds the push constants to the command buffer
     * provided
     * - it is required that the pushConstants provided are of the same size as
     * the ones provided during initialization.
     *
     * @param commandBuffer Command buffer to record the algorithm resources to
     */
    void recordBindPush(const vk::CommandBuffer& commandBuffer);

    /**
     * function that checks all the gpu resource components to verify if these
     * have been created and returns true if all are valid.
     *
     * @returns returns true if the algorithm is currently initialized.
     */
    bool isInit();

    /**
     * Sets the work group to use in the recordDispatch
     *
     * @param workgroup The kp::Workgroup value to use to update the algorithm.
     * It must have a value greater than 1 on the x value (index 1) otherwise it
     * will be initialized on the size of the first tensor (ie.
     * this->mTensor[0]->size())
     */
    void setWorkgroup(const Workgroup& workgroup, uint32_t minSize = 1);
    /**
     * Sets the push constants to the new value provided to use in the next
     * bindPush()
     *
     * @param The kp::Constant to use to set the push constants to use in the
     * next bindPush(...) calls. The constants provided must be of the same size
     * as the ones created during initialization.
     */
    void setPush(const Constants& pushConstants);

    /**
     * Gets the current workgroup from the algorithm.
     *
     * @param The kp::Constant to use to set the push constants to use in the
     * next bindPush(...) calls. The constants provided must be of the same size
     * as the ones created during initialization.
     */
    const Workgroup& getWorkgroup();
    /**
     * Gets the specialization constants of the current algorithm.
     *
     * @returns The kp::Constants currently set for specialization constants
     */
    const Constants& getSpecializationConstants();
    /**
     * Gets the specialization constants of the current algorithm.
     *
     * @returns The kp::Constants currently set for push constants
     */
    const Constants& getPush();
    /**
     * Gets the current tensors that are used in the algorithm.
     *
     * @returns The list of tensors used in the algorithm.
     */
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

    // Create util functions
    void createShaderModule();
    void createPipeline();

    // Parameters
    void createParameters();
};

} // End namespace kp
