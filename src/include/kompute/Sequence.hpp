#pragma once

#include "kompute/Core.hpp"

#include "kompute/operations/OpBase.hpp"

namespace kp {

/**
 *  Container of operations that can be sent to GPU as batch
 */
class Sequence
{
  public:
    /**
     *  Base constructor for Sequence. Should not be used unless explicit
     * intended.
     */
    Sequence();
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
     * Initialises sequence including the creation of the command pool and the
     * command buffer.
     */
    void init();

    /**
     * Begins recording commands for commands to be submitted into the command
     * buffer.
     *
     * @return Boolean stating whether execution was successful.
     */
    bool begin();

    /**
     * Ends the recording and stops recording commands when the record command
     * is sent.
     *
     * @return Boolean stating whether execution was successful.
     */
    bool end();

    /**
     * Eval sends all the recorded and stored operations in the vector of
     * operations into the gpu as a submit job with a barrier.
     *
     * @return Boolean stating whether execution was successful.
     */
    bool eval();

    /**
     * Eval Async sends all the recorded and stored operations in the vector of operations into the gpu as a submit job with a barrier. EvalAwait() must be called after to ensure the sequence is terminated correctly.
     *
     * @return Boolean stating whether execution was successful.
     */
    bool evalAsync();

    /**
     * Eval Await waits for the fence to finish processing and then once it finishes, it runs the postEval of all operations.
     *
     * @param waitFor Number of milliseconds to wait before timing out.
     * @return Boolean stating whether execution was successful.
     */
    bool evalAwait(uint64_t waitFor = UINT64_MAX);

    /**
     * Returns true if the sequence is currently in recording activated.
     *
     * @return Boolean stating if recording ongoing.
     */
    bool isRecording();

    /**
     * Returns true if the sequence has been successfully initialised.
     *
     * @return Boolean stating if sequence has been initialised.
     */
    bool isInit();

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
    template<typename T, typename... TArgs>
    bool record(std::vector<std::shared_ptr<Tensor>> tensors, TArgs&&... params)
    {
        static_assert(std::is_base_of<OpBase, T>::value,
                      "Kompute Sequence record(...) template only valid with "
                      "OpBase derived classes");

        SPDLOG_DEBUG("Kompute Sequence record function started");

        if (!this->isRecording()) {
            SPDLOG_ERROR(
              "Kompute sequence record attempted when not record BEGIN");
            return false;
        }

        SPDLOG_DEBUG("Kompute Sequence creating OpBase derived class instance");
        T* op = new T(this->mPhysicalDevice,
                      this->mDevice,
                      this->mCommandBuffer,
                      tensors,
                      std::forward<TArgs>(params)...);

        OpBase* baseOp = dynamic_cast<OpBase*>(op);

        std::unique_ptr<OpBase> baseOpPtr{ baseOp };

        SPDLOG_DEBUG(
          "Kompute Sequence running init on OpBase derived class instance");
        baseOpPtr->init();

        SPDLOG_DEBUG(
          "Kompute Sequence running record on OpBase derived class instance");
        baseOpPtr->record();

        mOperations.push_back(std::move(baseOpPtr));

        return true;
    }

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
    std::vector<std::unique_ptr<OpBase>> mOperations;

    // State
    bool mIsInit = false;
    bool mRecording = false;
    bool mEvalBusy = false;

    // Create functions
    void createCommandPool();
    void createCommandBuffer();
};

} // End namespace kp
