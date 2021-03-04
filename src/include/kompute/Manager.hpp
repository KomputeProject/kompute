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
     * @param manageResources (Optional) Whether to manage the memory of the
     * resources created and destroy when the manager is destroyed.
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
            std::shared_ptr<vk::Device> device);

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
      Tensor::TensorTypes tensorType = Tensor::TensorTypes::eDevice);

    std::shared_ptr<Algorithm> algorithm(
      const std::vector<std::shared_ptr<Tensor>>& tensors = {},
      const std::vector<uint32_t>& spirv = {},
      const Workgroup& workgroup = {},
      const Constants& specializationConstants = {},
      const Constants& pushConstants = {});

    void destroy();
    void clear();

  private:
    // -------------- OPTIONALLY OWNED RESOURCES
    std::shared_ptr<vk::Instance> mInstance = nullptr;
    bool mFreeInstance = false;
    std::shared_ptr<vk::PhysicalDevice> mPhysicalDevice = nullptr;
    std::shared_ptr<vk::Device> mDevice = nullptr;
    bool mFreeDevice = false;

    // -------------- ALWAYS OWNED RESOURCES
    std::vector<std::weak_ptr<Tensor>> mManagedTensors;
    std::vector<std::weak_ptr<Sequence>> mManagedSequences;
    std::vector<std::weak_ptr<Algorithm>> mManagedAlgorithms;

    std::vector<uint32_t> mComputeQueueFamilyIndices;
    std::vector<std::shared_ptr<vk::Queue>> mComputeQueues;

    bool mManageResources = false;

#if DEBUG
#ifndef KOMPUTE_DISABLE_VK_DEBUG_LAYERS
    vk::DebugReportCallbackEXT mDebugReportCallback;
    vk::DispatchLoaderDynamic mDebugDispatcher;
#endif
#endif

    // Create functions
    void createInstance();
    void createDevice(const std::vector<uint32_t>& familyQueueIndices = {},
                      uint32_t hysicalDeviceIndex = 0);
};

} // End namespace kp
