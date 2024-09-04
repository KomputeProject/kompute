// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "kompute/Core.hpp"

#include "kompute/Memory.hpp"

#include "kompute/operations/OpBase.hpp"

namespace kp {

/**
 * Operation that copies the data from the first memory object to the rest of
 * the memory objects provided, using a record command for all the vectors. This
 * operation does not own/manage the memory of the memory objects passed to it.
 */
class OpCopy : public OpBase
{
  public:
    /**
     * Default constructor with parameters that provides the core vulkan
     * resources and the memory objects that will be used in the operation.
     *
     * @param memObjects memory objects that will be used to create in
     * operation.
     */
    OpCopy(const std::vector<std::shared_ptr<Memory>>& memObjects);

    /**
     * Default destructor. This class does not manage memory so it won't be
     * expecting the parent to perform a release.
     */
    ~OpCopy() override;

    /**
     * Records the copy commands from the first memory object into all the other
     * memory objects provided. Also optionally records a barrier.
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
     * Copies the local vectors for all the memory objects to sync the data with
     * the gpu.
     *
     * @param commandBuffer The command buffer to record the command into.
     */
    virtual void postEval(const vk::CommandBuffer& commandBuffer) override;

  private:
    // -------------- ALWAYS OWNED RESOURCES
    std::vector<std::shared_ptr<Memory>> mMemObjects;
};

} // End namespace kp
