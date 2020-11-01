#pragma once

#include "kompute/operations/OpAlgoLhsRhsOut.hpp"

namespace kp {

OpAlgoLhsRhsOut::OpAlgoLhsRhsOut()
{
    SPDLOG_DEBUG("Kompute OpAlgoLhsRhsOut constructor base");
}

OpAlgoLhsRhsOut::OpAlgoLhsRhsOut(
  std::shared_ptr<vk::PhysicalDevice> physicalDevice,
  std::shared_ptr<vk::Device> device,
  std::shared_ptr<vk::CommandBuffer> commandBuffer,
  std::vector<std::shared_ptr<Tensor>> tensors,
  KomputeWorkgroup komputeWorkgroup)
  // The inheritance is initialised with the copyOutputData to false given that
  // this depencendant class handles the transfer of data via staging buffers in
  // a granular way.
  : OpAlgoBase(physicalDevice, device, commandBuffer, tensors, komputeWorkgroup)
{
    SPDLOG_DEBUG("Kompute OpAlgoLhsRhsOut constructor with params");
}

OpAlgoLhsRhsOut::~OpAlgoLhsRhsOut()
{
    SPDLOG_DEBUG("Kompute OpAlgoLhsRhsOut destructor started");
}

void
OpAlgoLhsRhsOut::init()
{
    SPDLOG_DEBUG("Kompute OpAlgoLhsRhsOut init called");

    if (this->mTensors.size() < 3) {
        throw std::runtime_error(
          "Kompute OpAlgoLhsRhsOut called with less than 1 tensor");
    } else if (this->mTensors.size() > 3) {
        SPDLOG_WARN(
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

    this->mTensorOutputStaging = std::make_shared<Tensor>(
      this->mTensorOutput->data(), Tensor::TensorTypes::eStaging);

    this->mTensorOutputStaging->init(this->mPhysicalDevice, this->mDevice);

    SPDLOG_DEBUG("Kompute OpAlgoLhsRhsOut fetching spirv data");

    std::vector<char> shaderFileData = this->fetchSpirvBinaryData();

    SPDLOG_DEBUG("Kompute OpAlgoLhsRhsOut Initialising algorithm component");

    this->mAlgorithm->init(shaderFileData, this->mTensors);
}

void
OpAlgoLhsRhsOut::record()
{
    SPDLOG_DEBUG("Kompute OpAlgoLhsRhsOut record called");

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

    this->mAlgorithm->recordDispatch(this->mKomputeWorkgroup.x,
                                     this->mKomputeWorkgroup.y,
                                     this->mKomputeWorkgroup.z);

    // Barrier to ensure the shader code is executed before buffer read
    this->mTensorOutput->recordBufferMemoryBarrier(
      this->mCommandBuffer,
      vk::AccessFlagBits::eShaderWrite,
      vk::AccessFlagBits::eTransferRead,
      vk::PipelineStageFlagBits::eComputeShader,
      vk::PipelineStageFlagBits::eTransfer);

    this->mTensorOutputStaging->recordCopyFrom(
      this->mCommandBuffer, this->mTensorOutput, true);
}

void
OpAlgoLhsRhsOut::postEval()
{
    SPDLOG_DEBUG("Kompute OpAlgoLhsRhsOut postSubmit called");

    this->mTensorOutputStaging->mapDataFromHostMemory();

    this->mTensorOutput->setData(this->mTensorOutputStaging->data());
}

}
