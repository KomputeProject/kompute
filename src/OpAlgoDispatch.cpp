#pragma once

#include "kompute/operations/OpAlgoDispatch.hpp"

namespace kp {

OpAlgoDispatch::OpAlgoDispatch(const std::shared_ptr<kp::Algorithm>& algorithm,
                               const kp::Constants& pushConstants)
{
    KP_LOG_DEBUG("Kompute OpAlgoDispatch constructor");

    this->mAlgorithm = algorithm;
    this->mPushConstants = pushConstants;
}

OpAlgoDispatch::~OpAlgoDispatch()
{
    KP_LOG_DEBUG("Kompute OpAlgoDispatch destructor started");
}

void
OpAlgoDispatch::record(const vk::CommandBuffer& commandBuffer)
{
    KP_LOG_DEBUG("Kompute OpAlgoDispatch record called");

    // Barrier to ensure the data is finished writing to buffer memory
    for (const std::shared_ptr<Tensor>& tensor :
         this->mAlgorithm->getTensors()) {
        tensor->recordPrimaryBufferMemoryBarrier(
          commandBuffer,
          vk::AccessFlagBits::eTransferWrite,
          vk::AccessFlagBits::eShaderRead,
          vk::PipelineStageFlagBits::eTransfer,
          vk::PipelineStageFlagBits::eComputeShader);
    }

    if (this->mPushConstants.size()) {
        this->mAlgorithm->setPush(this->mPushConstants);
    }

    this->mAlgorithm->recordBindCore(commandBuffer);
    this->mAlgorithm->recordBindPush(commandBuffer);
    this->mAlgorithm->recordDispatch(commandBuffer);
}

void
OpAlgoDispatch::preEval(const vk::CommandBuffer& commandBuffer)
{
    KP_LOG_DEBUG("Kompute OpAlgoDispatch preEval called");
}

void
OpAlgoDispatch::postEval(const vk::CommandBuffer& commandBuffer)
{
    KP_LOG_DEBUG("Kompute OpAlgoDispatch postSubmit called");
}

}
