#pragma once

#include <fstream>

#include "kompute/Core.hpp"

#include "kompute/Algorithm.hpp"
#include "kompute/Tensor.hpp"

#include "kompute/operations/OpAlgoBase.hpp"

namespace kp {

/**
 * Operation base class to simplify the creation of operations that require
 * right hand and left hand side datapoints together with a single output.
 * The expected data passed is two input tensors and one output tensor.
 * The template parameters specify the processing GPU layout number of
 * iterations for each x, y, z parameter. More specifically, this will be the
 * input to ".dispatch(uint32_t tX, uint32_t tY, uint32_t, tZ)"
 */
template<uint32_t tX = 0, uint32_t tY = 0, uint32_t tZ = 0>
class OpAlgoLhsRhsOut : public OpAlgoBase<tX, tY, tZ>
{
  public:
    /**
     *  Base constructor, should not be used unless explicitly intended.
     */
    OpAlgoLhsRhsOut();

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
    OpAlgoLhsRhsOut(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
           std::shared_ptr<vk::Device> device,
           std::shared_ptr<vk::CommandBuffer> commandBuffer,
           std::vector<std::shared_ptr<Tensor>> tensors);

    /**
     * Default destructor, which is in charge of destroying the algorithm
     * components but does not destroy the underlying tensors
     */
    virtual ~OpAlgoLhsRhsOut() override;

    /**
     * The init function is responsible for ensuring that all of the tensors
     * provided are aligned with requirements such as LHS, RHS and Output
     * tensors, and  creates the algorithm component which processes the
     * computation.
     */
    virtual void init() override;

    /**
     * This records the commands that are to be sent to the GPU. This includes
     * the barriers that ensure the memory has been copied before going in and
     * out of the shader, as well as the dispatch operation that sends the
     * shader processing to the gpu. This function also records the GPU memory
     * copy of the output data for the staging bufffer so it can be read by the
     * host.
     */
    virtual void record() override;

    /**
     * Executes after the recorded commands are submitted, and performs a copy
     * of the GPU Device memory into the staging buffer so the output data can
     * be retrieved.
     */
    virtual void postSubmit() override;

  protected:
    // -------------- NEVER OWNED RESOURCES
    std::shared_ptr<Tensor> mTensorLHS; ///< Reference to the parameter used in the left hand side equation of the shader
    std::shared_ptr<Tensor> mTensorRHS; ///< Reference to the parameter used in the right hand side equation of the shader
    std::shared_ptr<Tensor> mTensorOutput; ///< Reference to the parameter used in the output of the shader and will be copied with a staging vector

    // -------------- ALWAYS OWNED RESOURCES
    std::shared_ptr<Tensor> mTensorOutputStaging; ///< Staging temporary tensor user do to copy the output of the tensor
};

} // End namespace kp

// Including implemenation for template class
#ifndef OPALGOLHSRHSOUT_CPP
#define OPALGOLHSRHSOUT_CPP

