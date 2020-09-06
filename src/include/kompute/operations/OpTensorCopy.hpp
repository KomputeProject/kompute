#pragma once

#include "kompute/Core.hpp"

#include "kompute/Tensor.hpp"

#include "kompute/operations/OpBase.hpp"

namespace kp {

/**
    Operation that copies the data from the first tensor to the rest of the tensors provided, using a record command for all the vectors. This operation does not own/manage the memory of the tensors passed to it.
*/
class OpTensorCopy : public OpBase
{
  public:
    OpTensorCopy();

    /**
     * Default constructor with parameters that provides the core vulkan resources and the tensors that will be used in the operation.
     *
     * @param physicalDevice Vulkan physical device used to find device queues
     * @param device Vulkan logical device for passing to Algorithm
     * @param commandBuffer Vulkan Command Buffer to record commands into
     * @param tensors Tensors that will be used to create in operation.
     */
    OpTensorCopy(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
                   std::shared_ptr<vk::Device> device,
                   std::shared_ptr<vk::CommandBuffer> commandBuffer,
                   std::vector<std::shared_ptr<Tensor>> tensors);

    /**
     * Default destructor which in this case expects the parent class to free
     * the tensors
     */
    ~OpTensorCopy() override;

    /**
     * TODO
     */
    void init() override;

    /**
     * Records the copy commands from teh first tensor into all the other tensors provided. Also optionally records a barrier.
     */
    void record() override;

    /**
     * Copies the local vectors for all the tensors to sync the data with the gpu.
     */
    void postSubmit() override;

  private:
};

} // End namespace kp

