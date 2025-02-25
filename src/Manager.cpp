// SPDX-License-Identifier: Apache-2.0

#include "kompute/Manager.hpp"
#include "kompute/logger/Logger.hpp"
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <iterator>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>

template <> struct fmt::formatter<vk::ValidationFeatureEnableEXT>: public fmt::formatter<std::string> {

  // parse is inherited from formatter<string>.

  template <typename Context>
  fmt::format_context::iterator format (vk::ValidationFeatureEnableEXT const& flag, Context& ctx) const {
      std::string name {"unknown"};
      switch(flag) {
        case vk::ValidationFeatureEnableEXT::eDebugPrintf:
            name = "debugPrintF";
            break;
        case vk::ValidationFeatureEnableEXT::eGpuAssisted:
            name = "gpuAssisted";
            break;
        case vk::ValidationFeatureEnableEXT::eGpuAssistedReserveBindingSlot:
            name = "gpuAssistedReserveBindingSlot";
            break;
        case vk::ValidationFeatureEnableEXT::eBestPractices:
            name = "bestPractices";
            break;
        case vk::ValidationFeatureEnableEXT::eSynchronizationValidation:
            name = "synchronizationValidation";
            break;
      }
      return fmt::formatter<std::string>::format(name, ctx);
  }
};

namespace kp {

#ifndef KOMPUTE_DISABLE_VK_DEBUG_LAYERS
static VKAPI_ATTR VkBool32 VKAPI_CALL
debugMessageCallback(VkDebugReportFlagsEXT /*flags*/,
                     VkDebugReportObjectTypeEXT /*objectType*/,
                     uint64_t /*object*/,
                     size_t /*location*/,
                     int32_t /*messageCode*/,
#if KOMPUTE_OPT_ACTIVE_LOG_LEVEL <= KOMPUTE_LOG_LEVEL_DEBUG
                     const char* pLayerPrefix,
                     const char* pMessage,
#else
                     const char* /*pLayerPrefix*/,
                     const char* /*pMessage*/,
#endif
                     void* /*pUserData*/)
{
    KP_LOG_DEBUG("[VALIDATION]: {} - {}", pLayerPrefix, pMessage);
    return VK_FALSE;
}
#endif

Manager::Manager()
  : Manager(0)
{
}

Manager::Manager(uint32_t physicalDeviceIndex,
                 const std::vector<uint32_t>& familyQueueIndices,
                 const std::vector<std::string>& desiredExtensions)
{
    this->mManageResources = true;

// Make sure the logger is setup
#if !KOMPUTE_OPT_LOG_LEVEL_DISABLED
    logger::setupLogger();
#endif

    this->createInstance();
    this->createDevice(
      familyQueueIndices, physicalDeviceIndex, desiredExtensions);
}

Manager::Manager(std::shared_ptr<vk::Instance> instance,
                 std::shared_ptr<vk::PhysicalDevice> physicalDevice,
                 std::shared_ptr<vk::Device> device)
{
    this->mManageResources = false;

    this->mInstance = instance;
    this->mPhysicalDevice = physicalDevice;
    this->mDevice = device;

// Make sure the logger is setup
#if !KOMPUTE_OPT_LOG_LEVEL_DISABLED
    logger::setupLogger();
#endif
}

Manager::~Manager()
{
    KP_LOG_DEBUG("Kompute Manager Destructor started");
    this->destroy();
}

void
Manager::destroy()
{

    KP_LOG_DEBUG("Kompute Manager destroy() started");

    if (this->mDevice == nullptr) {
        KP_LOG_ERROR(
          "Kompute Manager destructor reached with null Device pointer");
        return;
    }

    if (this->mManageResources && this->mManagedSequences.size()) {
        KP_LOG_DEBUG("Kompute Manager explicitly running destructor for "
                     "managed sequences");
        for (const std::weak_ptr<Sequence>& weakSq : this->mManagedSequences) {
            if (std::shared_ptr<Sequence> sq = weakSq.lock()) {
                sq->destroy();
            }
        }
        this->mManagedSequences.clear();
    }

    if (this->mManageResources && this->mManagedAlgorithms.size()) {
        KP_LOG_DEBUG("Kompute Manager explicitly freeing algorithms");
        for (const std::weak_ptr<Algorithm>& weakAlgorithm :
             this->mManagedAlgorithms) {
            if (std::shared_ptr<Algorithm> algorithm = weakAlgorithm.lock()) {
                algorithm->destroy();
            }
        }
        this->mManagedAlgorithms.clear();
    }

    if (this->mManageResources && this->mManagedMemObjects.size()) {
        KP_LOG_DEBUG("Kompute Manager explicitly freeing memory objects");
        for (const std::weak_ptr<Memory>& weakMemory :
             this->mManagedMemObjects) {
            if (std::shared_ptr<Memory> memory = weakMemory.lock()) {
                memory->destroy();
            }
        }
        this->mManagedMemObjects.clear();
    }

    if (this->mFreeDevice) {
        KP_LOG_INFO("Destroying device");
        this->mDevice->destroy(
          (vk::Optional<const vk::AllocationCallbacks>)nullptr);
        this->mDevice = nullptr;
        KP_LOG_DEBUG("Kompute Manager Destroyed Device");
    }

    if (this->mInstance == nullptr) {
        KP_LOG_ERROR(
          "Kompute Manager destructor reached with null Instance pointer");
        return;
    }

#ifndef KOMPUTE_DISABLE_VK_DEBUG_LAYERS
    if (this->mDebugReportCallback) {
        this->mInstance->destroyDebugReportCallbackEXT(
          this->mDebugReportCallback, nullptr, this->mDebugDispatcher);
        KP_LOG_DEBUG("Kompute Manager Destroyed Debug Report Callback");
    }
#endif

    if (this->mFreeInstance) {
        this->mInstance->destroy(
          (vk::Optional<const vk::AllocationCallbacks>)nullptr);
        this->mInstance = nullptr;
        KP_LOG_DEBUG("Kompute Manager Destroyed Instance");
    }
}

void
Manager::createInstance()
{

    KP_LOG_DEBUG("Kompute Manager creating instance");

    this->mFreeInstance = true;

    vk::ApplicationInfo applicationInfo;
    applicationInfo.pApplicationName = "Kompute";
    applicationInfo.pEngineName = "Kompute";
    applicationInfo.apiVersion = KOMPUTE_VK_API_VERSION;
    applicationInfo.engineVersion = KOMPUTE_VK_API_VERSION;
    applicationInfo.applicationVersion = KOMPUTE_VK_API_VERSION;

    std::vector<const char*> applicationExtensions;

#ifndef KOMPUTE_DISABLE_VK_DEBUG_LAYERS
    applicationExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif

    vk::InstanceCreateInfo computeInstanceCreateInfo;
    computeInstanceCreateInfo.pApplicationInfo = &applicationInfo;

#ifdef __APPLE__
    // Required for backwards compatibility for MacOS M1 devices
    // https://stackoverflow.com/questions/72374316/validation-error-on-device-extension-on-m1-mac
    applicationExtensions.push_back(
      VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    computeInstanceCreateInfo.flags |=
      vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;
#endif

    if (!applicationExtensions.empty()) {
        computeInstanceCreateInfo.setEnabledExtensionCount(
            (uint32_t)applicationExtensions.size());
        computeInstanceCreateInfo.setPpEnabledExtensionNames(
            applicationExtensions.data());
        KP_LOG_DEBUG("Desired extensions: {}", fmt::join(applicationExtensions, ", "));
    }

#ifndef KOMPUTE_DISABLE_VK_DEBUG_LAYERS
    KP_LOG_DEBUG("Kompute Manager adding debug validation layers");
    // We'll identify the layers that are supported
    std::vector<const char*> validLayerNames;
    std::vector<const char*> desiredLayerNames = {
        "VK_LAYER_LUNARG_assistant_layer",
        "VK_LAYER_LUNARG_standard_validation",
        "VK_LAYER_KHRONOS_validation",
    };
    std::vector<std::string> envLayerNames;
    const char* envLayerNamesVal = std::getenv("KOMPUTE_ENV_DEBUG_LAYERS");
    if (envLayerNamesVal != nullptr && *envLayerNamesVal != '\0') {
        KP_LOG_DEBUG("Kompute Manager adding environment layers: {}",
                     envLayerNamesVal);
        std::istringstream iss(envLayerNamesVal);
        std::istream_iterator<std::string> beg(iss);
        std::istream_iterator<std::string> end;
        envLayerNames = std::vector<std::string>(beg, end);
        for (const std::string& layerName : envLayerNames) {
            desiredLayerNames.push_back(layerName.c_str());
        }
    }
    KP_LOG_DEBUG("Desired layers: {}", fmt::join(desiredLayerNames, ", "));

    vk::ValidationFeaturesEXT validationFeatures;
    std::vector<vk::ValidationFeatureEnableEXT>  validationFeatureEnablesExt;
    const char* envPrintfVal = std::getenv("KOMPUTE_ENV_PRINTF");
    if (envPrintfVal != nullptr && *envPrintfVal != '\0') {
        KP_LOG_DEBUG("Kompute Manager adding debug printf");
        validationFeatureEnablesExt.push_back(vk::ValidationFeatureEnableEXT::eDebugPrintf);
    }

    if (!validationFeatureEnablesExt.empty()) {
        validationFeatures.setEnabledValidationFeatures(validationFeatureEnablesExt);
        computeInstanceCreateInfo.setPNext(&validationFeatures);
        KP_LOG_DEBUG("Validation features: {}", fmt::join(validationFeatureEnablesExt, ", "));
    }

    // Identify the valid layer names based on the desiredLayerNames
    {
        std::set<std::string> uniqueLayerNames;
        std::vector<vk::LayerProperties> availableLayerProperties =
          vk::enumerateInstanceLayerProperties();
        for (vk::LayerProperties layerProperties : availableLayerProperties) {
            std::string layerName(layerProperties.layerName.data());
            uniqueLayerNames.insert(layerName);
        }
        KP_LOG_DEBUG("Available layers: {}", fmt::join(uniqueLayerNames, ", "));
        for (const char* desiredLayerName : desiredLayerNames) {
            if (uniqueLayerNames.count(desiredLayerName) != 0) {
                validLayerNames.push_back(desiredLayerName);
            }
        }
    }

    if (!validLayerNames.empty()) {
        KP_LOG_DEBUG(
          "Kompute Manager Initializing instance with valid layers: {}",
          fmt::join(validLayerNames, ", "));
        computeInstanceCreateInfo.setEnabledLayerCount(
          static_cast<uint32_t>(validLayerNames.size()));
        computeInstanceCreateInfo.setPpEnabledLayerNames(validLayerNames.data());
    } else {
        KP_LOG_WARN("Kompute Manager no valid layer names found from desired "
                    "layer names");
    }
#endif

#if VK_USE_PLATFORM_ANDROID_KHR
    vk::DynamicLoader dl;
    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr =
      dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
#endif // VK_USE_PLATFORM_ANDROID_KHR

    this->mInstance = std::make_shared<vk::Instance>();
    vk::Result createInstanceResult = vk::createInstance(
      &computeInstanceCreateInfo, nullptr, this->mInstance.get());

    if (createInstanceResult != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to create instance: " +
                                 vk::to_string(createInstanceResult));
    }

#if VK_USE_PLATFORM_ANDROID_KHR
    VULKAN_HPP_DEFAULT_DISPATCHER.init(*this->mInstance);
#endif // VK_USE_PLATFORM_ANDROID_KHR

    KP_LOG_DEBUG("Kompute Manager Instance Created");

#ifndef KOMPUTE_DISABLE_VK_DEBUG_LAYERS
    if (validLayerNames.size() > 0) {
        KP_LOG_DEBUG("Kompute Manager adding debug callbacks");
        vk::DebugReportFlagsEXT debugFlags =
          vk::DebugReportFlagBitsEXT::eError |
          vk::DebugReportFlagBitsEXT::eWarning;
        auto debugCreateInfo = vk::DebugReportCallbackCreateInfoEXT()
            .setFlags(debugFlags)
            .setPfnCallback((PFN_vkDebugReportCallbackEXT)debugMessageCallback);

        this->mDebugDispatcher.init(*this->mInstance, &vkGetInstanceProcAddr);
        this->mDebugReportCallback =
          this->mInstance->createDebugReportCallbackEXT(
            debugCreateInfo, nullptr, this->mDebugDispatcher);
    }
#endif
}

void
Manager::clear()
{
    if (this->mManageResources) {
        this->mManagedMemObjects.erase(
          std::remove_if(begin(this->mManagedMemObjects),
                         end(this->mManagedMemObjects),
                         [](std::weak_ptr<Memory> m) { return m.expired(); }),
          end(this->mManagedMemObjects));
        this->mManagedAlgorithms.erase(
          std::remove_if(
            begin(this->mManagedAlgorithms),
            end(this->mManagedAlgorithms),
            [](std::weak_ptr<Algorithm> t) { return t.expired(); }),
          end(this->mManagedAlgorithms));
        this->mManagedSequences.erase(
          std::remove_if(begin(this->mManagedSequences),
                         end(this->mManagedSequences),
                         [](std::weak_ptr<Sequence> t) { return t.expired(); }),
          end(this->mManagedSequences));
    }
}

void
Manager::createDevice(const std::vector<uint32_t>& familyQueueIndices,
                      uint32_t physicalDeviceIndex,
                      const std::vector<std::string>& desiredExtensions)
{

    KP_LOG_DEBUG("Kompute Manager creating Device");

    if (this->mInstance == nullptr) {
        throw std::runtime_error("Kompute Manager instance is null");
    }

    this->mFreeDevice = true;

    // Getting an integer that says how many vuklan devices we have
    std::vector<vk::PhysicalDevice> physicalDevices =
      this->mInstance->enumeratePhysicalDevices();
    uint32_t deviceCount = physicalDevices.size();

    // This means there are no devices at all
    if (deviceCount == 0) {
        throw std::runtime_error("Failed to find GPUs with Vulkan support! "
                                 "Maybe you haven't installed vulkan drivers?");
    }

    // This means that we're exceeding our device limit, for
    // example if we have 2 devices, just physicalDeviceIndex
    // 0 and 1 are acceptable. Hence, physicalDeviceIndex should
    // always be less than deviceCount, else we raise an error
    if (!(deviceCount > physicalDeviceIndex)) {
        throw std::runtime_error("There is no such physical index or device, "
                                 "please use your existing device");
    }

    vk::PhysicalDevice physicalDevice = physicalDevices[physicalDeviceIndex];

    this->mPhysicalDevice =
      std::make_shared<vk::PhysicalDevice>(physicalDevice);

#if KOMPUTE_OPT_ACTIVE_LOG_LEVEL <= KOMPUTE_LOG_LEVEL_INFO
    vk::PhysicalDeviceProperties physicalDeviceProperties =
      physicalDevice.getProperties();
#endif

    KP_LOG_INFO("Using physical device index {} found {}",
                physicalDeviceIndex,
                physicalDeviceProperties.deviceName.data());

    if (familyQueueIndices.empty()) {
        // Find compute queue
        std::vector<vk::QueueFamilyProperties> allQueueFamilyProperties =
          physicalDevice.getQueueFamilyProperties();

        uint32_t computeQueueFamilyIndex = 0;
        bool computeQueueSupported = false;
        for (uint32_t i = 0; i < allQueueFamilyProperties.size(); i++) {
            vk::QueueFamilyProperties queueFamilyProperties =
              allQueueFamilyProperties[i];

            if (queueFamilyProperties.queueFlags &
                vk::QueueFlagBits::eCompute) {
                computeQueueFamilyIndex = i;
                computeQueueSupported = true;
                break;
            }
        }

        if (!computeQueueSupported) {
            throw std::runtime_error("Compute queue is not supported");
        }

        this->mComputeQueueFamilyIndices.push_back(computeQueueFamilyIndex);
    } else {
        this->mComputeQueueFamilyIndices = familyQueueIndices;
    }
    KP_LOG_DEBUG("compute queue family indices: {}", fmt::join(this->mComputeQueueFamilyIndices, ", "));

    std::unordered_map<uint32_t, uint32_t> familyQueueCounts;
    std::unordered_map<uint32_t, std::vector<float>> familyQueuePriorities;
    for (const auto& value : this->mComputeQueueFamilyIndices) {
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

    KP_LOG_DEBUG("Kompute Manager desired extension layers {}",
                 fmt::join(desiredExtensions, ", "));

    std::vector<vk::ExtensionProperties> deviceExtensions =
      this->mPhysicalDevice->enumerateDeviceExtensionProperties();

    std::set<std::string> uniqueExtensionNames;
    for (const vk::ExtensionProperties& ext : deviceExtensions) {
        uniqueExtensionNames.insert(ext.extensionName);
    }
    KP_LOG_DEBUG("Kompute Manager available extensions {}",
                 fmt::join(uniqueExtensionNames, ", "));
    std::vector<const char*> validExtensions;
    for (const std::string& ext : desiredExtensions) {
        if (uniqueExtensionNames.count(ext) != 0) {
            validExtensions.push_back(ext.c_str());
        }
    }

    if (desiredExtensions.size() != validExtensions.size()) {
        KP_LOG_ERROR("Kompute Manager not all extensions were added: {}",
                     fmt::join(validExtensions, ", "));
    }

    KP_LOG_DEBUG("Kompute Manager used extensions {}",
                 fmt::join(validExtensions, ", "));

    vk::DeviceCreateInfo deviceCreateInfo(vk::DeviceCreateFlags(),
                                          deviceQueueCreateInfos.size(),
                                          deviceQueueCreateInfos.data(),
                                          {},
                                          {},
                                          validExtensions.size(),
                                          validExtensions.data());

    this->mDevice = std::make_shared<vk::Device>();
    physicalDevice.createDevice(
      &deviceCreateInfo, nullptr, this->mDevice.get());
    KP_LOG_DEBUG("Kompute Manager device created");

    for (const uint32_t& familyQueueIndex : this->mComputeQueueFamilyIndices) {
        std::shared_ptr<vk::Queue> currQueue = std::make_shared<vk::Queue>();

        this->mDevice->getQueue(familyQueueIndex,
                                familyQueueIndexCount[familyQueueIndex],
                                currQueue.get());

        familyQueueIndexCount[familyQueueIndex]++;

        this->mComputeQueues.push_back(currQueue);
    }

    KP_LOG_DEBUG("Kompute Manager compute queue obtained");
}

std::shared_ptr<Sequence>
Manager::sequence(uint32_t queueIndex, uint32_t totalTimestamps)
{
    KP_LOG_DEBUG("Kompute Manager sequence() with queueIndex: {}", queueIndex);

    std::shared_ptr<Sequence> sq{ new kp::Sequence(
      this->mPhysicalDevice,
      this->mDevice,
      this->mComputeQueues[queueIndex],
      this->mComputeQueueFamilyIndices[queueIndex],
      totalTimestamps) };

    if (this->mManageResources) {
        this->mManagedSequences.push_back(sq);
    }

    return sq;
}

vk::PhysicalDeviceProperties
Manager::getDeviceProperties() const
{
    return this->mPhysicalDevice->getProperties();
}

std::vector<vk::PhysicalDevice>
Manager::listDevices() const
{
    return this->mInstance->enumeratePhysicalDevices();
}

std::shared_ptr<vk::Instance>
Manager::getVkInstance() const
{
    return this->mInstance;
}

}
