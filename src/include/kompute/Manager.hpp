#pragma once

#include <set>
#include <unordered_map>

#include "kompute/Core.hpp"

#include "kompute/Sequence.hpp"

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
     * @return Shared pointer to the manager owned sequence resource
     */
    std::shared_ptr<Sequence> getOrCreateManagedSequence(
      std::string sequenceName);

    /**
     * Create a new managed Kompute sequence so it's available within the
     * manager.
     *
     * @param sequenceName The name for the named sequence to be created, if
     * empty then default indexed value is used
     * @param queueIndex The queue to use from the available queues
     * @return Weak pointer to the manager owned sequence resource
     */
    std::shared_ptr<Sequence> createManagedSequence(
      std::string sequenceName = "",
      uint32_t queueIndex = 0);

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
        SPDLOG_DEBUG("Kompute Manager evalOp triggered");
        std::shared_ptr<kp::Sequence> sq =
          this->getOrCreateManagedSequence(sequenceName);

        SPDLOG_DEBUG("Kompute Manager evalOp running sequence BEGIN");
        sq->begin();

        SPDLOG_DEBUG("Kompute Manager evalOp running sequence RECORD");
        sq->record<T>(tensors, std::forward<TArgs>(params)...);

        SPDLOG_DEBUG("Kompute Manager evalOp running sequence END");
        sq->end();

        SPDLOG_DEBUG("Kompute Manager evalOp running sequence EVAL");
        sq->eval();

        SPDLOG_DEBUG("Kompute Manager evalOp running sequence SUCCESS");
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
        SPDLOG_DEBUG("Kompute Manager evalOp Default triggered");
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
        SPDLOG_DEBUG("Kompute Manager evalOpAsync triggered");

        std::shared_ptr<kp::Sequence> sq =
          this->getOrCreateManagedSequence(sequenceName);

        SPDLOG_DEBUG("Kompute Manager evalOpAsync running sequence BEGIN");
        sq->begin();

        SPDLOG_DEBUG("Kompute Manager evalOpAsync running sequence RECORD");
        sq->record<T>(tensors, std::forward<TArgs>(params)...);

        SPDLOG_DEBUG("Kompute Manager evalOpAsync running sequence END");
        sq->end();

        SPDLOG_DEBUG("Kompute Manager evalOpAsync running sequence EVAL");
        sq->evalAsync();

        SPDLOG_DEBUG("Kompute Manager evalOpAsync running sequence SUCCESS");
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
        SPDLOG_DEBUG("Kompute Manager evalOpAsyncDefault triggered");
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
        SPDLOG_DEBUG("Kompute Manager evalOpAwait triggered with sequence {}",
                     sequenceName);
        std::unordered_map<std::string, std::shared_ptr<Sequence>>::iterator
          found = this->mManagedSequences.find(sequenceName);

        if (found != this->mManagedSequences.end()) {
            if (std::shared_ptr<kp::Sequence> sq = found->second) {
                SPDLOG_DEBUG("Kompute Manager evalOpAwait running sequence "
                             "Sequence EVAL AWAIT");
                if (sq->isRunning()) {
                    sq->evalAwait(waitFor);
                }
            }
            SPDLOG_DEBUG(
              "Kompute Manager evalOpAwait running sequence SUCCESS");
        } else {
            SPDLOG_ERROR("Kompute Manager evalOpAwait Sequence not found");
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
        SPDLOG_DEBUG("Kompute Manager evalOpAwaitDefault triggered");
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
     * @returns Initialized Tensor with memory Syncd to GPU device
     */
    std::shared_ptr<Tensor> buildTensor(
      const std::vector<float>& data,
      Tensor::TensorTypes tensorType = Tensor::TensorTypes::eDevice)
    {
        SPDLOG_DEBUG("Kompute Manager buildTensor triggered");

        SPDLOG_DEBUG("Kompute Manager creating new tensor shared ptr");
        std::shared_ptr<Tensor> tensor =
          std::make_shared<Tensor>(kp::Tensor(data, tensorType));

        tensor->init(this->mPhysicalDevice, this->mDevice);
        if (tensor->tensorType() != Tensor::TensorTypes::eStorage) {
            tensor->mapDataIntoHostMemory();
        }
        this->mManagedTensors.insert(tensor);

        return tensor;
    }

    /**
     * Function that simplifies the common workflow of tensor initialisation. It
     * will take the constructor parameters for a Tensor and will will us it to
     * create a new Tensor. The tensor memory will then be managed and owned by
     * the manager.
     *
     * @param data The data to initialize the tensor with
     * @param tensorType The type of tensor to initialize
     * @returns Initialized Tensor with memory Syncd to GPU device
     */
    void rebuildTensors(std::vector<std::shared_ptr<kp::Tensor>> tensors)
    {
        SPDLOG_DEBUG("Kompute Manager rebuildTensors triggered");
        for (std::shared_ptr<Tensor> tensor : tensors) {

            if (tensor->isInit()) {
                tensor->freeMemoryDestroyGPUResources();
            }

            tensor->init(this->mPhysicalDevice, this->mDevice);
            if (tensor->tensorType() != Tensor::TensorTypes::eStorage) {
                tensor->mapDataIntoHostMemory();
            }

            std::set<std::shared_ptr<Tensor>>::iterator it =
              this->mManagedTensors.find(tensor);
            if (it == this->mManagedTensors.end()) {
                this->mManagedTensors.insert(tensor);
            }
        }
    }

  private:
    // -------------- OPTIONALLY OWNED RESOURCES
    std::shared_ptr<vk::Instance> mInstance = nullptr;
    bool mFreeInstance = false;
    std::shared_ptr<vk::PhysicalDevice> mPhysicalDevice = nullptr;
    uint32_t mPhysicalDeviceIndex = -1;
    std::shared_ptr<vk::Device> mDevice = nullptr;
    bool mFreeDevice = false;

    // -------------- ALWAYS OWNED RESOURCES
    std::set<std::shared_ptr<Tensor>> mManagedTensors;

    std::unordered_map<std::string, std::shared_ptr<Sequence>>
      mManagedSequences;

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
