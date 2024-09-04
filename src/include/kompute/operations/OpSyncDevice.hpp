// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "kompute/Core.hpp"
#include "kompute/Tensor.hpp"
#include "kompute/operations/OpBase.hpp"

namespace kp {

/**
 * Operation that syncs mem object's device memory by mapping local data into
 * the device memory. For MemoryTypes::eDevice it will use a record operation
 * for the memory to be syncd into GPU memory which means that the operation
 * will be done in sync with GPU commands. For MemoryTypes::eHost it will only
 * map the data into host memory which will happen during preEval before the
 * recorded commands are dispatched.
 */
class OpSyncDevice : public OpBase
{
  public:
    /**
     * Default constructor with parameters that provides the core vulkan
     * resources and the memory objects that will be used in the operation. The
     * memory objects provided cannot be of type MemoryTypes::eStorage.
     *
     * @param memObjects Memory objects that will be used to create in
     * operation.
     */
    OpSyncDevice(const std::vector<std::shared_ptr<Memory>>& memObjects);

    /**
     * Default destructor. This class does not manage memory so it won't be
     * expecting the parent to perform a release.
     */
    ~OpSyncDevice() override;

    /**
     * For device memory objects, it records the copy command for the memory
     * object to copy the data from its staging to device memory.
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
    std::vector<std::shared_ptr<Memory>> mMemObjects;
};

} // End namespace kp
