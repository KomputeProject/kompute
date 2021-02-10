
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

Manager::Manager(uint32_t physicalDeviceIndex,
                 const std::vector<uint32_t>& familyQueueIndices)
{
    this->mPhysicalDeviceIndex = physicalDeviceIndex;

    this->createInstance();
    this->createDevice(familyQueueIndices);
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
        SPDLOG_DEBUG("Kompute Manager explicitly running destructor for "
                     "managed sequences");
        for (const std::pair<std::string, std::shared_ptr<Sequence>>& sqPair :
             this->mManagedSequences) {
            sqPair.second->freeMemoryDestroyGPUResources();
        }
        this->mManagedSequences.clear();
    }

    if (this->mManagedTensors.size()) {
        SPDLOG_DEBUG("Kompute Manager explicitly freeing tensors");
        for (const std::shared_ptr<Tensor>& tensor : this->mManagedTensors) {
            if (!tensor->isInit()) {
                SPDLOG_ERROR("Kompute Manager attempted to free managed tensor "
                             "but not tensor is not initialised");
            }
            tensor->freeMemoryDestroyGPUResources();
        }
        this->mManagedTensors.clear();
    }

    if (this->mFreeDevice) {
        SPDLOG_INFO("Destroying device");
        this->mDevice->destroy(
          (vk::Optional<const vk::AllocationCallbacks>)nullptr);
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
        this->mInstance->destroy(
          (vk::Optional<const vk::AllocationCallbacks>)nullptr);
        SPDLOG_DEBUG("Kompute Manager Destroyed Instance");
    }
}

std::shared_ptr<Sequence>
Manager::sequence(std::string sequenceName, uint32_t queueIndex)
{
    SPDLOG_DEBUG("Kompute Manager sequence() with sequenceName: {} "
                 "and queueIndex: {}",
                 sequenceName,
                 queueIndex);

    std::shared_ptr<Sequence> sq = nullptr;

    std::unordered_map<std::string, std::shared_ptr<Sequence>>::iterator found =
      this->mManagedSequences.find(sequenceName);

    if (found == this->mManagedSequences.end()) {
        std::shared_ptr<Sequence> sq =
          std::make_shared<Sequence>(this->mPhysicalDevice,
                                     this->mDevice,
                                     this->mComputeQueues[queueIndex],
                                     this->mComputeQueueFamilyIndices[queueIndex]);
        sq->init();

        this->mManagedSequences.insert({ sequenceName, sq });

        return sq;
    } else {
        return found->second;
    }
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
            std::string layerName(layerProperties.layerName.data());
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
Manager::createDevice(const std::vector<uint32_t>& familyQueueIndices)
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

    vk::DeviceCreateInfo deviceCreateInfo(vk::DeviceCreateFlags(),
                                          deviceQueueCreateInfos.size(),
                                          deviceQueueCreateInfos.data());

    this->mDevice = std::make_shared<vk::Device>();
    physicalDevice.createDevice(
      &deviceCreateInfo, nullptr, this->mDevice.get());
    SPDLOG_DEBUG("Kompute Manager device created");

    for (const uint32_t& familyQueueIndex : this->mComputeQueueFamilyIndices) {
        std::shared_ptr<vk::Queue> currQueue = std::make_shared<vk::Queue>();

        this->mDevice->getQueue(familyQueueIndex,
                                familyQueueIndexCount[familyQueueIndex],
                                currQueue.get());

        familyQueueIndexCount[familyQueueIndex]++;

        this->mComputeQueues.push_back(currQueue);
    }

    SPDLOG_DEBUG("Kompute Manager compute queue obtained");
}

std::shared_ptr<Tensor>
Manager::tensor(
  const std::vector<float>& data,
  Tensor::TensorTypes tensorType,
  bool syncDataToGPU)
{
    SPDLOG_DEBUG("Kompute Manager tensor triggered");

    SPDLOG_DEBUG("Kompute Manager creating new tensor shared ptr");
    std::shared_ptr<Tensor> tensor =
      std::make_shared<Tensor>(kp::Tensor(data, tensorType));

    tensor->init(this->mPhysicalDevice, this->mDevice);

    if (syncDataToGPU) {
        this->evalOpDefault<OpTensorSyncDevice>({ tensor });
    }
    this->mManagedTensors.insert(tensor);

    return tensor;
}

