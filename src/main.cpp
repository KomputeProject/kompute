#if defined(_WIN32)
#pragma comment(linker, "/subsystem:console")
#endif

// SPDLOG_ACTIVE_LEVEL must be defined before spdlog.h import
#if DEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#endif

#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include <spdlog/spdlog.h>
// ranges.h must come after spdlog.h
#include <spdlog/fmt/bundled/ranges.h>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>

#include "Manager.hpp"
#include "BaseOperator.hpp"

#define BUFFER_ELEMENTS 32

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

class VulkanCompute
{
  public:
    vk::Instance mInstance;
    vk::PhysicalDevice mPhysicalDevice;
    vk::Device mDevice;
    vk::Queue mComputeQueue;
    vk::DescriptorPool mDescriptorPool;
    vk::DescriptorSetLayout mDescriptorSetLayout;
    vk::PipelineLayout mPipelineLayout;
    vk::DescriptorSet mDescriptorSet;
    vk::PipelineCache mPipelineCache;
    vk::ShaderModule mShaderModule;
    vk::Pipeline mPipeline;
    vk::CommandPool mCommandPool;
    vk::CommandBuffer mCommandBuffer;

    uint32_t mComputeQueueFamilyIndex;

#if DEBUG
    vk::DebugReportCallbackEXT mDebugReportCallback;
    vk::DispatchLoaderDynamic mDebugDispatcher;
#endif

    void createBuffer(const vk::BufferUsageFlags& aUsageFlags,
                      const vk::MemoryPropertyFlags& aMemoryPropertyFlags,
                      vk::Buffer* aBuffer,
                      vk::DeviceMemory* aMemory,
                      vk::DeviceSize aSize,
                      void* data = nullptr) const
    {
        SPDLOG_DEBUG("Creating buffer: {}, {}, {}",
                     vk::to_string(aUsageFlags),
                     vk::to_string(aMemoryPropertyFlags),
                     aSize);

        vk::BufferCreateInfo bufferCreateInfo(vk::BufferCreateFlags(),
                                              aSize,
                                              aUsageFlags,
                                              vk::SharingMode::eExclusive);

        *aBuffer = this->mDevice.createBuffer(bufferCreateInfo);

        vk::PhysicalDeviceMemoryProperties deviceMemoryProperties =
          this->mPhysicalDevice.getMemoryProperties();

        vk::MemoryRequirements memReqs =
          this->mDevice.getBufferMemoryRequirements(*aBuffer);

        uint32_t memoryTypeIndex = -1;
        for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
            if (memReqs.memoryTypeBits & (1 << i)) {
                if ((deviceMemoryProperties.memoryTypes[i].propertyFlags &
                     aMemoryPropertyFlags) == aMemoryPropertyFlags) {
                    memoryTypeIndex = i;
                    break;
                }
            }
        }
        if (memoryTypeIndex < 0) {
            throw std::runtime_error(
              "Memory type index for buffer creation not found");
        }

        vk::MemoryAllocateInfo memoryAllocateInfo(memReqs.size,
                                                  memoryTypeIndex);

        *aMemory = this->mDevice.allocateMemory(memoryAllocateInfo);

