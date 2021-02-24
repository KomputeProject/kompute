#pragma once

#include "kompute/Core.hpp"
#include "kompute/Algorithm.hpp"
#include "kompute/Tensor.hpp"
#include "kompute/operations/OpBase.hpp"

namespace kp {

/**
 * Operation that provides a general abstraction that simplifies the use of 
 * algorithm and parameter components which can be used with shaders.
 * By default it enables the user to provide a dynamic number of tensors
 * which are then passed as inputs.
 */
class OpAlgoDispatch : public OpBase
{
  public:

    /**
     * Default constructor with parameters that provides the bare minimum
     * requirements for the operations to be able to create and manage their
     * sub-components.
     *
     * @param physicalDevice Vulkan physical device used to find device queues
     * @param device Vulkan logical device for passing to Algorithm
     * @param commandBuffer Vulkan Command Buffer to record commands into
     * @param tensors Tensors that are to be used in this operation
     * @param shaderFilePath Optional parameter to specify the shader to load (either in spirv or raw format)
     * @param komputeWorkgroup Optional parameter to specify the layout for processing
     */
    OpAlgoDispatch(std::vector<std::shared_ptr<Tensor>> tensors,
           std::shared_ptr<kp::Algorithm> algorithm);

    /**
     * Constructor that enables a file to be passed to the operation with
     * the contents of the shader. This can be either in raw format or in
     * compiled SPIR-V binary format.
     *
     * @param physicalDevice Vulkan physical device used to find device queues
     * @param device Vulkan logical device for passing to Algorithm
     * @param commandBuffer Vulkan Command Buffer to record commands into
     * @param tensors Tensors that are to be used in this operation
     * @param shaderFilePath Parameter to specify the shader to load (either in spirv or raw format)
     * @param komputeWorkgroup Optional parameter to specify the layout for processing
     */
    OpAlgoDispatch(std::vector<std::shared_ptr<Tensor>>& tensors,
           std::shared_ptr<kp::Algorithm>& algorithm,
           std::string shaderFilePath);

    /**
     * Constructor that enables raw shader data to be passed to the main operation
     * which can be either in raw shader glsl code or in compiled SPIR-V binary.
     *
     * @param physicalDevice Vulkan physical device used to find device queues
     * @param device Vulkan logical device for passing to Algorithm
     * @param commandBuffer Vulkan Command Buffer to record commands into
     * @param tensors Tensors that are to be used in this operation
     * @param shaderDataRaw Optional parameter to specify the shader data either in binary or raw form
     * @param komputeWorkgroup Optional parameter to specify the layout for processing
     */
    OpAlgoDispatch(std::vector<std::shared_ptr<Tensor>>& tensors,
           std::shared_ptr<kp::Algorithm>& algorithm,
           const std::vector<uint32_t>& shaderDataRaw);

    /**
     * Default destructor, which is in charge of destroying the algorithm
     * components but does not destroy the underlying tensors
     */
    virtual ~OpAlgoDispatch() override;

    /**
     * The init function is responsible for the initialisation of the algorithm
     * component based on the parameters specified, and allows for extensibility
     * on the options provided. Further dependent classes can perform more 
     * specific checks such as ensuring tensors provided are initialised, etc.
     */
    virtual void init(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
            std::shared_ptr<vk::Device> device) override;

    /**
     * This records the commands that are to be sent to the GPU. This includes
     * the barriers that ensure the memory has been copied before going in and
     * out of the shader, as well as the dispatch operation that sends the
     * shader processing to the gpu. This function also records the GPU memory
     * copy of the output data for the staging buffer so it can be read by the
     * host.
     */
    virtual void record(std::shared_ptr<vk::CommandBuffer> commandBuffer) override;


    /**
     * Does not perform any preEval commands.
     */
    virtual void preEval() override;

    /**
     * Executes after the recorded commands are submitted, and performs a copy
     * of the GPU Device memory into the staging buffer so the output data can
     * be retrieved.
     */
    virtual void postEval() override;

};

} // End namespace kp

