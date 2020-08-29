#pragma once

#include <fstream>

#include "kompute/Core.hpp"

#include "kompute/Algorithm.hpp"
#include "kompute/Tensor.hpp"

#include "kompute/operations/OpAlgoBase.hpp"

namespace kp {

/**
 * Operation base class to simplify the creation of operations that require
 * multiple unknown number of tensors, all which will be expected to be
 * Device storage tensors with the data already stored. All the tensors
 * will also be used as outputs so the data will be copied from the device
 * into the respective tensors.
 * The template parameters specify the processing GPU layout number of
 * iterations for each x, y, z parameter. More specifically, this will be the
 * input to ".dispatch(uint32_t tX, uint32_t tY, uint32_t, tZ)"
 */
template<uint32_t tX = 0, uint32_t tY = 0, uint32_t tZ = 0>
class OpAlgoAllInOut : public OpAlgoBase<tX, tY, tZ>
{
  public:
    /**
     *  Base constructor, should not be used unless explicitly intended.
     */
    OpAlgoAllInOut();

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
    OpAlgoAllInOut(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
           std::shared_ptr<vk::Device> device,
           std::shared_ptr<vk::CommandBuffer> commandBuffer,
           std::vector<std::shared_ptr<Tensor>>& tensors);

    /**
     * Default destructor, which is in charge of destroying the algorithm
     * components but does not destroy the underlying tensors
     */
    ~OpAlgoAllInOut();

    /**
     * The init function is responsible for ensuring that all of the tensors
     * passed into the function have been initialised and are of type Device.
     * This is required as the parameters provided are expected to be 
     * used as storage buffers, as well as output buffers, so the data will
     * be transferred out from the Device into the Tensors replacing existing
     * data.
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

  protected:
    // -------------- ALWAYS OWNED RESOURCES
    std::vector<std::shared_ptr<Tensor>> mOutputStagingTensors; ///< Array of output staging tensors which will be expected to be the same size as the number of inputs.
};

} // End namespace kp

// Including implemenation for template class
#ifndef OPALGOALLINOUT_CPP
#define OPALGOALLINOUT_CPP

namespace kp {

template<uint32_t tX, uint32_t tY, uint32_t tZ>
OpAlgoAllInOut<tX, tY, tZ>::OpAlgoAllInOut()
{
    SPDLOG_DEBUG("Kompute OpAlgoAllInOut constructor base");
}

template<uint32_t tX, uint32_t tY, uint32_t tZ>
OpAlgoAllInOut<tX, tY, tZ>::OpAlgoAllInOut(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
                           std::shared_ptr<vk::Device> device,
                           std::shared_ptr<vk::CommandBuffer> commandBuffer,
                           std::vector<std::shared_ptr<Tensor>>& tensors)
  : OpAlgoBase<tX, tY, tZ>(physicalDevice, device, commandBuffer, tensors)
{
    SPDLOG_DEBUG("Kompute OpAlgoAllInOut constructor with params");
}

template<uint32_t tX, uint32_t tY, uint32_t tZ>
OpAlgoAllInOut<tX, tY, tZ>::~OpAlgoAllInOut()
{
    SPDLOG_DEBUG("Kompute OpAlgoAllInOut destructor started");

    SPDLOG_DEBUG("Kompute OpAlgoAllInOut destroying staging tensors");
    for (std::shared_ptr<Tensor> stagingTensor : this->mOutputStagingTensors) {
        stagingTensor->freeMemoryDestroyGPUResources();
    }
}

template<uint32_t tX, uint32_t tY, uint32_t tZ>
void
OpAlgoAllInOut<tX, tY, tZ>::init()
{
    SPDLOG_DEBUG("Kompute OpAlgoAllInOut init called");

    if (this->mTensors.size() < 1) {
        throw std::runtime_error(
          "Kompute OpAlgoAllInOut called with less than 1 tensor");
    } 

    for (std::shared_ptr<Tensor> tensor : this->mTensors) {
        if(!tensor->isInit()) {
            throw std::runtime_error("Kompute OpAlgoAllInOut validation failed; all tensor parameters must be initialised.");
        }
    }

    SPDLOG_DEBUG("Kompute OpAlgoAllInOut creating staging output tensors");

    for (std::shared_ptr<Tensor> tensor : this->mTensors) {
        std::shared_ptr<Tensor> stagingTensor = std::make_shared<Tensor>(
          tensor->data(), Tensor::TensorTypes::eStaging);
        stagingTensor->init(
            this->mPhysicalDevice, this->mDevice, this->mCommandBuffer);
        this->mOutputStagingTensors.push_back(stagingTensor);
    }

    SPDLOG_DEBUG("Kompute OpAlgoAllInOut fetching spirv data");

    std::vector<char>& shaderFileData = this->fetchSpirvBinaryData();

    SPDLOG_DEBUG("Kompute OpAlgoAllInOut Initialising algorithm component");

    this->mAlgorithm->init(shaderFileData, this->mTensors);
}

template<uint32_t tX, uint32_t tY, uint32_t tZ>
void
OpAlgoAllInOut<tX, tY, tZ>::record()
{
    SPDLOG_DEBUG("Kompute OpAlgoAllInOut record called");

    // Barrier to ensure the data is finished writing to buffer memory
    for (std::shared_ptr<Tensor> tensor : this->mTensors) {
        tensor->recordBufferMemoryBarrier(
          vk::AccessFlagBits::eHostWrite,
          vk::AccessFlagBits::eShaderRead,
          vk::PipelineStageFlagBits::eHost,
          vk::PipelineStageFlagBits::eComputeShader);
    }

    this->mAlgorithm->recordDispatch(this->mX, this->mY, this->mZ);

    // Barrier to ensure the shader code is executed before buffer read
    for (std::shared_ptr<Tensor> tensor : this->mTensors) {
        tensor->recordBufferMemoryBarrier(
          vk::AccessFlagBits::eShaderWrite,
          vk::AccessFlagBits::eTransferRead,
          vk::PipelineStageFlagBits::eComputeShader,
          vk::PipelineStageFlagBits::eTransfer);
    }

    // Record copy from and create barrier for STAGING tensors
    for (std::shared_ptr<Tensor> stagingTensor : this->mOutputStagingTensors) {
        stagingTensor->recordCopyFrom(this->mTensorOutput, true);
    }
}

template<uint32_t tX, uint32_t tY, uint32_t tZ>
void
OpAlgoAllInOut<tX, tY, tZ>::postSubmit()
{
    SPDLOG_DEBUG("Kompute OpAlgoAllInOut postSubmit called");

    for (size_t i = 0; i < this->mTensors.size(); i++) {
        this->mOutputStagingTensors[i]->mapDataFromHostMemory();

        this->mTensors[i]->setData(this->mOutputStagingTensors[i]->data());
    }
}

}

#endif // #ifndef OPALGOALLINOUT_CPP


