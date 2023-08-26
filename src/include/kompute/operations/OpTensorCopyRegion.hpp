// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "kompute/Core.hpp"

#include "kompute/Tensor.hpp"

#include "kompute/operations/OpBase.hpp"

namespace kp {

struct TensorCopyRegions {
  std::shared_ptr<Tensor> srcTensor;
  std::vector<TensorRegion> dstRegions;
};

/**
 * Operation that copies the data from the first tensor to the rest of the
 * tensors provided, using a record command for all the vectors. This operation
 * does not own/manage the memory of the tensors passed to it. The operation
 * must only receive tensors of type
 */
class OpTensorCopyRegion : public OpBase
{
  public:
    using ConstructorParameterType = TensorCopyRegions;

    /**
     * Default constructor with parameters that provides the core vulkan
     * resources and the tensors that will be used in the operation.
     *
     * @param tensors Tensors that will be used to create in operation.
     */
    OpTensorCopyRegion(const TensorCopyRegions regions);

    /**
     * Default destructor. This class does not manage memory so it won't be
     * expecting the parent to perform a release.
     */
    ~OpTensorCopyRegion() override;

    /**
     * Records the copy commands from the first tensor into all the other
     * tensors provided. Also optionally records a barrier.
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
     * Does not perform any preEval commands.
     *
     * @param commandBuffer The command buffer to record the command into.
     */
    virtual void postEval(const vk::CommandBuffer& commandBuffer) override;

  private:
    // -------------- ALWAYS OWNED RESOURCES
    TensorCopyRegions mRegions;
};

} // End namespace kp
