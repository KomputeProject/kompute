#pragma once

#include <set>
#include <unordered_map>

#include "kompute/Core.hpp"

#include "kompute/Sequence.hpp"

#include "kompute/operations/OpTensorSyncDevice.hpp"

#define KP_DEFAULT_SESSION "DEFAULT"

namespace kp {

/**
    Base orchestrator which creates and manages device and child components
*/
class Manager
{
  public:
    /**
        Base constructor and default used which creates the base resources
       including choosing the device 0 by default.
    */
    Manager();

    /**
     * Similar to base constructor but allows the user to provide the device
     * they would like to create the resources on.
     *
     * @param physicalDeviceIndex The index of the physical device to use
     * @param familyQueueIndices (Optional) List of queue indices to add for
     * explicit allocation
     * @param totalQueues The total number of compute queues to create.
     */
    Manager(uint32_t physicalDeviceIndex,
            const std::vector<uint32_t>& familyQueueIndices = {});

    /**
     * Manager constructor which allows your own vulkan application to integrate
     * with the vulkan kompute use.
     *
     * @param instance Vulkan compute instance to base this application
     * @param physicalDevice Vulkan physical device to use for application
     * @param device Vulkan logical device to use for all base resources
     * @param physicalDeviceIndex Index for vulkan physical device used
     */
    Manager(std::shared_ptr<vk::Instance> instance,
            std::shared_ptr<vk::PhysicalDevice> physicalDevice,
            std::shared_ptr<vk::Device> device,
            uint32_t physicalDeviceIndex);

    /**
     * Manager destructor which would ensure all owned resources are destroyed
     * unless explicitly stated that resources should not be destroyed or freed.
     */
    ~Manager();

    /**
     * Get or create a managed Sequence that will be contained by this manager.
     * If the named sequence does not currently exist, it would be created and
     * initialised.
     *
     * @param sequenceName The name for the named sequence to be retrieved or
     * created
     * @param queueIndex The queue to use from the available queues
     * @return Shared pointer to the manager owned sequence resource
     */
    std::shared_ptr<Sequence> sequence(uint32_t queueIndex = 0);

    /**
     * Function that evaluates operation against named sequence.
     *
     * @param tensors The tensors to be used in the operation recorded
     * @param sequenceName The name of the sequence to be retrieved or created
     * @param TArgs Template parameters that will be used to initialise
     * Operation to allow for extensible configurations on initialisation
     */
    template<typename T, typename... TArgs>
    void evalOp(std::vector<std::shared_ptr<Tensor>> tensors,
                std::string sequenceName,
                TArgs&&... params)
    {
        KP_LOG_DEBUG("Kompute Manager evalOp triggered");
        std::shared_ptr<kp::Sequence> sq =
          this->sequence(sequenceName);

        KP_LOG_DEBUG("Kompute Manager evalOp running sequence BEGIN");
        sq->begin();

        KP_LOG_DEBUG("Kompute Manager evalOp running sequence RECORD");
        sq->record<T>(tensors, std::forward<TArgs>(params)...);

        KP_LOG_DEBUG("Kompute Manager evalOp running sequence END");
        sq->end();

        KP_LOG_DEBUG("Kompute Manager evalOp running sequence EVAL");
        sq->eval();

        KP_LOG_DEBUG("Kompute Manager evalOp running sequence SUCCESS");
    }

    /**
     * Function that evaluates operation against a newly created sequence.
     *
     * @param tensors The tensors to be used in the operation recorded
     * @param TArgs Template parameters that will be used to initialise
     * Operation to allow for extensible configurations on initialisation
     */
    template<typename T, typename... TArgs>
    void evalOpDefault(std::vector<std::shared_ptr<Tensor>> tensors,
                       TArgs&&... params)
    {
        KP_LOG_DEBUG("Kompute Manager evalOp Default triggered");
        this->mCurrentSequenceIndex++;
        this->evalOp<T>(
          tensors, KP_DEFAULT_SESSION, std::forward<TArgs>(params)...);
    }

