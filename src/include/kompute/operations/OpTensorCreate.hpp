#pragma once

#include "kompute/Core.hpp"

#include "kompute/operations/OpBase.hpp"
#include "kompute/Tensor.hpp"
#include "kompute/Algorithm.hpp"

namespace kp {

/**
 *  Base Operation which provides the high level interface that Kompute
 *  operations implement in order to perform a set of actions in the GPU.
 *
 *  Operations can perform actions on tensors, and optionally can also own an
 *  Algorithm with respective parameters. kp::Operations with kp::Algorithms
 *  would inherit from kp::OpBaseAlgo.
 */
class OpTensorCreate : public OpBase
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
     */
    OpTensorCreate(std::vector<std::shared_ptr<Tensor>>& tensors);

    /**
     * Default destructor for OpTensorCreate class. This OpTensorCreate destructor class should
     * always be called to destroy and free owned resources unless it is
     * intended to destroy the resources in the parent class.
     */
    virtual ~OpTensorCreate() override;

    /**
     * The init function is responsible for setting up all the resources and
     * should be called after the Operation has been created.
     */
    virtual void init(
            std::shared_ptr<vk::PhysicalDevice> physicalDevice,
            std::shared_ptr<vk::Device> device) override;

    /**
     * Record runs the core actions to create the tensors. For device tensors
     * it records a copyCommand to move the data from the staging tensor to the 
     * device tensor. The mapping for staging tensors happens in the init function
     * not in the record function.
     */
    void record(std::shared_ptr<vk::CommandBuffer> commandBuffer) override;

    /**
     * Does not perform any preEval commands.
     */
    virtual void preEval() override;

    /**
     * Performs a copy back into the main tensor to ensure that the data
     * contained is the one that is now being stored in the GPU.
     */
    virtual void postEval() override;

};

} // End namespace kp
