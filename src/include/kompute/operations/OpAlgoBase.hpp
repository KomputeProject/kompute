#pragma once

#include <fstream>

#include "kompute/Core.hpp"

#include "kompute/shaders/shaderopmult.hpp"

#include "kompute/Algorithm.hpp"
#include "kompute/Tensor.hpp"

#include "kompute/operations/OpBase.hpp"

namespace kp {

/**
 * Operation that provides a general abstraction that simplifies the use of 
 * algorithm and parameter components which can be used with shaders.
 * The template parameters specify the processing GPU layout number of
 * iterations for each x, y, z parameter. More specifically, this will be the
 * input to ".dispatch(uint32_t tX, uint32_t tY, uint32_t, tZ)"
 */
template<uint32_t tX = 0, uint32_t tY = 0, uint32_t tZ = 0>
class OpAlgoBase : public OpBase
{
  public:
    /**
     *  Base constructor, should not be used unless explicitly intended.
     */
    OpAlgoBase();

    /**
     * Default constructor with parameters that provides the bare minimum
     * requirements for the operations to be able to create and manage their
     * sub-components.
     *
     * @param physicalDevice Vulkan physical device used to find device queues
     * @param device Vulkan logical device for passing to Algorithm
     * @param commandBuffer Vulkan Command Buffer to record commands into
     * @param tensors Tensors that are to be used in this operation
     */
    OpAlgoBase(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
           std::shared_ptr<vk::Device> device,
           std::shared_ptr<vk::CommandBuffer> commandBuffer,
           std::vector<std::shared_ptr<Tensor>>& tensors);

    /**
     * Default destructor, which is in charge of destroying the algorithm
     * components but does not destroy the underlying tensors
     */
    ~OpAlgoBase();

    /**
     * The init function is responsible for the initialisation of the algorithm
     * component based on the parameters specified, and allows for extensibility
     * on the options provided. Further dependent classes can perform more 
     * specific checks such as ensuring tensors provided are initialised, etc.
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

    // -------------- OPTIONALLY OWNED RESOURCES
    std::shared_ptr<Algorithm> mAlgorithm;
    bool mFreeAlgorithm = false;

    // -------------- ALWAYS OWNED RESOURCES
    uint32_t mX;
    uint32_t mY;
    uint32_t mZ;

    std::string mOptSpirvBinPath; ///< Optional member variable which can be provided for the OpAlgoBase to find the data automatically and load for processing

    virtual std::vector<char> fetchSpirvBinaryData();
};

} // End namespace kp

// Including implemenation for template class
#ifndef OPALGOBASE_IMPL
#define OPALGOBASE_IMPL

namespace kp {

template<uint32_t tX, uint32_t tY, uint32_t tZ>
OpAlgoBase<tX, tY, tZ>::OpAlgoBase()
{
    SPDLOG_DEBUG("Kompute OpAlgoBase constructor base");
}

template<uint32_t tX, uint32_t tY, uint32_t tZ>
OpAlgoBase<tX, tY, tZ>::OpAlgoBase(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
                           std::shared_ptr<vk::Device> device,
                           std::shared_ptr<vk::CommandBuffer> commandBuffer,
                           std::vector<std::shared_ptr<Tensor>>& tensors)
  : OpBase(physicalDevice, device, commandBuffer, tensors, false)
{
    SPDLOG_DEBUG("Kompute OpAlgoBase constructor with params");

    // The dispatch size is set up based on either explicitly provided template
    // parameters or by default it would take the shape and size of the tensors
    if (tX > 0) {
        // If at least the x value is provided we use mainly the parameters
        // provided
        this->mX = tX;
        this->mY = tY > 0 ? tY : 1;
        this->mZ = tZ > 0 ? tZ : 1;
    } else {
        // TODO: If tensor empty vector exception would be thrown
        // TODO: Fully support the full size dispatch using size for the shape
        this->mX = tensors[0]->size();
        this->mY = 1;
        this->mZ = 1;
    }
    spdlog::info("Kompute OpAlgoBase dispatch size X: {}, Y: {}, Z: {}",
                 this->mX,
                 this->mY,
                 this->mZ);

    this->mAlgorithm = std::make_shared<Algorithm>(device, commandBuffer);
}

template<uint32_t tX, uint32_t tY, uint32_t tZ>
OpAlgoBase<tX, tY, tZ>::~OpAlgoBase()
{
    SPDLOG_DEBUG("Kompute OpAlgoBase destructor started");
}

template<uint32_t tX, uint32_t tY, uint32_t tZ>
std::vector<char> OpAlgoBase<tX, tY, tZ>::fetchSpirvBinaryData() 
{
    SPDLOG_WARN(
      "Kompute OpAlgoBase Running shaders directly from spirv file");

    std::ifstream fileStream(this->mOptSpirvBinPath,
                             std::ios::binary | std::ios::in | std::ios::ate);

    size_t shaderFileSize = fileStream.tellg();
    fileStream.seekg(0, std::ios::beg);
    char* shaderDataRaw = new char[shaderFileSize];
    fileStream.read(shaderDataRaw, shaderFileSize);
    fileStream.close();

    return std::vector<char>(shaderDataRaw,
                             shaderDataRaw + shaderFileSize);
}

template<uint32_t tX, uint32_t tY, uint32_t tZ>
void
OpAlgoBase<tX, tY, tZ>::init()
{
    SPDLOG_DEBUG("Kompute OpAlgoBase init called");

    std::vector<char> shaderFileData = this->fetchSpirvBinaryData();

    this->mAlgorithm->init(shaderFileData, this->mTensors);
}

template<uint32_t tX, uint32_t tY, uint32_t tZ>
void
OpAlgoBase<tX, tY, tZ>::record()
{
    SPDLOG_DEBUG("Kompute OpAlgoBase record called");

    this->mAlgorithm->recordDispatch(this->mX, this->mY, this->mZ);
}

template<uint32_t tX, uint32_t tY, uint32_t tZ>
void
OpAlgoBase<tX, tY, tZ>::postSubmit()
{
    SPDLOG_DEBUG("Kompute OpAlgoBase postSubmit called");
}

}

#endif // #ifndef OPALGOBASE_IMPL