        this->mDevice.bindBufferMemory(*aBuffer, *aMemory, 0);
    }

    VulkanCompute()
    {
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

        this->mInstance = vk::createInstance(computeInstanceCreateInfo);

#if DEBUG
        if (validLayerNames.size() > 0) {
            vk::DebugReportFlagsEXT debugFlags =
              vk::DebugReportFlagBitsEXT::eError |
              vk::DebugReportFlagBitsEXT::eWarning;
            vk::DebugReportCallbackCreateInfoEXT debugCreateInfo = {};
            debugCreateInfo.pfnCallback =
              (PFN_vkDebugReportCallbackEXT)debugMessageCallback;
            debugCreateInfo.flags = debugFlags;

            this->mDebugDispatcher.init(this->mInstance,
                                        &vkGetInstanceProcAddr);
            this->mDebugReportCallback =
              this->mInstance.createDebugReportCallbackEXT(
                debugCreateInfo, nullptr, this->mDebugDispatcher);
        }
#endif

        // Find device (currently only pick first device)
        {
            std::vector<vk::PhysicalDevice> physicalDevices =
              this->mInstance.enumeratePhysicalDevices();

            this->mPhysicalDevice = physicalDevices[0];

            vk::PhysicalDeviceProperties physicalDeviceProperties =
              this->mPhysicalDevice.getProperties();

            spdlog::info("Device {}", physicalDeviceProperties.deviceName);
        }

        {
            spdlog::info("Finding compute queue");
            // Find compute queue
            std::vector<vk::QueueFamilyProperties> allQueueFamilyProperties =
              this->mPhysicalDevice.getQueueFamilyProperties();

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

            this->mDevice =
              this->mPhysicalDevice.createDevice(deviceCreateInfo);

            this->mComputeQueue =
              this->mDevice.getQueue(this->mComputeQueueFamilyIndex, 0);
        }

        /*
                Create command pool
        */
        {
            vk::CommandPoolCreateInfo commandPoolInfo(
              vk::CommandPoolCreateFlags(), this->mComputeQueueFamilyIndex);

            this->mCommandPool =
              this->mDevice.createCommandPool(commandPoolInfo);
        }

        /*
                Prepare storage buffers
        */
        std::vector<uint32_t> computeInput(BUFFER_ELEMENTS);
        std::vector<uint32_t> computeOutput(BUFFER_ELEMENTS);

        // Fill input data
        uint32_t n = 0;
        std::generate(
          computeInput.begin(), computeInput.end(), [&n] { return n++; });

        const VkDeviceSize bufferSize = BUFFER_ELEMENTS * sizeof(uint32_t);

        vk::Buffer hostBuffer, deviceBuffer;
        vk::DeviceMemory hostMemory, deviceMemory;

        {
            createBuffer(vk::BufferUsageFlagBits::eTransferSrc |
                           vk::BufferUsageFlagBits::eTransferDst,
                         vk::MemoryPropertyFlagBits::eHostVisible,
                         &hostBuffer,
                         &hostMemory,
                         bufferSize);

            createBuffer(vk::BufferUsageFlagBits::eStorageBuffer |
                           vk::BufferUsageFlagBits::eTransferSrc |
                           vk::BufferUsageFlagBits::eTransferDst,
                         vk::MemoryPropertyFlagBits::eDeviceLocal,
                         &deviceBuffer,
                         &deviceMemory,
                         bufferSize);
        }

        /*
                Copy data to host memory
        */
        {
            void* mapped = this->mDevice.mapMemory(
              hostMemory, 0, bufferSize, vk::MemoryMapFlags());
            memcpy(mapped, computeInput.data(), bufferSize);
            vk::MappedMemoryRange mappedRange(hostMemory, 0, bufferSize);
            this->mDevice.flushMappedMemoryRanges(1, &mappedRange);
            this->mDevice.unmapMemory(hostMemory);
        }

        /*
                Copy data from host memory to staging buffer
        */
        {
            spdlog::info("Copying data from host memory to staging buffer");
            vk::CommandBufferAllocateInfo commandBufferAllocateInfo(
              this->mCommandPool, vk::CommandBufferLevel::ePrimary, 1);

            std::vector<vk::CommandBuffer> copyCommandBuffers =
              this->mDevice.allocateCommandBuffers(commandBufferAllocateInfo);

            vk::CommandBuffer copyCommandBuffer = copyCommandBuffers[0];

            copyCommandBuffer.begin(vk::CommandBufferBeginInfo());
            {
                vk::BufferCopy copyRegion(0, 0, bufferSize);
                copyCommandBuffer.copyBuffer(
                  hostBuffer, deviceBuffer, copyRegion);
            }
            copyCommandBuffer.end();

            const vk::PipelineStageFlags waitStageMask =
              vk::PipelineStageFlagBits::eTransfer;

            vk::SubmitInfo submitInfo(
              0, nullptr, &waitStageMask, 1, &copyCommandBuffer);
            vk::Fence fence = this->mDevice.createFence(vk::FenceCreateInfo());

            this->mComputeQueue.submit(1, &submitInfo, fence);
            this->mDevice.waitForFences(1, &fence, VK_TRUE, UINT64_MAX);

            this->mDevice.destroy(fence);
            this->mDevice.freeCommandBuffers(
              this->mCommandPool, 1, &copyCommandBuffer);
        }

        {
            std::vector<vk::DescriptorPoolSize> poolSizes = {
                vk::DescriptorPoolSize(vk::DescriptorType::eStorageBuffer, 1)
            };

            vk::DescriptorPoolCreateInfo descriptorPoolInfo(
              vk::DescriptorPoolCreateFlags(),
              1,
              static_cast<uint32_t>(poolSizes.size()),
              poolSizes.data());

            this->mDescriptorPool =
              this->mDevice.createDescriptorPool(descriptorPoolInfo);

            std::vector<vk::DescriptorSetLayoutBinding> setLayoutBindings = {
                vk::DescriptorSetLayoutBinding(
                  0,
                  vk::DescriptorType::eStorageBuffer,
                  1,
                  vk::ShaderStageFlagBits::eCompute)
            };

            vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutInfo(
              vk::DescriptorSetLayoutCreateFlags(),
              static_cast<uint32_t>(setLayoutBindings.size()),
              setLayoutBindings.data());

            this->mDescriptorSetLayout =
              this->mDevice.createDescriptorSetLayout(descriptorSetLayoutInfo);

            // For simplicity we don't create an array and pass a single
            // descriptorSetLayout
            vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo(
              vk::PipelineLayoutCreateFlags(), 1, &this->mDescriptorSetLayout);

            this->mPipelineLayout =
              this->mDevice.createPipelineLayout(pipelineLayoutCreateInfo);

            vk::DescriptorSetAllocateInfo descriptorSetAllocateInfo(
              this->mDescriptorPool, 1, &this->mDescriptorSetLayout);

            std::vector<vk::DescriptorSet> descriptorSets =
              this->mDevice.allocateDescriptorSets(descriptorSetAllocateInfo);
            this->mDescriptorSet = descriptorSets[0];

            vk::DescriptorBufferInfo descriptorBufferInfo(
              deviceBuffer, 0, VK_WHOLE_SIZE);

            std::vector<vk::WriteDescriptorSet> computeWriteDescriptorSets = {
                vk::WriteDescriptorSet(this->mDescriptorSet,
                                       0,
                                       0,
                                       1,
                                       vk::DescriptorType::eStorageBuffer,
                                       nullptr,
                                       &descriptorBufferInfo)
            };

            this->mDevice.updateDescriptorSets(computeWriteDescriptorSets,
                                               nullptr);
        }

        {
            struct SpecializationData
            {
                uint32_t BUFFER_ELEMENT_COUNT = BUFFER_ELEMENTS;
            } specializationData;

            vk::SpecializationMapEntry specializationMapEntry(
              0, 0, sizeof(SpecializationData));
            vk::SpecializationInfo specializationInfo(
              1,
              &specializationMapEntry,
              sizeof(SpecializationData),
              &specializationData);

            const std::string shadersPath = "shaders/glsl/";
            const std::string shaderFilePath =
              shadersPath + "computeheadless.comp.spv";
            spdlog::info("Shader file path: {}", shaderFilePath);

            SPDLOG_DEBUG("Reading file");
            std::ifstream fileStream(
              shaderFilePath, std::ios::binary | std::ios::in | std::ios::ate);

            size_t shaderFileSize = fileStream.tellg();
            fileStream.seekg(0, std::ios::beg);
            char* shaderFileData = new char[shaderFileSize];
            fileStream.read(shaderFileData, shaderFileSize);
            fileStream.close();

            SPDLOG_DEBUG("Converting the read file into module");
            vk::ShaderModuleCreateInfo shaderModuleInfo(
              vk::ShaderModuleCreateFlags(),
              shaderFileSize,
              (uint32_t*)shaderFileData);

            this->mShaderModule =
              this->mDevice.createShaderModule(shaderModuleInfo);

            SPDLOG_DEBUG("Converting to shader stage");
            vk::PipelineShaderStageCreateInfo shaderStage(
              vk::PipelineShaderStageCreateFlags(),
              vk::ShaderStageFlagBits::eCompute,
              this->mShaderModule,
              "main",
              &specializationInfo);

            vk::PipelineCacheCreateInfo pipelineCacheCreateInfo;
            this->mPipelineCache =
              this->mDevice.createPipelineCache(pipelineCacheCreateInfo);

            vk::ComputePipelineCreateInfo computePipelineCreateInfo(
              vk::PipelineCreateFlags(),
              shaderStage,
              this->mPipelineLayout,
              vk::Pipeline(),
              0);

            vk::ResultValue<vk::Pipeline> pipelineResult =
              this->mDevice.createComputePipeline(this->mPipelineCache,
                                                  computePipelineCreateInfo);

            if (pipelineResult.result != vk::Result::eSuccess) {
                throw std::runtime_error("Failed to create pipeline result: " +
                                         vk::to_string(pipelineResult.result));
            }
            this->mPipeline = pipelineResult.value;
        }

        {
            vk::CommandBufferAllocateInfo cmdBufferAllocInfo(
              this->mCommandPool, vk::CommandBufferLevel::ePrimary, 1);

            std::vector<vk::CommandBuffer> cmdBuffers =
              this->mDevice.allocateCommandBuffers(cmdBufferAllocInfo);

            this->mCommandBuffer = cmdBuffers[0];
            this->mCommandBuffer.begin(vk::CommandBufferBeginInfo());
            {
                // Barrier to ensure input transfer is finished before compute
                // shader reads from it
                vk::BufferMemoryBarrier bufferMemoryBarrier;
                bufferMemoryBarrier.buffer = deviceBuffer;
                bufferMemoryBarrier.size = VK_WHOLE_SIZE;
                bufferMemoryBarrier.srcAccessMask =
                  vk::AccessFlagBits::eHostWrite;
                bufferMemoryBarrier.dstAccessMask =
                  vk::AccessFlagBits::eShaderRead;
                bufferMemoryBarrier.srcQueueFamilyIndex =
                  VK_QUEUE_FAMILY_IGNORED;
                bufferMemoryBarrier.dstQueueFamilyIndex =
                  VK_QUEUE_FAMILY_IGNORED;

                this->mCommandBuffer.pipelineBarrier(
                  vk::PipelineStageFlagBits::eHost,
                  vk::PipelineStageFlagBits::eComputeShader,
                  vk::DependencyFlags(),
                  nullptr,
                  bufferMemoryBarrier,
                  nullptr);
                this->mCommandBuffer.bindPipeline(
                  vk::PipelineBindPoint::eCompute, this->mPipeline);
                this->mCommandBuffer.bindDescriptorSets(
                  vk::PipelineBindPoint::eCompute,
                  this->mPipelineLayout,
                  0,
                  this->mDescriptorSet,
                  nullptr);

                this->mCommandBuffer.dispatch(BUFFER_ELEMENTS / 4, 1, 1);

                // Barrier to ensure that shader writes are finished before
                // buffer is read back from GPU
                bufferMemoryBarrier.srcAccessMask =
                  vk::AccessFlagBits::eShaderWrite;
                bufferMemoryBarrier.dstAccessMask =
                  vk::AccessFlagBits::eTransferRead;
                this->mCommandBuffer.pipelineBarrier(
                  vk::PipelineStageFlagBits::eComputeShader,
                  vk::PipelineStageFlagBits::eTransfer,
                  vk::DependencyFlags(),
                  nullptr,
                  bufferMemoryBarrier,
                  nullptr);

                // Read back to host visible buffer
                vk::BufferCopy copyRegion(0, 0, bufferSize);
                this->mCommandBuffer.copyBuffer(
                  deviceBuffer, hostBuffer, copyRegion);

                // Barrier to ensure that buffer copy is finished before host
                // reading from it
                bufferMemoryBarrier.srcAccessMask =
                  vk::AccessFlagBits::eTransferWrite;
                bufferMemoryBarrier.dstAccessMask =
                  vk::AccessFlagBits::eHostRead;
                bufferMemoryBarrier.buffer = hostBuffer;
                this->mCommandBuffer.pipelineBarrier(
                  vk::PipelineStageFlagBits::eTransfer,
                  vk::PipelineStageFlagBits::eHost,
                  vk::DependencyFlags(),
                  nullptr,
                  bufferMemoryBarrier,
                  nullptr);
            }
            this->mCommandBuffer.end();
        }

        {
            vk::Fence fence = this->mDevice.createFence(vk::FenceCreateInfo());

            const vk::PipelineStageFlags waitStageMask =
              vk::PipelineStageFlagBits::eTransfer;

            vk::SubmitInfo computeSubmitInfo(
              0, nullptr, &waitStageMask, 1, &this->mCommandBuffer);

            this->mComputeQueue.submit(computeSubmitInfo, fence);

            this->mDevice.waitForFences(fence, VK_TRUE, UINT64_MAX);
            this->mDevice.destroy(fence);
        }

        {
            // Make device writes visible to host
            void* mapped = this->mDevice.mapMemory(
              hostMemory, 0, VK_WHOLE_SIZE, vk::MemoryMapFlags());
            vk::MappedMemoryRange mappedMemoryRange(
              hostMemory, 0, VK_WHOLE_SIZE);
            this->mDevice.invalidateMappedMemoryRanges(mappedMemoryRange);
            memcpy(computeOutput.data(), mapped, bufferSize);
            this->mDevice.unmapMemory(hostMemory);
        }

        {
            this->mComputeQueue.waitIdle();

            spdlog::info("Compute input: {}", computeInput);
            spdlog::info("Compute output: {}", computeOutput);
        }

        {
            this->mDevice.destroy(deviceBuffer);
            this->mDevice.freeMemory(deviceMemory);
            this->mDevice.destroy(hostBuffer);
            this->mDevice.freeMemory(hostMemory);
        }
    }

    ~VulkanCompute()
    {
        this->mDevice.destroy(this->mPipelineLayout);
        this->mDevice.destroy(this->mDescriptorSetLayout);
        this->mDevice.destroy(this->mDescriptorPool);
        this->mDevice.destroy(this->mPipeline);
        this->mDevice.destroy(this->mPipelineCache);
        this->mDevice.destroy(this->mCommandPool);
        this->mDevice.destroy(this->mShaderModule);
        this->mDevice.destroy();

#if DEBUG
        if (this->mDebugReportCallback) {
            this->mInstance.destroyDebugReportCallbackEXT(
              this->mDebugReportCallback, nullptr, this->mDebugDispatcher);
        }
#endif
        this->mInstance.destroy();
    }
};

int
main()
{
#if DEBUG
    spdlog::set_level(spdlog::level::debug);
#else
    spdlog::set_level(spdlog::level::info);
#endif

    try {
        //VulkanCompute* vulkanExample = new VulkanCompute();
        //spdlog::info("Finished.");
        //delete (vulkanExample);

        // Run Kompute
        spdlog::info("Creating manager");
        kp::Manager mgr;
        spdlog::info("Calling manager eval");
        mgr.eval<kp::BaseOperator>("one", "two");
        spdlog::info("Called manager eval success");
        return 0;
    } catch (const std::exception& exc) {
        spdlog::error(exc.what());
        return 1;
    }
}
