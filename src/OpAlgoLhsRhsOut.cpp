#pragma once

#include "kompute/operations/OpAlgoLhsRhsOut.hpp"

namespace kp {

OpAlgoLhsRhsOut::OpAlgoLhsRhsOut()
{
    KP_LOG_DEBUG("Kompute OpAlgoLhsRhsOut constructor base");
}

OpAlgoLhsRhsOut::OpAlgoLhsRhsOut(
  std::vector<std::shared_ptr<Tensor>>& tensors,
            std::shared_ptr<Algorithm> algorithm)
  // The inheritance is initialised with the copyOutputData to false given that
  // this depencendant class handles the transfer of data via staging buffers in
  // a granular way.
  : OpAlgoCreate(tensors, algorithm)
{
    KP_LOG_DEBUG("Kompute OpAlgoLhsRhsOut constructor with params");
}

OpAlgoLhsRhsOut::~OpAlgoLhsRhsOut()
{
    KP_LOG_DEBUG("Kompute OpAlgoLhsRhsOut destructor started");
}

void
OpAlgoLhsRhsOut::init(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
            std::shared_ptr<vk::Device> device)
{
    KP_LOG_DEBUG("Kompute OpAlgoLhsRhsOut init called");

    if (this->mTensors.size() < 3) {
        throw std::runtime_error(
          "Kompute OpAlgoLhsRhsOut called with less than 1 tensor");
    } else if (this->mTensors.size() > 3) {
        KP_LOG_WARN(
          "Kompute OpAlgoLhsRhsOut called with more than 3 this->mTensors");
    }

    this->mTensorLHS = this->mTensors[0];
    this->mTensorRHS = this->mTensors[1];
    this->mTensorOutput = this->mTensors[2];

    if (!(this->mTensorLHS->isInit() && this->mTensorRHS->isInit() &&
          this->mTensorOutput->isInit())) {
        throw std::runtime_error(
          "Kompute OpAlgoLhsRhsOut all tensor parameters must be initialised. "
          "LHS: " +
          std::to_string(this->mTensorLHS->isInit()) +
          " RHS: " + std::to_string(this->mTensorRHS->isInit()) +
          " Output: " + std::to_string(this->mTensorOutput->isInit()));
    }

    if (!(this->mTensorLHS->size() == this->mTensorRHS->size() &&
          this->mTensorRHS->size() == this->mTensorOutput->size())) {
        throw std::runtime_error(
          "Kompute OpAlgoLhsRhsOut all tensor parameters must be the same size "
          "LHS: " +
          std::to_string(this->mTensorLHS->size()) +
          " RHS: " + std::to_string(this->mTensorRHS->size()) +
          " Output: " + std::to_string(this->mTensorOutput->size()));
    }

    KP_LOG_DEBUG("Kompute OpAlgoLhsRhsOut fetching spirv data");

    std::vector<uint32_t> shaderFileData = this->fetchSpirvBinaryData();

    KP_LOG_DEBUG("Kompute OpAlgoLhsRhsOut Initialising algorithm component");
}

void
OpAlgoLhsRhsOut::record(std::shared_ptr<vk::CommandBuffer> commandBuffer)
{
    KP_LOG_DEBUG("Kompute OpAlgoLhsRhsOut record called");

    // Barrier to ensure the data is finished writing to buffer memory
    this->mTensorLHS->recordBufferMemoryBarrier(
      this->mCommandBuffer,
      vk::AccessFlagBits::eHostWrite,
      vk::AccessFlagBits::eShaderRead,
      vk::PipelineStageFlagBits::eHost,
      vk::PipelineStageFlagBits::eComputeShader);
    this->mTensorRHS->recordBufferMemoryBarrier(
      this->mCommandBuffer,
      vk::AccessFlagBits::eHostWrite,
      vk::AccessFlagBits::eShaderRead,
      vk::PipelineStageFlagBits::eHost,
      vk::PipelineStageFlagBits::eComputeShader);

    this->mAlgorithm->recordDispatch(this->mKomputeWorkgroup[0],
                                     this->mKomputeWorkgroup[1],
                                     this->mKomputeWorkgroup[2]);

    // Barrier to ensure the shader code is executed before buffer read
    this->mTensorOutput->recordBufferMemoryBarrier(
      this->mCommandBuffer,
      vk::AccessFlagBits::eShaderWrite,
      vk::AccessFlagBits::eTransferRead,
      vk::PipelineStageFlagBits::eComputeShader,
      vk::PipelineStageFlagBits::eTransfer);

    if (this->mTensorOutput->tensorType() == Tensor::TensorTypes::eDevice) {
        this->mTensorOutput->recordCopyFromDeviceToStaging(this->mCommandBuffer,
                                                           true);
    }
}

void
OpAlgoLhsRhsOut::postEval()
{
    KP_LOG_DEBUG("Kompute OpAlgoLhsRhsOut postSubmit called");

    this->mTensorOutput->mapDataFromHostMemory();
}

}
