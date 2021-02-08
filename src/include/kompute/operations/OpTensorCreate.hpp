#pragma once

#include "kompute/Core.hpp"

#include "kompute/Tensor.hpp"

#include "kompute/operations/OpBase.hpp"

namespace kp {

/**
    Operation that creates tensor and manages the memory of the components
   created
*/
class OpTensorCreate : public OpBase
{
  public:
    OpTensorCreate();

    /**
     * Default constructor with parameters that provides the bare minimum
     * requirements for the operations to be able to create and manage their
     * sub-components.
     *
     * @param physicalDevice Vulkan physical device used to find device queues
     * @param device Vulkan logical device for passing to Algorithm
     * @param commandBuffer Vulkan Command Buffer to record commands into
     * @param tensors Tensors that will be used to create in operation.
     * @param freeTensors Whether operation manages the memory of the Tensors
     */
    OpTensorCreate(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
                   std::shared_ptr<vk::Device> device,
                   std::shared_ptr<vk::CommandBuffer> commandBuffer,
                   std::vector<std::shared_ptr<Tensor>> tensors);

    /**
     * Default destructor which in this case expects the parent class to free
     * the tensors
     */
    ~OpTensorCreate() override;

    /**
     * In charge of initialising the primary Tensor as well as the staging
     * tensor as required. It will only initialise a staging tensor if the
     * Primary tensor is of type Device. For staging tensors it performs a 
     * mapDataIntoHostMemory which would perform immediately as opposed to 
     * on sequence eval/submission.
     */
    void init() override;

    /**
     * Record runs the core actions to create the tensors. For device tensors
     * it records a copyCommand to move the data from the staging tensor to the 
     * device tensor. The mapping for staging tensors happens in the init function
     * not in the record function.
     */
    void record() override;

    /**
     * Does not perform any preEval commands.
     */
    virtual void preEval() override;

    /**
     * Performs a copy back into the main tensor to ensure that the data
     * contained is the one that is now being stored in the GPU.
     */
    virtual void postEval() override;


  private:
};

} // End namespace kp
