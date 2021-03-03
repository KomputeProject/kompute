#pragma once

#include "kompute/operations/OpAlgoFactory.hpp"
#include "kompute/shaders/shaderopmult.hpp"

namespace kp {

OpAlgoFactory::OpAlgoFactory(
            const std::shared_ptr<kp::Algorithm>& algorithm,
            const std::vector<std::shared_ptr<kp::Tensor>>& tensors,
            const Workgroup& workgroup,
            const Constants& specializationConstants,
            const kp::Constants& pushConstants)
{
    KP_LOG_DEBUG("Kompute OpAlgoFactory constructor");

    this->mAlgorithm = algorithm;
    this->mPushConstants = pushConstants;
}

OpAlgoFactory::~OpAlgoFactory()
{
    KP_LOG_DEBUG("Kompute OpAlgoFactory destructor started");
}

void
OpAlgoFactory::record(const vk::CommandBuffer& commandBuffer)
{
    KP_LOG_DEBUG("Kompute OpAlgoFactory record called");

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
OpAlgoFactory::preEval(const vk::CommandBuffer& commandBuffer)
{
    KP_LOG_DEBUG("Kompute OpAlgoFactory preEval called");
}

void
OpAlgoFactory::postEval(const vk::CommandBuffer& commandBuffer)
{
    KP_LOG_DEBUG("Kompute OpAlgoFactory postSubmit called");
}

std::shared_ptr<kp::OpAlgoFactory>
OpAlgoFactory::push(const Constants& pushConstants)
{
    this->mPushConstants = pushConstants;
    return this->shared_from_this();
}

void
OpAlgoFactory::rebuildAlgorithmFromFactory() {
    const unsigned char* spirv;
    uint32_t len;
    uint32_t tensorMin;
    uint32_t tensorMax;

    switch (this->mAlgoType) {
        case AlgoTypes::eMult: {
            tensorMin = 3;
            tensorMax = 3;
            spirv = kp::shader_data::shaders_glsl_opmult_comp_spv;
            len = kp::shader_data::shaders_glsl_opmult_comp_spv_len;
            break;
        }
        default: {
            throw std::runtime_error("Kompute OpAlgoFactory invalid algo type");
        }
    }

    if(this->mTensors.size() < tensorMin || this->mTensors.size() > tensorMax) {
        throw std::runtime_error(fmt::format("Kompute OpAlgoFactory Tensors size max {} min {} given {}",
                tensorMax, tensorMin, this->mTensors.size()));
    }

    this->mAlgorithm->rebuild(
        this->mTensors,
        std::vector<uint32_t>((uint32_t*)spirv, (uint32_t*)(spirv + len)),
        this->mWorkgroup,
        this->mSpecializationConstants);
}

}
