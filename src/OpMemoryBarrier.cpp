// SPDX-License-Identifier: Apache-2.0

#include "kompute/operations/OpMemoryBarrier.hpp"

namespace kp {

OpMemoryBarrier::OpMemoryBarrier(
  const std::vector<std::shared_ptr<Memory>>& memObjects,
  const vk::AccessFlagBits& srcAccessMask,
  const vk::AccessFlagBits& dstAccessMask,
  const vk::PipelineStageFlagBits& srcStageMask,
  const vk::PipelineStageFlagBits& dstStageMask,
  bool barrierOnPrimary) noexcept
  : mSrcAccessMask(srcAccessMask)
  , mDstAccessMask(dstAccessMask)
  , mSrcStageMask(srcStageMask)
  , mDstStageMask(dstStageMask)
  , mBarrierOnPrimary(barrierOnPrimary)
  , mMemObjects(memObjects)
{
    KP_LOG_DEBUG("Kompute OpMemoryBarrier constructor");
}

OpMemoryBarrier::~OpMemoryBarrier() noexcept
{
    KP_LOG_DEBUG("Kompute OpMemoryBarrier destructor started");
}

void
OpMemoryBarrier::record(const vk::CommandBuffer& commandBuffer)
{
    KP_LOG_DEBUG("Kompute OpMemoryBarrier record called");

    // Barrier to ensure the data is finished writing to buffer memory
    if (this->mBarrierOnPrimary) {
        for (const std::shared_ptr<Memory>& tensor : this->mMemObjects) {
            tensor->recordPrimaryMemoryBarrier(commandBuffer,
                                               this->mSrcAccessMask,
                                               this->mDstAccessMask,
                                               this->mSrcStageMask,
                                               this->mDstStageMask);
        }
    } else {
        for (const std::shared_ptr<Memory>& tensor : this->mMemObjects) {
            tensor->recordStagingMemoryBarrier(commandBuffer,
                                               this->mSrcAccessMask,
                                               this->mDstAccessMask,
                                               this->mSrcStageMask,
                                               this->mDstStageMask);
        }
    }
}

void
OpMemoryBarrier::preEval(const vk::CommandBuffer& /*commandBuffer*/)
{
    KP_LOG_DEBUG("Kompute OpMemoryBarrier preEval called");
}

void
OpMemoryBarrier::postEval(const vk::CommandBuffer& /*commandBuffer*/)
{
    KP_LOG_DEBUG("Kompute OpMemoryBarrier postSubmit called");
}

}
