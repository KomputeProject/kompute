#pragma once

#include "kompute/Core.hpp"

#include "kompute/operations/OpBase.hpp"

namespace kp {

/**
 *  Container of operations that can be sent to GPU as batch
 */
class Sequence: public std::enable_shared_from_this<Sequence>
{
  public:
    /**
     * Main constructor for sequence which requires core vulkan components to
     * generate all dependent resources.
     *
     * @param physicalDevice Vulkan physical device
     * @param device Vulkan logical device
     * @param computeQueue Vulkan compute queue
     * @param queueIndex Vulkan compute queue index in device
     */
    Sequence(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
             std::shared_ptr<vk::Device> device,
             std::shared_ptr<vk::Queue> computeQueue,
             uint32_t queueIndex);
    /**
     * Destructor for sequence which is responsible for cleaning all subsequent
     * owned operations.
     */
    ~Sequence();

    /**
     * Record function for operation to be added to the GPU queue in batch. This
     * template requires classes to be derived from the OpBase class. This
     * function also requires the Sequence to be recording, otherwise it will
     * not be able to add the operation.
     *
     * @param tensors Vector of tensors to use for the operation
     * @param TArgs Template parameters that are used to initialise operation
     * which allows for extensible configurations on initialisation.
     */
    std::shared_ptr<Sequence> record(std::shared_ptr<OpBase> op);

    /**
     * Clear function clears all operations currently recorded and starts recording again.
     */
    void clear();

    /**
     * Begins recording commands for commands to be submitted into the command
     * buffer.
     *
     * @return Boolean stating whether execution was successful.
     */
    void begin();

    /**
     * Ends the recording and stops recording commands when the record command
     * is sent.
     *
     * @return Boolean stating whether execution was successful.
     */
    void end();

    /**
     * Eval sends all the recorded and stored operations in the vector of
     * operations into the gpu as a submit job with a barrier.
     *
     * @return Boolean stating whether execution was successful.
     */
    std::shared_ptr<Sequence> eval();

    /**
     * Eval Async sends all the recorded and stored operations in the vector of
     * operations into the gpu as a submit job with a barrier. EvalAwait() must
     * be called after to ensure the sequence is terminated correctly.
     *
     * @return Boolean stating whether execution was successful.
     */
    std::shared_ptr<Sequence> evalAsync();

    /**
     * Eval Await waits for the fence to finish processing and then once it
     * finishes, it runs the postEval of all operations.
     *
     * @param waitFor Number of milliseconds to wait before timing out.
     * @return Boolean stating whether execution was successful.
     */
    std::shared_ptr<Sequence> evalAwait(uint64_t waitFor = UINT64_MAX);

    /**
     * Returns true if the sequence is currently in recording activated.
     *
     * @return Boolean stating if recording ongoing.
     */
    bool isRecording();

    /**
     * Returns true if the sequence is currently running - mostly used for async
     * workloads.
     *
     * @return Boolean stating if currently running.
     */
    bool isRunning();

    /**
     * Destroys and frees the GPU resources which include the buffer and memory
     * and sets the sequence as init=False.
     */
    void freeMemoryDestroyGPUResources();

  private:
    // -------------- NEVER OWNED RESOURCES
    std::shared_ptr<vk::PhysicalDevice> mPhysicalDevice = nullptr;
    std::shared_ptr<vk::Device> mDevice = nullptr;
    std::shared_ptr<vk::Queue> mComputeQueue = nullptr;
    uint32_t mQueueIndex = -1;

    // -------------- OPTIONALLY OWNED RESOURCES
    std::shared_ptr<vk::CommandPool> mCommandPool = nullptr;
    bool mFreeCommandPool = false;
    std::shared_ptr<vk::CommandBuffer> mCommandBuffer = nullptr;
    bool mFreeCommandBuffer = false;

    // -------------- ALWAYS OWNED RESOURCES
    vk::Fence mFence;
    std::vector<std::shared_ptr<OpBase>> mOperations;

    // State
    bool mRecording = false;
    bool mIsRunning = false;

    // Create functions
    void createCommandPool();
    void createCommandBuffer();
};

} // End namespace kp