    /**
     * Function that evaluates operation against named sequence asynchronously.
     *
     * @param tensors The tensors to be used in the operation recorded
     * @param sequenceName The name of the sequence to be retrieved or created
     * @param params Template parameters that will be used to initialise
     * Operation to allow for extensible configurations on initialisation
     */
    template<typename T, typename... TArgs>
    void evalOpAsync(std::vector<std::shared_ptr<Tensor>> tensors,
                     std::string sequenceName,
                     TArgs&&... params)
    {
        KP_LOG_DEBUG("Kompute Manager evalOpAsync triggered");

        std::shared_ptr<kp::Sequence> sq =
          this->sequence(sequenceName);

        KP_LOG_DEBUG("Kompute Manager evalOpAsync running sequence BEGIN");
        sq->begin();

        KP_LOG_DEBUG("Kompute Manager evalOpAsync running sequence RECORD");
        sq->record<T>(tensors, std::forward<TArgs>(params)...);

        KP_LOG_DEBUG("Kompute Manager evalOpAsync running sequence END");
        sq->end();

        KP_LOG_DEBUG("Kompute Manager evalOpAsync running sequence EVAL");
        sq->evalAsync();

        KP_LOG_DEBUG("Kompute Manager evalOpAsync running sequence SUCCESS");
    }

    /**
     * Operation that evaluates operation against default sequence
     * asynchronously.
     *
     * @param tensors The tensors to be used in the operation recorded
     * @param params Template parameters that will be used to initialise
     * Operation to allow for extensible configurations on initialisation
     */
    template<typename T, typename... TArgs>
    void evalOpAsyncDefault(std::vector<std::shared_ptr<Tensor>> tensors,
                            TArgs&&... params)
    {
        KP_LOG_DEBUG("Kompute Manager evalOpAsyncDefault triggered");
        this->mCurrentSequenceIndex++;
        this->evalOpAsync<T>(
          tensors, KP_DEFAULT_SESSION, std::forward<TArgs>(params)...);
    }

    /**
     * Operation that awaits for named sequence to finish.
     *
     * @param sequenceName The name of the sequence to wait for termination
     * @param waitFor The amount of time to wait before timing out
     */
    void evalOpAwait(std::string sequenceName, uint64_t waitFor = UINT64_MAX)
    {
        KP_LOG_DEBUG("Kompute Manager evalOpAwait triggered with sequence {}",
                     sequenceName);
        std::unordered_map<std::string, std::shared_ptr<Sequence>>::iterator
          found = this->mManagedSequences.find(sequenceName);

        if (found != this->mManagedSequences.end()) {
            if (std::shared_ptr<kp::Sequence> sq = found->second) {
                KP_LOG_DEBUG("Kompute Manager evalOpAwait running sequence "
                             "Sequence EVAL AWAIT");
                if (sq->isRunning()) {
                    sq->evalAwait(waitFor);
                }
            }
            KP_LOG_DEBUG(
              "Kompute Manager evalOpAwait running sequence SUCCESS");
        } else {
            KP_LOG_ERROR("Kompute Manager evalOpAwait Sequence not found");
        }
    }

    /**
     * Operation that awaits for default sequence to finish.
     *
     * @param tensors The tensors to be used in the operation recorded
     * @param params Template parameters that will be used to initialise
     * Operation to allow for extensible configurations on initialisation
     */
    void evalOpAwaitDefault(uint64_t waitFor = UINT64_MAX)
    {
        KP_LOG_DEBUG("Kompute Manager evalOpAwaitDefault triggered");
        this->evalOpAwait(KP_DEFAULT_SESSION, waitFor);
    }

    /**
     * Function that simplifies the common workflow of tensor creation and
     * initialization. It will take the constructor parameters for a Tensor
     * and will will us it to create a new Tensor and then create it. The
     * tensor memory will then be managed and owned by the manager.
     *
     * @param data The data to initialize the tensor with
     * @param tensorType The type of tensor to initialize
     * @param syncDataToGPU Whether to sync the data to GPU memory
     * @returns Initialized Tensor with memory Syncd to GPU device
     */
    std::shared_ptr<Tensor> tensor(
      const std::vector<float>& data,
      Tensor::TensorTypes tensorType = Tensor::TensorTypes::eDevice,
      bool syncDataToGPU = true);

