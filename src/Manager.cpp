#if defined(_WIN32)
#pragma comment(linker, "/subsystem:console")
#endif

// SPDLOG_ACTIVE_LEVEL must be defined before spdlog.h import
#if DEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#endif

#include <set>
#include <string>

#include <spdlog/spdlog.h>

#include "Manager.hpp"

namespace kp {

#if DEBUG
static VKAPI_ATTR VkBool32 VKAPI_CALL
debugMessageCallback(VkDebugReportFlagsEXT flags,
                     VkDebugReportObjectTypeEXT objectType,
                     uint64_t object,
                     size_t location,
                     int32_t messageCode,
                     const char* pLayerPrefix,
                     const char* pMessage,
                     void* pUserData)
{
    SPDLOG_DEBUG("[VALIDATION]: {} - {}", pLayerPrefix, pMessage);
    return VK_FALSE;
}
#endif

Manager::Manager()
{
    this->createInstance();
    this->createDevice();
}

Manager::~Manager()
{
    SPDLOG_DEBUG("Destroying Kompute Manager");

    if (this->mDevice == nullptr) {
        spdlog::error("Kompute Manager destructor reached with null Device pointer");
        return;
    }

    if (this->mFreeDevice) {
        this->mDevice->destroy();
    }

    if (this->mInstance == nullptr) {
        spdlog::error("Kompute Manager destructor reached with null Instance pointer");
        return;
    }

    if (this->mFreeInstance) {
        this->mInstance->destroy();
    }
}

void Manager::createInstance() {

    this->mFreeInstance = true;

    vk::ApplicationInfo applicationInfo;
    applicationInfo.pApplicationName = "Vulkan compute";
    applicationInfo.pEngineName = "VulkanCompute";
    applicationInfo.apiVersion = VK_API_VERSION_1_2;

    std::vector<const char*> applicationExtensions;
    applicationExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

    vk::InstanceCreateInfo computeInstanceCreateInfo;
    computeInstanceCreateInfo.pApplicationInfo = &applicationInfo;
    if (!applicationExtensions.empty()) {
        computeInstanceCreateInfo.enabledExtensionCount =
          (uint32_t)applicationExtensions.size();
        computeInstanceCreateInfo.ppEnabledExtensionNames =
          applicationExtensions.data();
    }

#if DEBUG
    // We'll identify the layers that are supported
    std::vector<const char*> validLayerNames;
    std::vector<const char*> desiredLayerNames = {
        "VK_LAYER_LUNARG_assistant_layer",
        "VK_LAYER_LUNARG_standard_validation"
    };
    // Identify the valid layer names based on the desiredLayerNames
    {
        std::set<std::string> uniqueLayerNames;
        std::vector<vk::LayerProperties> availableLayerProperties =
          vk::enumerateInstanceLayerProperties();
        for (vk::LayerProperties layerProperties :
             availableLayerProperties) {
            std::string layerName(layerProperties.layerName);
            uniqueLayerNames.insert(layerName);
        }
        for (const char* desiredLayerName : desiredLayerNames) {
            if (uniqueLayerNames.count(desiredLayerName) != 0) {
                validLayerNames.push_back(desiredLayerName);
            }
        }
    }

    if (validLayerNames.size() > 0) {
        computeInstanceCreateInfo.enabledLayerCount =
          (uint32_t)validLayerNames.size();
        computeInstanceCreateInfo.ppEnabledLayerNames =
          validLayerNames.data();
    }
#endif

    vk::createInstance(&computeInstanceCreateInfo, nullptr, this->mInstance);

#if DEBUG
    if (validLayerNames.size() > 0) {
        vk::DebugReportFlagsEXT debugFlags =
          vk::DebugReportFlagBitsEXT::eError |
          vk::DebugReportFlagBitsEXT::eWarning;
        vk::DebugReportCallbackCreateInfoEXT debugCreateInfo = {};
        debugCreateInfo.pfnCallback =
          (PFN_vkDebugReportCallbackEXT)debugMessageCallback;
        debugCreateInfo.flags = debugFlags;

        this->mDebugDispatcher.init(*this->mInstance,
                                    &vkGetInstanceProcAddr);
        this->mDebugReportCallback =
          this->mInstance->createDebugReportCallbackEXT(
            debugCreateInfo, nullptr, this->mDebugDispatcher);
    }
#endif
}

void Manager::createDevice() {

    if (this->mInstance == nullptr) {
        throw std::runtime_error("Manager instance is null");
    }

    this->mFreeDevice = true;

    std::vector<vk::PhysicalDevice> physicalDevices =
      this->mInstance->enumeratePhysicalDevices();

    vk::PhysicalDevice physicalDevice = physicalDevices[0];

    vk::PhysicalDeviceProperties physicalDeviceProperties =
      physicalDevice.getProperties();

    spdlog::info("Using device {}", physicalDeviceProperties.deviceName);

    // Find compute queue
    std::vector<vk::QueueFamilyProperties> allQueueFamilyProperties =
      physicalDevice.getQueueFamilyProperties();

    this->mComputeQueueFamilyIndex = -1;
    for (uint32_t i = 0; i < allQueueFamilyProperties.size(); i++) {
        vk::QueueFamilyProperties queueFamilyProperties =
          allQueueFamilyProperties[i];

        if (queueFamilyProperties.queueFlags &
            vk::QueueFlagBits::eCompute) {
            this->mComputeQueueFamilyIndex = i;
            break;
        }
    }

    if (this->mComputeQueueFamilyIndex < 0) {
        spdlog::critical("Compute queue is not supported");
    }

    const float defaultQueuePriority(0.0f);
    const uint32_t defaultQueueCount(1);
    vk::DeviceQueueCreateInfo deviceQueueCreateInfo(
      vk::DeviceQueueCreateFlags(),
      this->mComputeQueueFamilyIndex,
      defaultQueueCount,
      &defaultQueuePriority);

    vk::DeviceCreateInfo deviceCreateInfo(
      vk::DeviceCreateFlags(),
      1, // Number of deviceQueueCreateInfo
      &deviceQueueCreateInfo);

    physicalDevice.createDevice(&deviceCreateInfo, nullptr, this->mDevice);

    this->mDevice->getQueue(this->mComputeQueueFamilyIndex, 0, this->mComputeQueue);
}

}
