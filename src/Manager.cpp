
#include <set>
#include <string>

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
    this->mPhysicalDeviceIndex = 0;
    this->createInstance();
    this->createDevice();
}

Manager::~Manager()
{
    SPDLOG_DEBUG("Kompute Manager Destructor started");

    if (this->mDevice == nullptr) {
        spdlog::error("Kompute Manager destructor reached with null Device pointer");
        return;
    }

    if (this->mFreeDevice) {
        spdlog::info("Destroying device");
        this->mDevice->destroy();
        SPDLOG_DEBUG("Kompute Manager Destroyed Device");
    }

    if (this->mInstance == nullptr) {
        spdlog::error("Kompute Manager destructor reached with null Instance pointer");
        return;
    }

#if DEBUG
    if (this->mDebugReportCallback) {
        this->mInstance->destroyDebugReportCallbackEXT(
          this->mDebugReportCallback, nullptr, this->mDebugDispatcher);
        SPDLOG_DEBUG("Kompute Manager Destroyed Debug Report Callback");
    }
#endif

    if (this->mFreeInstance) {
        this->mInstance->destroy();
        SPDLOG_DEBUG("Kompute Manager Destroyed Instance");
    }
}

void Manager::createInstance() {

    SPDLOG_DEBUG("Kompute Manager creating instance");

    this->mFreeInstance = true;

    vk::ApplicationInfo applicationInfo;
    applicationInfo.pApplicationName = "Vulkan Kompute";
    applicationInfo.pEngineName = "VulkanKompute";
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
    SPDLOG_DEBUG("Kompute Manager adding debug validation layers");
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

    this->mInstance = std::make_shared<vk::Instance>();
    vk::createInstance(&computeInstanceCreateInfo, nullptr, this->mInstance.get());
    SPDLOG_DEBUG("Kompute Manager Instance Created");

#if DEBUG
    SPDLOG_DEBUG("Kompute Manager adding debug callbacks");
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

    SPDLOG_DEBUG("Kompute Manager creating Device");

    if (this->mInstance == nullptr) {
        throw std::runtime_error("Kompute Manager instance is null");
    }
    if (this->mPhysicalDeviceIndex < 0) {
        throw std::runtime_error("Kompute Manager physical device index not provided");
    }

    this->mFreeDevice = true;

    std::vector<vk::PhysicalDevice> physicalDevices =
      this->mInstance->enumeratePhysicalDevices();

    vk::PhysicalDevice physicalDevice = physicalDevices[this->mPhysicalDeviceIndex];

    vk::PhysicalDeviceProperties physicalDeviceProperties =
      physicalDevice.getProperties();

    spdlog::info("Using physical device index {} found {}", this->mPhysicalDeviceIndex, physicalDeviceProperties.deviceName);

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

    this->mDevice = std::make_shared<vk::Device>();
    physicalDevice.createDevice(&deviceCreateInfo, nullptr, this->mDevice.get());
    SPDLOG_DEBUG("Kompute Manager device created");

    this->mComputeQueue = std::make_shared<vk::Queue>();
    this->mDevice->getQueue(this->mComputeQueueFamilyIndex, 0, this->mComputeQueue.get());
    SPDLOG_DEBUG("Kompute Manager compute queue obtained");
}

}
