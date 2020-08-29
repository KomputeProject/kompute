#pragma once

#include <fstream>

#include "kompute/Core.hpp"

#if RELEASE
#include "kompute/shaders/shaderopmult.hpp"
#endif

#include "kompute/Algorithm.hpp"
#include "kompute/Tensor.hpp"

#include "kompute/operations/OpAlgoBase.hpp"

namespace kp {

/**
 * Operation that performs multiplication on two tensors and outpus on third
 * tensor. The template parameters specify the processing GPU layout number of
 * iterations for each x, y, z parameter. More specifically, this will be the
 * input to ".dispatch(uint32_t tX, uint32_t tY, uint32_t, tZ)"
 */
template<uint32_t tX = 0, uint32_t tY = 0, uint32_t tZ = 0>
class OpMult : public OpAlgoBase<tX, tY, tZ>
{
  public:
    /**
     *  Base constructor, should not be used unless explicitly intended.
     */
    OpMult();

    /**
     * Default constructor with parameters that provides the bare minimum
     * requirements for the operations to be able to create and manage their
     * sub-components.
     *
     * @param physicalDevice Vulkan physical device used to find device queues
     * @param device Vulkan logical device for passing to Algorithm
     * @param commandBuffer Vulkan Command Buffer to record commands into
     * @param tensors Tensors that are to be used in this operation
     * @param freeTensors Whether operation manages the memory of the Tensors
     */
    OpMult(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
           std::shared_ptr<vk::Device> device,
           std::shared_ptr<vk::CommandBuffer> commandBuffer,
           std::vector<std::shared_ptr<Tensor>>& tensors);

    /**
     * Default destructor, which is in charge of destroying the algorithm
     * components but does not destroy the underlying tensors
     */
    ~OpMult();

    /**
     * The init function is responsible for ensuring that all of the tensors
     * provided are aligned with requirements such as LHS, RHS and Output
     * tensors, and  creates the algorithm component which processes the
     * computation.
     */
    void init() override;

    /**
     * This records the commands that are to be sent to the GPU. This includes
     * the barriers that ensure the memory has been copied before going in and
     * out of the shader, as well as the dispatch operation that sends the
     * shader processing to the gpu. This function also records the GPU memory
     * copy of the output data for the staging bufffer so it can be read by the
     * host.
     */
    void record() override;

    /**
     * Executes after the recorded commands are submitted, and performs a copy
     * of the GPU Device memory into the staging buffer so the output data can
     * be retrieved.
     */
    void postSubmit() override;

  private:
    // -------------- NEVER OWNED RESOURCES
    std::shared_ptr<Tensor> mTensorLHS;
    std::shared_ptr<Tensor> mTensorRHS;
    std::shared_ptr<Tensor> mTensorOutput;

    // -------------- ALWAYS OWNED RESOURCES
    std::shared_ptr<Tensor> mTensorOutputStaging;
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

template<uint32_t tX, uint32_t tY, uint32_t tZ>
OpMult<tX, tY, tZ>::OpMult(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
                           std::shared_ptr<vk::Device> device,
                           std::shared_ptr<vk::CommandBuffer> commandBuffer,
                           std::vector<std::shared_ptr<Tensor>>& tensors)
  : OpAlgoBase<tX, tY, tZ>(physicalDevice, device, commandBuffer, tensors)
{
    SPDLOG_DEBUG("Kompute OpMult constructor with params");
}

template<uint32_t tX, uint32_t tY, uint32_t tZ>
OpMult<tX, tY, tZ>::~OpMult()
{
    SPDLOG_DEBUG("Kompute OpMult destructor started");
}

template<uint32_t tX, uint32_t tY, uint32_t tZ>
void
OpMult<tX, tY, tZ>::init()
{
    SPDLOG_DEBUG("Kompute OpMult init called");

    if (this->mTensors.size() < 3) {
        throw std::runtime_error(
          "Kompute OpMult called with less than 1 tensor");
    } else if (this->mTensors.size() > 3) {
        spdlog::warn("Kompute OpMult called with more than 3 this->mTensors");
    }

    this->mTensorLHS = this->mTensors[0];
    this->mTensorRHS = this->mTensors[1];
    this->mTensorOutput = this->mTensors[2];


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
    this->mOptSpirvBinPath = "shaders/glsl/opmult.comp.spv";
    std::vector<char>& shaderFileData = this->fetchSpirvBinaryData();
#endif

    SPDLOG_DEBUG("Kompute OpMult Initialising algorithm component");

    this->mAlgorithm->init(shaderFileData, this->mTensors);
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

    this->mTensorOutputStaging->recordCopyFrom(this->mTensorOutput);
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
