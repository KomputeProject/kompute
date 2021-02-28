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
        tensor->recordBufferMemoryBarrier(
          commandBuffer,
          vk::AccessFlagBits::eHostWrite,
          vk::AccessFlagBits::eShaderRead,
          vk::PipelineStageFlagBits::eHost,
          vk::PipelineStageFlagBits::eComputeShader);
    }

    this->mAlgorithm->bindCore(commandBuffer);
    this->mAlgorithm->bindPush(commandBuffer, this->mPushConstants);
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
