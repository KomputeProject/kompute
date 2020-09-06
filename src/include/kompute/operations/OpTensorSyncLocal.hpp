#pragma once

#include "kompute/Core.hpp"

#include "kompute/Tensor.hpp"

#include "kompute/operations/OpBase.hpp"

namespace kp {

/**
    Operation that syncs tensor's local data by mapping the data from device memory into the local vector. For TensorTypes::eDevice it will use a staging tensor to perform the copy. For TensorTypes::eStaging it will only copy the data and perform a map, which will be executed during the postSubmit (there will be no copy during the sequence eval/submit). This function cannot be carried out for TensorTypes::eStaging.
*/
class OpTensorSyncLocal : public OpBase
{
  public:
    OpTensorSyncLocal();

    /**
     * Default constructor with parameters that provides the core vulkan resources and the tensors that will be used in the operation. The tensors provided cannot be of type TensorTypes::eStorage.
     *
     * @param physicalDevice Vulkan physical device used to find device queues
     * @param device Vulkan logical device for passing to Algorithm
     * @param commandBuffer Vulkan Command Buffer to record commands into
     * @param tensors Tensors that will be used to create in operation.
     */
    OpTensorSyncLocal(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
                   std::shared_ptr<vk::Device> device,
                   std::shared_ptr<vk::CommandBuffer> commandBuffer,
                   std::vector<std::shared_ptr<Tensor>> tensors);

    /**
     * Default destructor. This class manages the memory of the staging tensors it owns but these are released in the postSubmit, before it arrives to the destructor.
     */
    ~OpTensorSyncLocal() override;

    /**
     * Performs basic checks such as ensuring that there is at least one tensor provided, that they are initialized and that they are not of type TensorTpes::eStaging.
     */
    void init() override;

    /**
     * For device tensors, it records the copy command into the staging tensor from the device tensor.
     */
    void record() override;

    /**
     * For host tensors it performs the map command from the host memory into local memory. Frees the staging tensors together with their respective memory.
     */
    void postSubmit() override;

  private:
    // Never owned resources
    std::vector<std::shared_ptr<Tensor>> mStagingTensors;
};

} // End namespace kp



