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

private:
    vk::Instance mInstance;

    // Create functions
    vk::InstanceCreateInfo createInstanceInfo();

#if DEBUG
    vk::DebugReportCallbackEXT mDebugReportCallback;
    vk::DispatchLoaderDynamic mDebugDispatcher;
#endif
};

} // End namespace kp

