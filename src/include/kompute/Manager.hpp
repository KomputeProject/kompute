#pragma once

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
  private:
  public:
    /**
        Base constructor and default used which creates the base resources
       including choosing the device 0 by default.
    */
    Manager();

    /**
        Similar to base constructor but allows the user to provide the device
       they would like to create the resources on.
    */
    Manager(uint32_t physicalDeviceIndex);

    /**
     * Manager constructor which allows your own vulkan application to integrate
     * with the vulkan kompute use.
     *
     * @param instance Vulkan compute instance to base this application
     * @physicalDevice Vulkan physical device to use for application
     * @device Vulkan logical device to use for all base resources
     * @physicalDeviceIndex Index for vulkan physical device used
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
     * @return Weak pointer to the manager owned sequence resource
     */
    std::weak_ptr<Sequence> getOrCreateManagedSequence(
      std::string sequenceName);

    /**
     * Operation that adds extra operations to existing or new created
     * sequences.
     *
     * @param tensors The tensors to be used in the operation recorded
     * @param sequenceName The name of the sequence to be retrieved or created
     * @param TArgs Template parameters that will be used to initialise Operation to allow for extensible configurations on initialisation
     */
    template<typename T, typename... TArgs>
    void evalOp(std::vector<std::shared_ptr<Tensor>> tensors,
                std::string sequenceName = KP_DEFAULT_SESSION,
                TArgs&&... params)
    {
        SPDLOG_DEBUG("Kompute Manager evalOp triggered");
        std::weak_ptr<Sequence> sqWeakPtr =
          this->getOrCreateManagedSequence(sequenceName);

        if (std::shared_ptr<kp::Sequence> sq = sqWeakPtr.lock()) {
            SPDLOG_DEBUG("Kompute Manager evalOp running sequence BEGIN");
            sq->begin();

            SPDLOG_DEBUG("Kompute Manager evalOp running sequence RECORD");
            sq->record<T>(tensors, std::forward<TArgs>(params)...);

            SPDLOG_DEBUG("Kompute Manager evalOp running sequence END");
            sq->end();

            SPDLOG_DEBUG("Kompute Manager evalOp running sequence EVAL");
            sq->eval();
        }
        SPDLOG_DEBUG("Kompute Manager evalOp running sequence SUCCESS");
    }

  private:
    // -------------- OPTIONALLY OWNED RESOURCES
    std::shared_ptr<vk::Instance> mInstance = nullptr;
    bool mFreeInstance = false;
    std::shared_ptr<vk::PhysicalDevice> mPhysicalDevice = nullptr;
    uint32_t mPhysicalDeviceIndex = -1;
    std::shared_ptr<vk::Device> mDevice = nullptr;
    bool mFreeDevice = false;
    uint32_t mComputeQueueFamilyIndex = -1;
    std::shared_ptr<vk::Queue> mComputeQueue = nullptr;

    // -------------- ALWAYS OWNED RESOURCES
    std::unordered_map<std::string, std::shared_ptr<Sequence>>
      mManagedSequences;

#if DEBUG
    vk::DebugReportCallbackEXT mDebugReportCallback;
    vk::DispatchLoaderDynamic mDebugDispatcher;
#endif

    // Create functions
    void createInstance();
    void createDevice();
};

} // End namespace kp
