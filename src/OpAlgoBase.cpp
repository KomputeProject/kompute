#pragma once

#include "kompute/operations/OpAlgoBase.hpp"

namespace kp {

OpAlgoBase::OpAlgoBase()
{
    SPDLOG_DEBUG("Kompute OpAlgoBase constructor base");
}

OpAlgoBase::OpAlgoBase(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
                       std::shared_ptr<vk::Device> device,
                       std::shared_ptr<vk::CommandBuffer> commandBuffer,
                       std::vector<std::shared_ptr<Tensor>>& tensors,
                       KomputeWorkgroup komputeWorkgroup,
                       const std::vector<float>& specializationConstants)
  : OpBase(physicalDevice, device, commandBuffer, tensors)
{
    SPDLOG_DEBUG("Kompute OpAlgoBase constructor with params numTensors: {}",
                 tensors.size());

    // The dispatch size is set up based on either explicitly provided template
    // parameters or by default it would take the shape and size of the tensors
    if (komputeWorkgroup.x > 0) {
        // If at least the x value is provided we use mainly the parameters
        // provided
        this->mKomputeWorkgroup = {
            komputeWorkgroup.x,
            komputeWorkgroup.y > 0 ? komputeWorkgroup.y : 1,
            komputeWorkgroup.z > 0 ? komputeWorkgroup.z : 1
        };
    } else {
        this->mKomputeWorkgroup = { tensors[0]->size(), 1, 1 };
    }
    SPDLOG_INFO("Kompute OpAlgoBase dispatch size X: {}, Y: {}, Z: {}",
                this->mKomputeWorkgroup.x,
                this->mKomputeWorkgroup.y,
                this->mKomputeWorkgroup.z);

    this->mAlgorithm = std::make_shared<Algorithm>(device, commandBuffer, specializationConstants);
}

OpAlgoBase::OpAlgoBase(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
                       std::shared_ptr<vk::Device> device,
                       std::shared_ptr<vk::CommandBuffer> commandBuffer,
                       std::vector<std::shared_ptr<Tensor>>& tensors,
                       std::string shaderFilePath,
                       KomputeWorkgroup komputeWorkgroup,
                       const std::vector<float>& specializationConstants)
  : OpAlgoBase(physicalDevice, device, commandBuffer, tensors, komputeWorkgroup, specializationConstants)
{
    SPDLOG_DEBUG(
      "Kompute OpAlgoBase shaderFilePath constructo with shaderfile path: {}",
      shaderFilePath);

    this->mShaderFilePath = shaderFilePath;
}

OpAlgoBase::OpAlgoBase(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
                       std::shared_ptr<vk::Device> device,
                       std::shared_ptr<vk::CommandBuffer> commandBuffer,
                       std::vector<std::shared_ptr<Tensor>>& tensors,
                       const std::vector<char>& shaderDataRaw,
                       KomputeWorkgroup komputeWorkgroup,
                       const std::vector<float>& specializationConstants)
  : OpAlgoBase(physicalDevice, device, commandBuffer, tensors, komputeWorkgroup, specializationConstants)
{
    SPDLOG_DEBUG("Kompute OpAlgoBase shaderFilePath constructo with shader raw "
                 "data length: {}",
                 shaderDataRaw.size());

    this->mShaderDataRaw = shaderDataRaw;
}

OpAlgoBase::~OpAlgoBase()
{
    SPDLOG_DEBUG("Kompute OpAlgoBase destructor started");
}

void
OpAlgoBase::init()
{
    SPDLOG_DEBUG("Kompute OpAlgoBase init called");

    if (this->mTensors.size() < 1) {
        throw std::runtime_error(
          "Kompute OpAlgoBase called with less than 1 tensor");
    }

    for (std::shared_ptr<Tensor> tensor : this->mTensors) {
        if (!tensor->isInit()) {
            throw std::runtime_error(
              "Kompute OpAlgoBase validation failed; all tensor parameters "
              "must be initialised.");
        }
    }

    SPDLOG_DEBUG("Kompute OpAlgoBase fetching spirv data");

    std::vector<char> shaderFileData = this->fetchSpirvBinaryData();

    SPDLOG_DEBUG("Kompute OpAlgoBase Initialising algorithm component");

    this->mAlgorithm->init(shaderFileData, this->mTensors);
}

void
OpAlgoBase::record()
{
    SPDLOG_DEBUG("Kompute OpAlgoBase record called");

    // Barrier to ensure the data is finished writing to buffer memory
    for (std::shared_ptr<Tensor> tensor : this->mTensors) {
        tensor->recordBufferMemoryBarrier(
          this->mCommandBuffer,
          vk::AccessFlagBits::eHostWrite,
          vk::AccessFlagBits::eShaderRead,
          vk::PipelineStageFlagBits::eHost,
          vk::PipelineStageFlagBits::eComputeShader);
    }

    this->mAlgorithm->recordDispatch(this->mKomputeWorkgroup.x,
                                     this->mKomputeWorkgroup.y,
                                     this->mKomputeWorkgroup.z);
}

void
OpAlgoBase::preEval()
{
    SPDLOG_DEBUG("Kompute OpAlgoBase preEval called");
}

void
OpAlgoBase::postEval()
{
    SPDLOG_DEBUG("Kompute OpAlgoBase postSubmit called");
}

std::vector<char>
OpAlgoBase::fetchSpirvBinaryData()
{
    SPDLOG_DEBUG("Kompute OpAlgoBase Running fetchSpirvBinaryData");

    if (this->mShaderFilePath.size()) {
        SPDLOG_DEBUG("Kompute OpAlgoBase Reading data from file path");

        std::ifstream fileStream(this->mShaderFilePath,
                                 std::ios::binary | std::ios::in |
                                   std::ios::ate);

        if (!fileStream.good()) {
            throw std::runtime_error("Error reading file: " +
                                     this->mShaderFilePath);
        }

        size_t shaderFileSize = fileStream.tellg();
        fileStream.seekg(0, std::ios::beg);
        char* shaderDataRaw = new char[shaderFileSize];
        fileStream.read(shaderDataRaw, shaderFileSize);
        fileStream.close();

        SPDLOG_WARN("Kompute OpAlgoBase fetched {} bytes", shaderFileSize);

        return std::vector<char>(shaderDataRaw, shaderDataRaw + shaderFileSize);
    } else if (this->mShaderDataRaw.size()) {
        SPDLOG_DEBUG("Kompute OpAlgoBase Reading data from data provided");
        return this->mShaderDataRaw;
    } else {
        throw std::runtime_error(
          "Kompute OpAlgoBase Error reached fetchSpirvBinaryData but neither "
          "filepath nor data provided");
    }
}

}