void
Manager::rebuild(std::vector<std::shared_ptr<kp::Tensor>> tensors,
                    bool syncDataToGPU)
{
    SPDLOG_DEBUG("Kompute Manager rebuild triggered");
    for (std::shared_ptr<Tensor> tensor : tensors) {

        // False syncData to run all tensors at once instead one by one
        this->rebuild(tensor, false);
    }

    if (syncDataToGPU) {
        this->evalOpDefault<OpTensorSyncDevice>(tensors);
    }
}

void
Manager::rebuild(std::shared_ptr<kp::Tensor> tensor,
                    bool syncDataToGPU)
{
    SPDLOG_DEBUG("Kompute Manager rebuild Tensor triggered");

    if (tensor->isInit()) {
        tensor->freeMemoryDestroyGPUResources();
    }

    tensor->init(this->mPhysicalDevice, this->mDevice);

    std::set<std::shared_ptr<Tensor>>::iterator it =
      this->mManagedTensors.find(tensor);
    if (it == this->mManagedTensors.end()) {
        this->mManagedTensors.insert(tensor);
    }

    if (syncDataToGPU) {
        this->evalOpDefault<OpTensorSyncDevice>({ tensor });
    }
}

void
Manager::destroy(std::shared_ptr<kp::Tensor> tensor)
{
    SPDLOG_DEBUG("Kompute Manager rebuild Tensor triggered");

    if (tensor->isInit()) {
        tensor->freeMemoryDestroyGPUResources();
    }

    // TODO: Confirm not limiting destroying tensors owned by this manager allowed
    std::set<std::shared_ptr<Tensor>>::iterator it =
      this->mManagedTensors.find(tensor);

    if (it != this->mManagedTensors.end()) {
        this->mManagedTensors.erase(tensor);
    }
}

void
Manager::destroy(std::vector<std::shared_ptr<kp::Tensor>> tensors)
{
    SPDLOG_DEBUG("Kompute Manager rebuild Tensor triggered");

    for (std::shared_ptr<Tensor> tensor : tensors) {
        this->destroy(tensor);
    }
}

void
Manager::destroy(std::vector<std::shared_ptr<kp::Sequence>> sequences)
{
    SPDLOG_DEBUG("Kompute Manager rebuild Sequence triggered");

    for (std::shared_ptr<kp::Sequence> sequence : sequences) {
        this->destroy(sequence);
    }
}

void
Manager::destroy(std::shared_ptr<kp::Sequence> sequence)
{
    SPDLOG_DEBUG("Kompute Manager rebuild Sequence triggered");

    // Inefficient but required to delete by value
    // Depending on the amount of named sequences created may be worth creating
    // a set to ensure efficient delete.
    for (std::unordered_map<std::string, std::shared_ptr<Sequence>>::iterator it = this->mManagedSequences.begin(); it != this->mManagedSequences.end(); it++) {
        if (it->second == sequence) {
            this->mManagedSequences.erase(it);
            break;
        }
    }

    if (sequence->isInit()) {
        sequence->freeMemoryDestroyGPUResources();
    }
}

void
Manager::destroy(const std::string& sequenceName)
{
    SPDLOG_DEBUG("Kompute Manager rebuild Sequence triggered");

    std::unordered_map<std::string, std::shared_ptr<Sequence>>::iterator
      found = this->mManagedSequences.find(sequenceName);

    if (found != this->mManagedSequences.end()) {
        // We don't call destroy(sequence) as erasing sequence by name more efficient
        if (found->second->isInit()) {
            found->second->freeMemoryDestroyGPUResources();
        }
        this->mManagedSequences.erase(sequenceName);
    }
}

void
Manager::destroy(const std::vector<std::string>& sequenceNames)
{
    SPDLOG_DEBUG("Kompute Manager rebuild Sequence triggered");

    for (const std::string& sequenceName : sequenceNames) {
        this->destroy(sequenceName);
    }
}


}
