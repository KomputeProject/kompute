#pragma once

#include "kompute/Core.hpp"
#include "kompute/operations/OpBase.hpp"
#include "kompute/Tensor.hpp"
#include "kompute/operations/OpBase.hpp"

namespace kp {

/**
    Operation that syncs tensor's device by mapping local data into the device memory. For TensorTypes::eDevice it will use a record operation for the memory to be syncd into GPU memory which means that the operation will be done in sync with GPU commands. For TensorTypes::eStaging it will only map the data into host memory which will happen during preEval before the recorded commands are dispatched. This operation won't have any effect on TensorTypes::eStaging.
*/
class OpTensorSyncDevice : public OpBase
{
  public:
    /**
     * Default constructor with parameters that provides the core vulkan resources and the tensors that will be used in the operation. The tensos provided cannot be of type TensorTypes::eStorage.
     *
     * @param physicalDevice Vulkan physical device used to find device queues
     * @param device Vulkan logical device for passing to Algorithm
     * @param commandBuffer Vulkan Command Buffer to record commands into
     * @param tensors Tensors that will be used to create in operation.
     */
    OpTensorSyncDevice(const std::vector<std::shared_ptr<Tensor>>& tensors);

    /**
     * Default destructor. This class does not manage memory so it won't be expecting the parent to perform a release.
     */
    ~OpTensorSyncDevice() override;

    /**
     * For device tensors, it records the copy command for the tensor to copy the data from its staging to device memory.
     */
    void record(std::shared_ptr<vk::CommandBuffer> commandBuffer) override;

    /**
     * Does not perform any preEval commands.
     */
    virtual void preEval() override;

    /**
     * Does not perform any postEval commands.
     */
    virtual void postEval() override;

  private:
    // -------------- ALWAYS OWNED RESOURCES
    std::vector<std::shared_ptr<Tensor>> mTensors;
};

} // End namespace kp


