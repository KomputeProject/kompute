
#include <chrono>
#include <thread>

#include "Tensor.hpp"

// Only defining hpp file for syntax validation in editors
#ifndef OPMULT_H
#include "OpMult.hpp"
#endif

#ifndef OPMULT_CPP
#define OPMULT_CPP

namespace kp {

template<uint32_t tX, uint32_t tY, uint32_t tZ>
OpMult<tX, tY, tZ>::OpMult()
{
    SPDLOG_DEBUG("Kompute OpMult constructor base");
}

// TODO: Remove physicalDevice from main initialiser
template<uint32_t tX, uint32_t tY, uint32_t tZ>
OpMult<tX, tY, tZ>::OpMult(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
               std::shared_ptr<vk::Device> device,
               std::shared_ptr<vk::CommandBuffer> commandBuffer)
  : OpBase(physicalDevice, device, commandBuffer)
{
    SPDLOG_DEBUG("Kompute OpMult constructor with params");

    this->mAlgorithm = std::make_shared<Algorithm>(device, commandBuffer);
}

template<uint32_t tX, uint32_t tY, uint32_t tZ>
OpMult<tX, tY, tZ>::~OpMult()
{
    SPDLOG_DEBUG("Kompute OpMult destructor started");
}

template<uint32_t tX, uint32_t tY, uint32_t tZ>
void
OpMult<tX, tY, tZ>::init(std::vector<std::shared_ptr<Tensor>> tensors)
{
    SPDLOG_DEBUG("Kompute OpMult init called");

    if (tensors.size() < 3) {
        throw std::runtime_error(
          "Kompute OpMult called with less than 1 tensor");
    } else if (tensors.size() > 3) {
        spdlog::warn("Kompute OpMult called with more than 3 tensors");
    }

    this->mTensorLHS = tensors[0];
    this->mTensorRHS = tensors[1];
    this->mTensorOutput = tensors[2];

    // The dispatch size is set up based on either explicitly provided template parameters or by default it would take the shape and size of the tensors
    if (tX > 0) {
        // If at least the x value is provided we use mainly the parameters provided
        this->mX = tX;
        this->mY = tY > 0 ? tY : 1;
        this->mZ = tZ > 0 ? tZ : 1;
    }
    else {
        // TODO: Fully support the full size dispatch using size for the shape
        this->mX = this->mTensorLHS->size();
        this->mY = 1;
        this->mZ = 1;
    }
    spdlog::info("Kompute OpMult dispatch size X: {}, Y: {}, Z: {}", this->mX, this->mY, this->mZ);

    // TODO: Explore adding a validate function
    if (!(this->mTensorLHS->isInit() && this->mTensorRHS->isInit() &&
          this->mTensorOutput->isInit())) {
        throw std::runtime_error(
          "Kompute OpMult all tensor parameters must be initialised. LHS: " +
          std::to_string(this->mTensorLHS->isInit()) +
          " RHS: " + std::to_string(this->mTensorRHS->isInit()) +
          " Output: " + std::to_string(this->mTensorOutput->isInit()));
    }

    // TODO: Explore use-cases where tensors shouldn't be the same size, and how
    // to deal with those situations
    if (!(this->mTensorLHS->size() == this->mTensorRHS->size() &&
          this->mTensorRHS->size() == this->mTensorOutput->size())) {
        throw std::runtime_error(
          "Kompute OpMult all tensor parameters must be the same size LHS: " +
          std::to_string(this->mTensorLHS->size()) +
          " RHS: " + std::to_string(this->mTensorRHS->size()) +
          " Output: " + std::to_string(this->mTensorOutput->size()));
    }

    this->mTensorOutputStaging = std::make_shared<Tensor>(
      this->mTensorOutput->data(), Tensor::TensorTypes::eStaging);

    this->mTensorOutputStaging->init(this->mPhysicalDevice,
                                     this->mDevice,
                                     this->mCommandBuffer,
                                     this->mTensorOutput->data());

    // TODO: Make this path configurable
    this->mAlgorithm->init("shaders/glsl/opmult.comp.spv", tensors);
}

template<uint32_t tX, uint32_t tY, uint32_t tZ>
void
OpMult<tX, tY, tZ>::record()
{
    SPDLOG_DEBUG("Kompute OpMult record called");

    // Barrier to ensure the data is finished writing to buffer memory
    this->mTensorLHS->recordBufferMemoryBarrier(
        vk::AccessFlagBits::eHostWrite,
        vk::AccessFlagBits::eShaderRead,
        vk::PipelineStageFlagBits::eHost,
        vk::PipelineStageFlagBits::eComputeShader);
    this->mTensorRHS->recordBufferMemoryBarrier(
        vk::AccessFlagBits::eHostWrite,
        vk::AccessFlagBits::eShaderRead,
        vk::PipelineStageFlagBits::eHost,
        vk::PipelineStageFlagBits::eComputeShader);

    this->mAlgorithm->recordDispatch(this->mX, this->mY, this->mZ);

    // Barrier to ensure the shader code is executed before buffer read
    this->mTensorOutput->recordBufferMemoryBarrier(
        vk::AccessFlagBits::eShaderWrite,
        vk::AccessFlagBits::eTransferRead,
        vk::PipelineStageFlagBits::eComputeShader,
        vk::PipelineStageFlagBits::eTransfer);
    this->mTensorLHS->recordBufferMemoryBarrier(
        vk::AccessFlagBits::eShaderWrite,
        vk::AccessFlagBits::eTransferRead,
        vk::PipelineStageFlagBits::eComputeShader,
        vk::PipelineStageFlagBits::eTransfer);
    this->mTensorRHS->recordBufferMemoryBarrier(
        vk::AccessFlagBits::eShaderWrite,
        vk::AccessFlagBits::eTransferRead,
        vk::PipelineStageFlagBits::eComputeShader,
        vk::PipelineStageFlagBits::eTransfer);

    this->mTensorOutputStaging->recordCopyFrom(this->mTensorLHS);

    // Buffer to ensure wait until data is copied to staging buffer
    this->mTensorOutput->recordBufferMemoryBarrier(
        vk::AccessFlagBits::eTransferWrite,
        vk::AccessFlagBits::eHostRead,
        vk::PipelineStageFlagBits::eTransfer,
        vk::PipelineStageFlagBits::eHost);
    this->mTensorLHS->recordBufferMemoryBarrier(
        vk::AccessFlagBits::eTransferWrite,
        vk::AccessFlagBits::eHostRead,
        vk::PipelineStageFlagBits::eTransfer,
        vk::PipelineStageFlagBits::eHost);
    this->mTensorRHS->recordBufferMemoryBarrier(
        vk::AccessFlagBits::eTransferWrite,
        vk::AccessFlagBits::eHostRead,
        vk::PipelineStageFlagBits::eTransfer,
        vk::PipelineStageFlagBits::eHost);
}

template<uint32_t tX, uint32_t tY, uint32_t tZ>
void
OpMult<tX, tY, tZ>::postSubmit()
{
    SPDLOG_DEBUG("Kompute OpCreateTensor postSubmit called");

    this->mTensorOutputStaging->copyDataFromHostBuffer();

    this->mTensorOutput->setData(this->mTensorOutputStaging->data());
}

}

#endif // #ifndef OPMULT_CPP

