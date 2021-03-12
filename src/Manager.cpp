
#include <iterator>
#include <set>
#include <sstream>
#include <string>

#include "kompute/Manager.hpp"

#include "fmt/ranges.h"

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
    KP_LOG_DEBUG("[VALIDATION]: {} - {}", pLayerPrefix, pMessage);
    return VK_FALSE;
}
#endif
#endif

Manager::Manager()
  : Manager(0)
{}

Manager::Manager(uint32_t physicalDeviceIndex,
                 const std::vector<uint32_t>& familyQueueIndices,
                 const std::vector<std::string>& desiredExtensions)
{
    this->mManageResources = true;

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

    if (this->mManageResources && this->mManagedTensors.size()) {
        KP_LOG_DEBUG("Kompute Manager explicitly freeing tensors");
        for (const std::weak_ptr<Tensor>& weakTensor : this->mManagedTensors) {
            if (std::shared_ptr<Tensor> tensor = weakTensor.lock()) {
                tensor->destroy();
            }
        }
        this->mManagedTensors.clear();
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

#if DEBUG
#ifndef KOMPUTE_DISABLE_VK_DEBUG_LAYERS
    if (this->mDebugReportCallback) {
        this->mInstance->destroyDebugReportCallbackEXT(
          this->mDebugReportCallback, nullptr, this->mDebugDispatcher);
        KP_LOG_DEBUG("Kompute Manager Destroyed Debug Report Callback");
    }
#endif
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
    applicationInfo.pApplicationName = "Vulkan Kompute";
    applicationInfo.pEngineName = "VulkanKompute";
    applicationInfo.apiVersion = KOMPUTE_VK_API_VERSION;
    applicationInfo.engineVersion = KOMPUTE_VK_API_VERSION;
    applicationInfo.applicationVersion = KOMPUTE_VK_API_VERSION;

    std::vector<const char*> applicationExtensions;

#if DEBUG
    applicationExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif

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
    KP_LOG_DEBUG("Kompute Manager adding environment layers: {}",
                 envLayerNamesVal);
    if (envLayerNamesVal != NULL && *envLayerNamesVal != '\0') {
        std::istringstream iss(envLayerNamesVal);
        std::istream_iterator<std::string> beg(iss), end;
        envLayerNames = std::vector<std::string>(beg, end);
        for (const std::string& layerName : envLayerNames) {
            desiredLayerNames.push_back(layerName.c_str());
        }
        KP_LOG_DEBUG("Desired layers: {}", desiredLayerNames);
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
        KP_LOG_DEBUG("Available layers: {}", uniqueLayerNames);
        for (const char* desiredLayerName : desiredLayerNames) {
            if (uniqueLayerNames.count(desiredLayerName) != 0) {
                validLayerNames.push_back(desiredLayerName);
            }
        }
    }

    if (validLayerNames.size() > 0) {
        KP_LOG_DEBUG(
          "Kompute Manager Initializing instance with valid layers: {}",
          validLayerNames);
        computeInstanceCreateInfo.enabledLayerCount =
          (uint32_t)validLayerNames.size();
        computeInstanceCreateInfo.ppEnabledLayerNames = validLayerNames.data();
    } else {
        KP_LOG_WARN("Kompute Manager no valid layer names found from desired "
                    "layer names");
    }
#endif
#endif

    this->mInstance = std::make_shared<vk::Instance>();
    vk::createInstance(
      &computeInstanceCreateInfo, nullptr, this->mInstance.get());
    KP_LOG_DEBUG("Kompute Manager Instance Created");

#if DEBUG
#ifndef KOMPUTE_DISABLE_VK_DEBUG_LAYERS
    KP_LOG_DEBUG("Kompute Manager adding debug callbacks");
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
Manager::clear()
{
    if (this->mManageResources) {
        this->mManagedTensors.erase(
          std::remove_if(begin(this->mManagedTensors),
                         end(this->mManagedTensors),
                         [](std::weak_ptr<Tensor> t) { return t.expired(); }),
          end(this->mManagedTensors));
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
    if (physicalDeviceIndex < 0) {
        throw std::runtime_error(
          "Kompute Manager physical device index not provided");
    }

    this->mFreeDevice = true;

    std::vector<vk::PhysicalDevice> physicalDevices =
      this->mInstance->enumeratePhysicalDevices();

    vk::PhysicalDevice physicalDevice = physicalDevices[physicalDeviceIndex];

    this->mPhysicalDevice =
      std::make_shared<vk::PhysicalDevice>(physicalDevice);

    vk::PhysicalDeviceProperties physicalDeviceProperties =
      physicalDevice.getProperties();

    KP_LOG_INFO("Using physical device index {} found {}",
                physicalDeviceIndex,
                physicalDeviceProperties.deviceName.data());

    if (!familyQueueIndices.size()) {
        // Find compute queue
        std::vector<vk::QueueFamilyProperties> allQueueFamilyProperties =
          physicalDevice.getQueueFamilyProperties();

        uint32_t computeQueueFamilyIndex = -1;
        for (uint32_t i = 0; i < allQueueFamilyProperties.size(); i++) {
            vk::QueueFamilyProperties queueFamilyProperties =
              allQueueFamilyProperties[i];

            if (queueFamilyProperties.queueFlags &
                vk::QueueFlagBits::eCompute) {
                computeQueueFamilyIndex = i;
                break;
            }
        }

        if (computeQueueFamilyIndex < 0) {
            throw std::runtime_error("Compute queue is not supported");
        }

        this->mComputeQueueFamilyIndices.push_back(computeQueueFamilyIndex);
    } else {
        this->mComputeQueueFamilyIndices = familyQueueIndices;
    }

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
                 desiredExtensions);

    std::vector<vk::ExtensionProperties> deviceExtensions =
      this->mPhysicalDevice->enumerateDeviceExtensionProperties();

    std::set<std::string> uniqueExtensionNames;
    for (const vk::ExtensionProperties& ext : deviceExtensions) {
        std::string extName(ext.extensionName.data());
        uniqueExtensionNames.insert(extName);
    }
    KP_LOG_DEBUG("Kompute Manager available extensions {}",
                 uniqueExtensionNames);
    std::vector<const char*> validExtensions;
    for (std::string ext : desiredExtensions) {
        if (uniqueExtensionNames.count(ext) != 0) {
            validExtensions.push_back(ext.c_str());
        }
    }
    if (desiredExtensions.size() != validExtensions.size()) {
        KP_LOG_ERROR("Kompute Manager not all extensions were added: {}",
                     validExtensions);
    }

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

std::shared_ptr<Algorithm>
Manager::algorithm(const std::vector<std::shared_ptr<Tensor>>& tensors,
                   const std::vector<uint32_t>& spirv,
                   const Workgroup& workgroup,
                   const Constants& specializationConstants,
                   const Constants& pushConstants)
{

    KP_LOG_DEBUG("Kompute Manager algorithm creation triggered");

    std::shared_ptr<Algorithm> algorithm{ new kp::Algorithm(
      this->mDevice,
      tensors,
      spirv,
      workgroup,
      specializationConstants,
      pushConstants) };

    if (this->mManageResources) {
        this->mManagedAlgorithms.push_back(algorithm);
    }

    return algorithm;
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

}
