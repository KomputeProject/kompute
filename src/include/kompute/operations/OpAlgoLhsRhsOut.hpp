#pragma once

#include <fstream>

#include "kompute/Core.hpp"

#include "kompute/Algorithm.hpp"
#include "kompute/Tensor.hpp"

#include "kompute/operations/OpAlgoBase.hpp"

namespace kp {

/**
 * Operation base class to simplify the creation of operations that require
 * right hand and left hand side datapoints together with a single output.
 * The expected data passed is two input tensors and one output tensor.
 */
class OpAlgoLhsRhsOut : public OpAlgoBase
{
  public:
    /**
     *  Base constructor, should not be used unless explicitly intended.
     */
    OpAlgoLhsRhsOut();

    /**
     * Default constructor with parameters that provides the bare minimum
     * requirements for the operations to be able to create and manage their
     * sub-components.
     *
     * @param physicalDevice Vulkan physical device used to find device queues
     * @param device Vulkan logical device for passing to Algorithm
     * @param commandBuffer Vulkan Command Buffer to record commands into
     * @param tensors Tensors that are to be used in this operation
     * @param freeTensors Whether operation manages the memory of the Tensors
     * @param komputeWorkgroup Optional parameter to specify the layout for processing
     */
    OpAlgoLhsRhsOut(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
           std::shared_ptr<vk::Device> device,
           std::shared_ptr<vk::CommandBuffer> commandBuffer,
           std::vector<std::shared_ptr<Tensor>> tensors,
           KomputeWorkgroup komputeWorkgroup = KomputeWorkgroup());

    /**
     * Default destructor, which is in charge of destroying the algorithm
     * components but does not destroy the underlying tensors
     */
    virtual ~OpAlgoLhsRhsOut() override;

    /**
     * The init function is responsible for ensuring that all of the tensors
     * provided are aligned with requirements such as LHS, RHS and Output
     * tensors, and  creates the algorithm component which processes the
     * computation.
     */
    virtual void init() override;

    /**
     * This records the commands that are to be sent to the GPU. This includes
     * the barriers that ensure the memory has been copied before going in and
     * out of the shader, as well as the dispatch operation that sends the
     * shader processing to the gpu. This function also records the GPU memory
     * copy of the output data for the staging buffer so it can be read by the
     * host.
     */
    virtual void record() override;

    /**
     * Executes after the recorded commands are submitted, and performs a copy
     * of the GPU Device memory into the staging buffer so the output data can
     * be retrieved.
     */
    virtual void postEval() override;

  protected:
    // -------------- NEVER OWNED RESOURCES
    std::shared_ptr<Tensor> mTensorLHS; ///< Reference to the parameter used in the left hand side equation of the shader
    std::shared_ptr<Tensor> mTensorRHS; ///< Reference to the parameter used in the right hand side equation of the shader
    std::shared_ptr<Tensor> mTensorOutput; ///< Reference to the parameter used in the output of the shader and will be copied with a staging vector

    // -------------- ALWAYS OWNED RESOURCES
    std::shared_ptr<Tensor> mTensorOutputStaging; ///< Staging temporary tensor user do to copy the output of the tensor
};

} // End namespace kp

