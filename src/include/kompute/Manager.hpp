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
        Constructor
    */
    Manager();

    Manager(uint32_t physicalDeviceIndex);

    Manager(std::shared_ptr<vk::Instance> instance,
            std::shared_ptr<vk::PhysicalDevice> physicalDevice,
            std::shared_ptr<vk::Device> device,
            uint32_t physicalDeviceIndex);

    ~Manager();

    std::weak_ptr<Sequence> getOrCreateManagedSequence(std::string sessionName);

    template<typename T, typename... TArgs>
    void evalOp(std::vector<std::shared_ptr<Tensor>> tensors,
                std::string sessionName = KP_DEFAULT_SESSION)
    {
        SPDLOG_DEBUG("Kompute Manager evalOp triggered");
        std::weak_ptr<Sequence> sqWeakPtr =
          this->getOrCreateManagedSequence(sessionName);

        if (std::shared_ptr<kp::Sequence> sq = sqWeakPtr.lock()) {
            SPDLOG_DEBUG("Kompute Manager evalOp running sequence BEGIN");
            sq->begin();

            SPDLOG_DEBUG("Kompute Manager evalOp running sequence RECORD");
            sq->record<T>(tensors);

            SPDLOG_DEBUG("Kompute Manager evalOp running sequence END");
            sq->end();

            SPDLOG_DEBUG("Kompute Manager evalOp running sequence EVAL");
            sq->eval();
        }
        SPDLOG_DEBUG("Kompute Manager evalOp running sequence SUCCESS");
    }

  private:
    std::shared_ptr<vk::Instance> mInstance = nullptr;
    bool mFreeInstance = false;
    std::shared_ptr<vk::PhysicalDevice> mPhysicalDevice = nullptr;
    uint32_t mPhysicalDeviceIndex = -1;
    std::shared_ptr<vk::Device> mDevice = nullptr;
    bool mFreeDevice = false;
    uint32_t mComputeQueueFamilyIndex = -1;
    std::shared_ptr<vk::Queue> mComputeQueue = nullptr;

    // Always owned resources
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