namespace kp {

template<uint32_t tX, uint32_t tY, uint32_t tZ>
OpAlgoLhsRhsOut<tX, tY, tZ>::OpAlgoLhsRhsOut()
{
    SPDLOG_DEBUG("Kompute OpAlgoLhsRhsOut constructor base");
}

template<uint32_t tX, uint32_t tY, uint32_t tZ>
OpAlgoLhsRhsOut<tX, tY, tZ>::OpAlgoLhsRhsOut(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
                           std::shared_ptr<vk::Device> device,
                           std::shared_ptr<vk::CommandBuffer> commandBuffer,
                           std::vector<std::shared_ptr<Tensor>> tensors)
  // The inheritance is initialised with the copyOutputData to false given that
  // this depencendant class handles the transfer of data via staging buffers in 
  // a granular way.
  : OpAlgoBase<tX, tY, tZ>(physicalDevice, device, commandBuffer, tensors)
{
    SPDLOG_DEBUG("Kompute OpAlgoLhsRhsOut constructor with params");
}

template<uint32_t tX, uint32_t tY, uint32_t tZ>
OpAlgoLhsRhsOut<tX, tY, tZ>::~OpAlgoLhsRhsOut()
{
    SPDLOG_DEBUG("Kompute OpAlgoLhsRhsOut destructor started");
}

template<uint32_t tX, uint32_t tY, uint32_t tZ>
void
OpAlgoLhsRhsOut<tX, tY, tZ>::init()
{
    SPDLOG_DEBUG("Kompute OpAlgoLhsRhsOut init called");

    if (this->mTensors.size() < 3) {
        throw std::runtime_error(
          "Kompute OpAlgoLhsRhsOut called with less than 1 tensor");
    } else if (this->mTensors.size() > 3) {
        SPDLOG_WARN("Kompute OpAlgoLhsRhsOut called with more than 3 this->mTensors");
    }

    this->mTensorLHS = this->mTensors[0];
    this->mTensorRHS = this->mTensors[1];
    this->mTensorOutput = this->mTensors[2];


    if (!(this->mTensorLHS->isInit() && this->mTensorRHS->isInit() &&
          this->mTensorOutput->isInit())) {
        throw std::runtime_error(
          "Kompute OpAlgoLhsRhsOut all tensor parameters must be initialised. LHS: " +
          std::to_string(this->mTensorLHS->isInit()) +
          " RHS: " + std::to_string(this->mTensorRHS->isInit()) +
          " Output: " + std::to_string(this->mTensorOutput->isInit()));
    }

    if (!(this->mTensorLHS->size() == this->mTensorRHS->size() &&
          this->mTensorRHS->size() == this->mTensorOutput->size())) {
        throw std::runtime_error(
          "Kompute OpAlgoLhsRhsOut all tensor parameters must be the same size LHS: " +
          std::to_string(this->mTensorLHS->size()) +
          " RHS: " + std::to_string(this->mTensorRHS->size()) +
          " Output: " + std::to_string(this->mTensorOutput->size()));
    }

    this->mTensorOutputStaging = std::make_shared<Tensor>(
      this->mTensorOutput->data(), Tensor::TensorTypes::eStaging);

    this->mTensorOutputStaging->init(
      this->mPhysicalDevice, this->mDevice, this->mCommandBuffer);

    SPDLOG_DEBUG("Kompute OpAlgoLhsRhsOut fetching spirv data");

    std::vector<char> shaderFileData = this->fetchSpirvBinaryData();

    SPDLOG_DEBUG("Kompute OpAlgoLhsRhsOut Initialising algorithm component");

    this->mAlgorithm->init(shaderFileData, this->mTensors);
}

template<uint32_t tX, uint32_t tY, uint32_t tZ>
void
OpAlgoLhsRhsOut<tX, tY, tZ>::record()
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

    this->mAlgorithm->recordDispatch(this->mX, this->mY, this->mZ);

    // Barrier to ensure the shader code is executed before buffer read
    this->mTensorOutput->recordBufferMemoryBarrier(
      this->mCommandBuffer,
      vk::AccessFlagBits::eShaderWrite,
      vk::AccessFlagBits::eTransferRead,
      vk::PipelineStageFlagBits::eComputeShader,
      vk::PipelineStageFlagBits::eTransfer);

    this->mTensorOutputStaging->recordCopyFrom(
            this->mCommandBuffer,
            this->mTensorOutput,
            true);
}

template<uint32_t tX, uint32_t tY, uint32_t tZ>
void
OpAlgoLhsRhsOut<tX, tY, tZ>::postSubmit()
{
    SPDLOG_DEBUG("Kompute OpAlgoLhsRhsOut postSubmit called");

    this->mTensorOutputStaging->mapDataFromHostMemory();

    this->mTensorOutput->setData(this->mTensorOutputStaging->data());
}

}

#endif // #ifndef OPALGOLHSRHSOUT_CPP