    std::shared_ptr<Algorithm> algorithm(
            const std::vector<std::shared_ptr<Tensor>>& tensors = {},
            const std::vector<uint32_t>& spirv = {},
            const Workgroup& workgroup = {},
            const Constants& specializationConstants = {},
            const Constants& pushConstants = {});

    /**
     * Function that simplifies the common workflow of tensor initialisation. It
     * will take the constructor parameters for a Tensor and will will us it to
     * create a new Tensor. The tensor memory will then be managed and owned by
     * the manager.
     *
     * @param tensors Array of tensors to rebuild
     * @param syncDataToGPU Whether to sync the data to GPU memory
     */
    void rebuild(std::vector<std::shared_ptr<kp::Tensor>> tensors,
                        bool syncDataToGPU = true);

    /**
     * Function that simplifies the common workflow of tensor initialisation. It
     * will take the constructor parameters for a Tensor and will will us it to
     * create a new Tensor. The tensor memory will then be managed and owned by
     * the manager.
     *
     * @param tensors Single tensor to rebuild
     * @param syncDataToGPU Whether to sync the data to GPU memory
     */
    void rebuild(std::shared_ptr<kp::Tensor> tensor,
                        bool syncDataToGPU = true);

    /**
     * Destroy owned Vulkan GPU resources and free GPU memory for
     * single tensor.
     *
     * @param tensors Single tensor to rebuild
     */
    void destroy(std::shared_ptr<kp::Tensor> tensor);

    /**
     * Destroy owned Vulkan GPU resources and free GPU memory for
     * vector of tensors.
     *
     * @param tensors Single tensor to rebuild
     */
    void destroy(std::vector<std::shared_ptr<kp::Tensor>> tensors);

    /**
     * Destroy owned Vulkan GPU resources and free GPU memory for
     * vector of sequences. Destroying by sequence name is more efficent
     * and hence recommended instead of by object.
     *
     * @param sequences Vector for shared ptrs with sequences to destroy
     */
    void destroy(std::vector<std::shared_ptr<kp::Sequence>> sequences);

    /**
     * Destroy owned Vulkan GPU resources and free GPU memory for
     * single sequence. Destroying by sequence name is more efficent
     * and hence recommended instead of by object.
     *
     * @param sequences Single sequence to rebuild
     */
    void destroy(std::shared_ptr<kp::Sequence> sequence);

    /**
     * Destroy owned Vulkan GPU resources and free GPU memory for
     * sequence by name.
     *
     * @param sequenceName Single name of named sequence to destroy
     */
    void destroy(const std::string& sequenceName);

    /**
     * Destroy owned Vulkan GPU resources and free GPU memory for
     * sequences using vector of named sequence names.
     *
     * @param sequenceName Vector of sequence names to destroy
     */
    void destroy(const std::vector<std::string>& sequenceNames);

  private:
    // -------------- OPTIONALLY OWNED RESOURCES
    std::shared_ptr<vk::Instance> mInstance = nullptr;
    bool mFreeInstance = false;
    std::shared_ptr<vk::PhysicalDevice> mPhysicalDevice = nullptr;
    uint32_t mPhysicalDeviceIndex = -1;
    std::shared_ptr<vk::Device> mDevice = nullptr;
    bool mFreeDevice = false;

    // -------------- ALWAYS OWNED RESOURCES
    std::set<std::weak_ptr<Tensor>> mManagedTensors;
    std::set<std::weak_ptr<Sequence>> mManagedSequences;
    std::set<std::weak_ptr<Algorithm>> mManagedAlgorithms;
    //std::unique_ptr<Sequence> mDefaultSequence;

    std::vector<uint32_t> mComputeQueueFamilyIndices;
    std::vector<std::shared_ptr<vk::Queue>> mComputeQueues;

    uint32_t mCurrentSequenceIndex = -1;

#if DEBUG
#ifndef KOMPUTE_DISABLE_VK_DEBUG_LAYERS
    vk::DebugReportCallbackEXT mDebugReportCallback;
    vk::DispatchLoaderDynamic mDebugDispatcher;
#endif
#endif

    // Create functions
    void createInstance();
    void createDevice(const std::vector<uint32_t>& familyQueueIndices = {});
};

} // End namespace kp
