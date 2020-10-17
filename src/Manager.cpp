
#include <set>
#include <string>

#include "kompute/Manager.hpp"

namespace kp {

#if DEBUG
#ifndef KOMPUTE_DISABLE_VK_DEBUG_LAYERS
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
#endif

Manager::Manager()
  : Manager(0)
{}

Manager::Manager(uint32_t physicalDeviceIndex, const std::vector<uint32_t> & familyQueueIndeces)
{
    this->mPhysicalDeviceIndex = physicalDeviceIndex;

    this->createInstance();
    this->createDevice(familyQueueIndeces);
}

Manager::Manager(std::shared_ptr<vk::Instance> instance,
                 std::shared_ptr<vk::PhysicalDevice> physicalDevice,
                 std::shared_ptr<vk::Device> device,
                 uint32_t physicalDeviceIndex)
{
    this->mInstance = instance;
    this->mPhysicalDevice = physicalDevice;
    this->mDevice = device;
    this->mPhysicalDeviceIndex = physicalDeviceIndex;
}

Manager::~Manager()
{
    SPDLOG_DEBUG("Kompute Manager Destructor started");

    if (this->mDevice == nullptr) {
        SPDLOG_ERROR(
          "Kompute Manager destructor reached with null Device pointer");
        return;
    }

    if (this->mManagedSequences.size()) {
        SPDLOG_DEBUG("Releasing managed sequence");
        this->mManagedSequences.clear();
    }

    if (this->mFreeDevice) {
        SPDLOG_INFO("Destroying device");
        this->mDevice->destroy();
        SPDLOG_DEBUG("Kompute Manager Destroyed Device");
    }

    if (this->mInstance == nullptr) {
        SPDLOG_ERROR(
          "Kompute Manager destructor reached with null Instance pointer");
        return;
    }

#if DEBUG
#ifndef KOMPUTE_DISABLE_VK_DEBUG_LAYERS
    if (this->mDebugReportCallback) {
        this->mInstance->destroyDebugReportCallbackEXT(
          this->mDebugReportCallback, nullptr, this->mDebugDispatcher);
        SPDLOG_DEBUG("Kompute Manager Destroyed Debug Report Callback");
    }
#endif
#endif

    if (this->mFreeInstance) {
        this->mInstance->destroy();
        SPDLOG_DEBUG("Kompute Manager Destroyed Instance");
    }
}

std::weak_ptr<Sequence>
Manager::getOrCreateManagedSequence(std::string sequenceName)
{
    SPDLOG_DEBUG("Kompute Manager creating Sequence object");
    std::unordered_map<std::string, std::shared_ptr<Sequence>>::iterator found =
      this->mManagedSequences.find(sequenceName);

    if (found == this->mManagedSequences.end()) {
        return this->createManagedSequence(sequenceName);
    } else {
        return found->second;
    }
}

std::weak_ptr<Sequence>
Manager::createManagedSequence(std::string sequenceName, uint32_t queueIndex) {

    SPDLOG_DEBUG("Kompute Manager createManagedSequence with sequenceName: {} and queueIndex: {}", sequenceName, queueIndex);

    std::shared_ptr<Sequence> sq =
      std::make_shared<Sequence>(this->mPhysicalDevice,
                                 this->mDevice,
                                 this->mComputeQueues[queueIndex],
                                 this->mComputeQueueFamilyIndeces[queueIndex]);
    sq->init();
    this->mManagedSequences.insert({ sequenceName, sq });
    return sq;
}

void
Manager::createInstance()
{

    SPDLOG_DEBUG("Kompute Manager creating instance");

    this->mFreeInstance = true;

    vk::ApplicationInfo applicationInfo;
    applicationInfo.pApplicationName = "Vulkan Kompute";
    applicationInfo.pEngineName = "VulkanKompute";
    applicationInfo.apiVersion = KOMPUTE_VK_API_VERSION;
    applicationInfo.engineVersion = KOMPUTE_VK_API_VERSION;
    applicationInfo.applicationVersion = KOMPUTE_VK_API_VERSION;

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
#ifndef KOMPUTE_DISABLE_VK_DEBUG_LAYERS
    SPDLOG_DEBUG("Kompute Manager adding debug validation layers");
    // We'll identify the layers that are supported
    std::vector<const char*> validLayerNames;
    std::vector<const char*> desiredLayerNames = {
        "VK_LAYER_LUNARG_assistant_layer", "VK_LAYER_LUNARG_standard_validation"
    };
    // Identify the valid layer names based on the desiredLayerNames
    {
        std::set<std::string> uniqueLayerNames;
        std::vector<vk::LayerProperties> availableLayerProperties =
          vk::enumerateInstanceLayerProperties();
        for (vk::LayerProperties layerProperties : availableLayerProperties) {
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
        computeInstanceCreateInfo.ppEnabledLayerNames = validLayerNames.data();
    }
#endif
#endif

    this->mInstance = std::make_shared<vk::Instance>();
    vk::createInstance(
      &computeInstanceCreateInfo, nullptr, this->mInstance.get());
    SPDLOG_DEBUG("Kompute Manager Instance Created");

#if DEBUG
#ifndef KOMPUTE_DISABLE_VK_DEBUG_LAYERS
    SPDLOG_DEBUG("Kompute Manager adding debug callbacks");
    if (validLayerNames.size() > 0) {
        vk::DebugReportFlagsEXT debugFlags =
          vk::DebugReportFlagBitsEXT::eError |
          vk::DebugReportFlagBitsEXT::eWarning;
        vk::DebugReportCallbackCreateInfoEXT debugCreateInfo = {};
        debugCreateInfo.pfnCallback =
          (PFN_vkDebugReportCallbackEXT)debugMessageCallback;
        debugCreateInfo.flags = debugFlags;

        this->mDebugDispatcher.init(*this->mInstance, &vkGetInstanceProcAddr);
        this->mDebugReportCallback =
          this->mInstance->createDebugReportCallbackEXT(
            debugCreateInfo, nullptr, this->mDebugDispatcher);
    }
#endif
#endif
}

void
Manager::createDevice(const std::vector<uint32_t> & familyQueueIndeces)
{

    SPDLOG_DEBUG("Kompute Manager creating Device");

    if (this->mInstance == nullptr) {
        throw std::runtime_error("Kompute Manager instance is null");
    }
    if (this->mPhysicalDeviceIndex < 0) {
        throw std::runtime_error(
          "Kompute Manager physical device index not provided");
    }

    this->mFreeDevice = true;

    std::vector<vk::PhysicalDevice> physicalDevices =
      this->mInstance->enumeratePhysicalDevices();

    vk::PhysicalDevice physicalDevice =
      physicalDevices[this->mPhysicalDeviceIndex];

    this->mPhysicalDevice =
      std::make_shared<vk::PhysicalDevice>(physicalDevice);

    vk::PhysicalDeviceProperties physicalDeviceProperties =
      physicalDevice.getProperties();

    SPDLOG_INFO("Using physical device index {} found {}",
                this->mPhysicalDeviceIndex,
                physicalDeviceProperties.deviceName);

    if (!familyQueueIndeces.size()) {
        // Find compute queue
        std::vector<vk::QueueFamilyProperties> allQueueFamilyProperties =
          physicalDevice.getQueueFamilyProperties();

        uint32_t computeQueueFamilyIndex = -1;
        for (uint32_t i = 0; i < allQueueFamilyProperties.size(); i++) {
            vk::QueueFamilyProperties queueFamilyProperties =
              allQueueFamilyProperties[i];

            if (queueFamilyProperties.queueFlags & vk::QueueFlagBits::eCompute) {
                computeQueueFamilyIndex = i;
                break;
            }
        }

        if (computeQueueFamilyIndex < 0) {
            throw std::runtime_error("Compute queue is not supported");
        }

        this->mComputeQueueFamilyIndeces.push_back(computeQueueFamilyIndex);
    }
    else {
        this->mComputeQueueFamilyIndeces = familyQueueIndeces;
    }

    std::unordered_map<uint32_t, uint32_t> familyQueueCounts;
    std::unordered_map<uint32_t, std::vector<float>> familyQueuePriorities;
    for (const auto& value : this->mComputeQueueFamilyIndeces) {
        familyQueueCounts[value]++;
        familyQueuePriorities[value].push_back(1.0f);
    }

    std::unordered_map<uint32_t, uint32_t> familyQueueIndexCount;
    std::vector<vk::DeviceQueueCreateInfo> deviceQueueCreateInfos;
    for (const auto& familyQueueInfo : familyQueueCounts) {
        // Setting the device count to 0
        familyQueueIndexCount[familyQueueInfo.first] = 0;

        // Creating the respective device queue
        vk::DeviceQueueCreateInfo deviceQueueCreateInfo(
                vk::DeviceQueueCreateFlags(),
                familyQueueInfo.first,
                familyQueueInfo.second,
                familyQueuePriorities[familyQueueInfo.first].data());
        deviceQueueCreateInfos.push_back(deviceQueueCreateInfo);
    }

    vk::DeviceCreateInfo deviceCreateInfo(vk::DeviceCreateFlags(),
        deviceQueueCreateInfos.size(),
        deviceQueueCreateInfos.data());

    this->mDevice = std::make_shared<vk::Device>();
    physicalDevice.createDevice(
      &deviceCreateInfo, nullptr, this->mDevice.get());
    SPDLOG_DEBUG("Kompute Manager device created");

    for (const uint32_t & familyQueueIndex : this->mComputeQueueFamilyIndeces) 
    {
        std::shared_ptr<vk::Queue> currQueue = std::make_shared<vk::Queue>();

        this->mDevice->getQueue(
          familyQueueIndex, familyQueueIndexCount[familyQueueIndex], currQueue.get());

        familyQueueIndexCount[familyQueueIndex]++;

        this->mComputeQueues.push_back(currQueue);
    }

    SPDLOG_DEBUG("Kompute Manager compute queue obtained");
}

}
