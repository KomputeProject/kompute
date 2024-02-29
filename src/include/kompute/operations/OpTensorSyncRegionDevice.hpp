// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "kompute/Core.hpp"
#include "kompute/Tensor.hpp"
#include "kompute/operations/OpBase.hpp"

namespace kp {

/**
 * Operation that syncs tensor's device by mapping local data into the device
 * memory. For TensorTypes::eDevice it will use a record operation for the
 * memory to be syncd into GPU memory which means that the operation will be
 * done in sync with GPU commands. For TensorTypes::eHost it will only map the
 * data into host memory which will happen during preEval before the recorded
 * commands are dispatched.
 */
class OpTensorSyncRegionDevice : public OpBase
{
  public:
    using ConstructorParameterType = std::vector<TensorRegion>;

    /**
     * Default constructor with parameters that provides the core vulkan
     * resources and the tensors that will be used in the operation. The tensors
     * provided cannot be of type TensorTypes::eStorage.
     * For each tensor a region to copy is specified.
     *
     * @param tensors Tensors that will be used to create in operation.
     * @param copyRegions The regions to copy.
     */
    OpTensorSyncRegionDevice(const std::vector<TensorRegion>& regions);

    /**
     * Default destructor. This class does not manage memory so it won't be
     * expecting the parent to perform a release.
     */
    ~OpTensorSyncRegionDevice() override;

    /**
     * For device tensors, it records the copy command for the tensor to copy
     * the data from its staging to device memory.
     *
     * @param commandBuffer The command buffer to record the command into.
     */
    void record(const vk::CommandBuffer& commandBuffer) override;

    /**
     * Does not perform any preEval commands.
     *
     * @param commandBuffer The command buffer to record the command into.
     */
    virtual void preEval(const vk::CommandBuffer& commandBuffer) override;

    /**
     * Does not perform any postEval commands.
     *
     * @param commandBuffer The command buffer to record the command into.
     */
    virtual void postEval(const vk::CommandBuffer& commandBuffer) override;

  private:
    // -------------- ALWAYS OWNED RESOURCES
    std::vector<TensorRegion> mRegions;
};

} // End namespace kp
