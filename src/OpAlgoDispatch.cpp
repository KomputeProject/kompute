// SPDX-License-Identifier: Apache-2.0

#include "kompute/operations/OpAlgoDispatch.hpp"

namespace kp {

OpAlgoDispatch::~OpAlgoDispatch() noexcept
{
    KP_LOG_DEBUG("Kompute OpAlgoDispatch destructor started");

    if (this->mPushConstantsData) {
        KP_LOG_DEBUG("Kompute freeing push constants data");
        free(this->mPushConstantsData);
    }
}

void
OpAlgoDispatch::record(const vk::CommandBuffer& commandBuffer)
{
    KP_LOG_DEBUG("Kompute OpAlgoDispatch record called");

    // Barrier to ensure the data is finished writing to buffer memory
    for (const std::shared_ptr<Memory>& mem :
         this->mAlgorithm->getMemObjects()) {

        // For images the image layout needs to be set to eGeneral before using
        // it for imageLoad/imageStore in a shader.
        if (mem->type() == Memory::Type::eImage) {
            std::shared_ptr<Image> image = std::static_pointer_cast<Image>(mem);

            image->recordPrimaryImageBarrier(
              commandBuffer,
              vk::AccessFlagBits::eTransferWrite,
              vk::AccessFlagBits::eShaderRead,
              vk::PipelineStageFlagBits::eTransfer,
              vk::PipelineStageFlagBits::eComputeShader,
              vk::ImageLayout::eGeneral);
        } else {
            mem->recordPrimaryMemoryBarrier(
              commandBuffer,
              vk::AccessFlagBits::eTransferWrite,
              vk::AccessFlagBits::eShaderRead,
              vk::PipelineStageFlagBits::eTransfer,
              vk::PipelineStageFlagBits::eComputeShader);
        }
    }

    if (this->mPushConstantsSize) {
        this->mAlgorithm->setPushConstants(
          this->mPushConstantsData,
          this->mPushConstantsSize,
          this->mPushConstantsDataTypeMemorySize);
    }

    this->mAlgorithm->recordBindCore(commandBuffer);
    this->mAlgorithm->recordBindPush(commandBuffer);
    this->mAlgorithm->recordDispatch(commandBuffer);
}

void
OpAlgoDispatch::preEval(const vk::CommandBuffer& /*commandBuffer*/)
{
    KP_LOG_DEBUG("Kompute OpAlgoDispatch preEval called");
}

void
OpAlgoDispatch::postEval(const vk::CommandBuffer& /*commandBuffer*/)
{
    KP_LOG_DEBUG("Kompute OpAlgoDispatch postSubmit called");
}

}
