#pragma once

#include "kompute/Core.hpp"

#include "kompute/Tensor.hpp"

#include "kompute/operations/OpBase.hpp"

namespace kp {

/**
    Operation that syncs tensor's device by mapping local data into the device memory. For TensorTypes::eDevice it will use a staging tensor to perform the copy. For TensorTypes::eStaging it will only copy the data and perform a map, which will be executed during the record (as opposed to during the sequence eval/submit). This function cannot be carried out for TensorTypes::eStaging.
*/
class OpTensorSyncDevice : public OpBase
{
  public:
    OpTensorSyncDevice();

    /**
     * Default constructor with parameters that provides the core vulkan resources and the tensors that will be used in the operation. The tensos provided cannot be of type TensorTypes::eStorage.
     *
     * @param physicalDevice Vulkan physical device used to find device queues
     * @param device Vulkan logical device for passing to Algorithm
     * @param commandBuffer Vulkan Command Buffer to record commands into
     * @param tensors Tensors that will be used to create in operation.
     */
    OpTensorSyncDevice(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
                   std::shared_ptr<vk::Device> device,
                   std::shared_ptr<vk::CommandBuffer> commandBuffer,
                   std::vector<std::shared_ptr<Tensor>> tensors);

    /**
     * Default destructor. This class does not manage memory so it won't be expecting the parent to perform a release.
     */
    ~OpTensorSyncDevice() override;

    /**
     * Performs basic checks such as ensuring that there is at least one tensor provided, that they are initialized and that they are not of type TensorTpes::eStaging. For staging tensors in host memory, the map is performed during the init function.
     */
    void init() override;

    /**
     * For device tensors, it records the copy command to the device tensor from the temporary staging tensor.
     */
    void record() override;

    /**
     * Does not perform any further sync functions. Frees the staging tensors together with their respective memory.
     */
    void postSubmit() override;

  private:
    // Never owned resources
    std::vector<std::shared_ptr<Tensor>> mStagingTensors;
};

} // End namespace kp


