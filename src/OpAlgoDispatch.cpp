#pragma once

#include "kompute/operations/OpAlgoDispatch.hpp"

namespace kp {

OpAlgoDispatch::OpAlgoDispatch(const std::vector<std::shared_ptr<Tensor>>& tensors,
           const std::shared_ptr<kp::Algorithm>& algorithm)
{
    KP_LOG_DEBUG("Kompute OpAlgoDispatch constructor");

}

OpAlgoDispatch::~OpAlgoDispatch()
{
    KP_LOG_DEBUG("Kompute OpAlgoDispatch destructor started");
}

void
OpAlgoDispatch::record(std::shared_ptr<vk::CommandBuffer> commandBuffer)
{
    KP_LOG_DEBUG("Kompute OpAlgoDispatch record called");

    // Barrier to ensure the data is finished writing to buffer memory
    for (std::shared_ptr<Tensor> tensor : this->mTensors) {
        tensor->recordBufferMemoryBarrier(
          commandBuffer,
          vk::AccessFlagBits::eHostWrite,
          vk::AccessFlagBits::eShaderRead,
          vk::PipelineStageFlagBits::eHost,
          vk::PipelineStageFlagBits::eComputeShader);
    }

    this->mAlgorithm->recordDispatch(commandBuffer);
}

void
OpAlgoDispatch::preEval()
{
    KP_LOG_DEBUG("Kompute OpAlgoDispatch preEval called");
}

void
OpAlgoDispatch::postEval()
{
    KP_LOG_DEBUG("Kompute OpAlgoDispatch postSubmit called");
}

}
