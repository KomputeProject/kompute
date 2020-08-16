#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>

#include <spdlog/spdlog.h>

#include "Sequence.hpp"

namespace kp {

class Manager
{
private:

public:
    Manager();

    ~Manager();

    // Evaluate actions
    template <typename T, typename... TArgs>
    void eval(TArgs&&... args) {
        SPDLOG_DEBUG("Kompute Manager eval triggered");
        Sequence sq(this->mDevice, this->mComputeQueue, this->mComputeQueueFamilyIndex);
        sq.begin();
        sq.record<T>(std::forward<TArgs>(args)...);
        sq.end();
        sq.eval();
    }


private:
    vk::Instance* mInstance = nullptr;
    bool mFreeInstance = false;
    uint32_t mPhysicalDeviceIndex = -1;
    vk::Device* mDevice = nullptr;
    bool mFreeDevice = false;
    uint32_t mComputeQueueFamilyIndex = -1;
    vk::Queue* mComputeQueue = nullptr;

#if DEBUG
    vk::DebugReportCallbackEXT mDebugReportCallback;
    vk::DispatchLoaderDynamic mDebugDispatcher;
#endif

    // Create functions
    void createInstance();
    void createDevice();
};

} // End namespace kp

