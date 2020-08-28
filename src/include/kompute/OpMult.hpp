#pragma once

#include <fstream>

#include "kompute/Core.hpp"

#include "kompute/shaders/shaderopmult.hpp"

#include "kompute/Algorithm.hpp"
#include "kompute/Tensor.hpp"

#include "kompute/OpBase.hpp"

namespace kp {

/**
    Base algorithm based operation
*/
template<uint32_t tX = 0, uint32_t tY = 0, uint32_t tZ = 0>
class OpMult : public OpBase
{
  public:
    /**
        Constructor
    */
    OpMult();

    OpMult(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
           std::shared_ptr<vk::Device> device,
           std::shared_ptr<vk::CommandBuffer> commandBuffer);

    ~OpMult();

    void init(std::vector<std::shared_ptr<Tensor>> tensors) override;

    void record() override;

    void postSubmit() override;

  private:
    // Always owned resources
    std::shared_ptr<Tensor> mTensorOutputStaging;

    // Optionally owned resources
    std::shared_ptr<Algorithm> mAlgorithm;
    bool mFreeAlgorithm = false;

    // Never owned resources
    std::shared_ptr<Tensor> mTensorLHS;
    std::shared_ptr<Tensor> mTensorRHS;
    std::shared_ptr<Tensor> mTensorOutput;

    uint32_t mX;
    uint32_t mY;
    uint32_t mZ;
};

} // End namespace kp

// Including implemenation for template class
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

    // The dispatch size is set up based on either explicitly provided template
    // parameters or by default it would take the shape and size of the tensors
    if (tX > 0) {
        // If at least the x value is provided we use mainly the parameters
        // provided
        this->mX = tX;
        this->mY = tY > 0 ? tY : 1;
        this->mZ = tZ > 0 ? tZ : 1;
    } else {
        // TODO: Fully support the full size dispatch using size for the shape
        this->mX = this->mTensorLHS->size();
        this->mY = 1;
        this->mZ = 1;
    }
    spdlog::info("Kompute OpMult dispatch size X: {}, Y: {}, Z: {}",
                 this->mX,
                 this->mY,
                 this->mZ);

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

    this->mTensorOutputStaging->init(
      this->mPhysicalDevice, this->mDevice, this->mCommandBuffer);

#if RELEASE
    std::vector<char> shaderFileData(
      shader_data::shaders_glsl_opmult_comp_spv,
      shader_data::shaders_glsl_opmult_comp_spv +
        kp::shader_data::shaders_glsl_opmult_comp_spv_len);
#else
    SPDLOG_DEBUG(
      "Kompute OpMult Running debug loading shaders directly from spirv file");

    // TODO: Move to utility function
    std::string shaderFilePath = "shaders/glsl/opmult.comp.spv";
    std::ifstream fileStream(shaderFilePath,
                             std::ios::binary | std::ios::in | std::ios::ate);

    size_t shaderFileSize = fileStream.tellg();
    fileStream.seekg(0, std::ios::beg);
    char* shaderDataRaw = new char[shaderFileSize];
    fileStream.read(shaderDataRaw, shaderFileSize);
    fileStream.close();

    std::vector<char> shaderFileData(shaderDataRaw,
                                     shaderDataRaw + shaderFileSize);
#endif

    SPDLOG_DEBUG("Kompute OpMult Initialising algorithm component");

    this->mAlgorithm->init(shaderFileData, tensors);
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
    this->mTensorLHS->recordBufferMemoryBarrier(
      vk::AccessFlagBits::eShaderWrite,
      vk::AccessFlagBits::eTransferRead,
      vk::PipelineStageFlagBits::eComputeShader,
      vk::PipelineStageFlagBits::eTransfer);
    this->mTensorOutput->recordBufferMemoryBarrier(
      vk::AccessFlagBits::eShaderWrite,
      vk::AccessFlagBits::eTransferRead,
      vk::PipelineStageFlagBits::eComputeShader,
      vk::PipelineStageFlagBits::eTransfer);

    this->mTensorOutputStaging->recordCopyFrom(this->mTensorOutput);

    // Buffer to ensure wait until data is copied to staging buffer
    this->mTensorLHS->recordBufferMemoryBarrier(
      vk::AccessFlagBits::eTransferWrite,
      vk::AccessFlagBits::eHostRead,
      vk::PipelineStageFlagBits::eTransfer,
      vk::PipelineStageFlagBits::eHost);
    this->mTensorOutput->recordBufferMemoryBarrier(
      vk::AccessFlagBits::eTransferWrite,
      vk::AccessFlagBits::eHostRead,
      vk::PipelineStageFlagBits::eTransfer,
      vk::PipelineStageFlagBits::eHost);
}

template<uint32_t tX, uint32_t tY, uint32_t tZ>
void
OpMult<tX, tY, tZ>::postSubmit()
{
    SPDLOG_DEBUG("Kompute OpMult postSubmit called");

    this->mTensorOutputStaging->mapDataFromHostMemory();

    this->mTensorOutput->setData(this->mTensorOutputStaging->data());
}

}

#endif // #ifndef OPMULT_CPP
