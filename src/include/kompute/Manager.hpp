#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>

// SPDLOG_ACTIVE_LEVEL must be defined before spdlog.h import
#if DEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#endif

#include <spdlog/spdlog.h>

#include "kompute/Sequence.hpp"

namespace kp {

class Manager
{
  private:
  public:
    Manager();

    Manager(std::shared_ptr<vk::Instance> instance,
            std::shared_ptr<vk::Device>,
            uint32_t queueIndex);

    ~Manager();

    Sequence constructSequence();

    template<typename T, typename... TArgs>
    void evalOp(std::vector<std::shared_ptr<Tensor>> tensors)
    {
        SPDLOG_DEBUG("Kompute Manager evalOp triggered");
        Sequence sq(this->mPhysicalDevice,
                    this->mDevice,
                    this->mComputeQueue,
                    this->mComputeQueueFamilyIndex);
        SPDLOG_DEBUG("Kompute Manager evalOp running sequence BEGIN");
        sq.begin();
        SPDLOG_DEBUG("Kompute Manager evalOp running sequence RECORD");
        sq.record<T>(tensors);
        SPDLOG_DEBUG("Kompute Manager evalOp running sequence END");
        sq.end();
        SPDLOG_DEBUG("Kompute Manager evalOp running sequence EVAL");
        sq.eval();
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

#if DEBUG
    vk::DebugReportCallbackEXT mDebugReportCallback;
    vk::DispatchLoaderDynamic mDebugDispatcher;
#endif

    // Create functions
    void createInstance();
    void createDevice();
};

} // End namespace kp
