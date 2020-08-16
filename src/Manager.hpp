#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>

namespace kp {

class Manager
{
private:

public:
    Manager();

    virtual ~Manager();

    // Templated Commands

private:
    vk::Instance* mInstance = nullptr;
    bool mFreeInstance = false;
    vk::Device* mDevice = nullptr;
    bool mFreeDevice = false;
    uint32_t mComputeQueueFamilyIndex = -1;
    vk::Queue* mComputeQueue = nullptr;
    bool mFreeComputeQueue = false;

#if DEBUG
    vk::DebugReportCallbackEXT mDebugReportCallback;
    vk::DispatchLoaderDynamic mDebugDispatcher;
#endif

    // Create functions
    void createInstance();
    void createDevice();

};

} // End namespace kp

