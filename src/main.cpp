#if defined(_WIN32)
#pragma comment(linker, "/subsystem:console")
#endif

// SPDLOG_ACTIVE_LEVEL must be defined before spdlog.h import
#if DEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#endif

#include <algorithm>
#include <assert.h>
#include <iostream>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include <spdlog/spdlog.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>

#include "VulkanTools.h"

#define BUFFER_ELEMENTS 32

#define LOG(...) printf(__VA_ARGS__)

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
    LOG("[VALIDATION]: %s - %s\n", pLayerPrefix, pMessage);
    return VK_FALSE;
}

class VulkanCompute
{
  public:
    vk::Instance mInstance;
    vk::PhysicalDevice mPhysicalDevice;
    vk::Device mDevice;
    vk::Queue mComputeQueue;

    uint32_t mComputeQueueFamilyIndex;

    vk::DebugReportCallbackEXT mDebugReportCallback{};

    void createBuffer(const vk::BufferUsageFlags& aUsageFlags,
                      const vk::MemoryPropertyFlags& aMemoryPropertyFlags,
                      vk::Buffer* aBuffer,
                      vk::DeviceMemory* aMemory,
                      vk::DeviceSize aSize,
                      void* aData = nullptr) const
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
        for (uint32_t i = 0; i < 32; i++) {
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

        if (aData != nullptr) {
            vk::DeviceSize offset = 0;
            void* mapped = this->mDevice.mapMemory(
              *aMemory, offset, aSize, vk::MemoryMapFlags());
            memcpy(mapped, aData, aSize);
            this->mDevice.unmapMemory(*aMemory);
        }

        this->mDevice.bindBufferMemory(*aBuffer, *aMemory, 0);
    }

    /*
     * C API
     */
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    // uint32_t queueFamilyIndex;
    VkPipelineCache pipelineCache;
    VkQueue queue;
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
    VkFence fence;
    VkDescriptorPool descriptorPool;
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorSet descriptorSet;
    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;
    VkShaderModule shaderModule;

    // VkDebugReportCallbackEXT debugReportCallback{};

    // C API Function
    VkResult createBuffer(VkBufferUsageFlags usageFlags,
                          VkMemoryPropertyFlags memoryPropertyFlags,
                          VkBuffer* buffer,
                          VkDeviceMemory* memory,
                          VkDeviceSize size,
                          void* data = nullptr)
    {
        // Create the buffer handle
        VkBufferCreateInfo bufferCreateInfo =
          vks::initializers::bufferCreateInfo(usageFlags, size);

        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        VK_CHECK_RESULT(
          vkCreateBuffer(this->device, &bufferCreateInfo, nullptr, buffer));

        // Create the memory backing up the buffer handle
        VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
        vkGetPhysicalDeviceMemoryProperties(this->physicalDevice,
                                            &deviceMemoryProperties);
        VkMemoryRequirements memReqs;
        VkMemoryAllocateInfo memAlloc = vks::initializers::memoryAllocateInfo();
        vkGetBufferMemoryRequirements(this->device, *buffer, &memReqs);
        memAlloc.allocationSize = memReqs.size;

        // Find a memory type index that fits the properties of the buffer
        bool memTypeFound = false;
        for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
            if ((memReqs.memoryTypeBits & 1) == 1) {
                if ((deviceMemoryProperties.memoryTypes[i].propertyFlags &
                     memoryPropertyFlags) == memoryPropertyFlags) {
                    memAlloc.memoryTypeIndex = i;
                    memTypeFound = true;
                }
            }
            memReqs.memoryTypeBits >>= 1;
        }
        assert(memTypeFound);
        VK_CHECK_RESULT(
          vkAllocateMemory(this->device, &memAlloc, nullptr, memory));

        if (data != nullptr) {
            void* mapped;
            VK_CHECK_RESULT(
              vkMapMemory(this->device, *memory, 0, size, 0, &mapped));
            memcpy(mapped, data, size);
            vkUnmapMemory(this->device, *memory);
        }

        VK_CHECK_RESULT(vkBindBufferMemory(this->device, *buffer, *memory, 0));

        return VK_SUCCESS;
    }

    VulkanCompute()
    {
        LOG("Running headless compute example\n");

        vk::ApplicationInfo applicationInfo;
        applicationInfo.pApplicationName = "Vulkan compute";
        applicationInfo.pEngineName = "VulkanCompute";
        applicationInfo.apiVersion = VK_API_VERSION_1_0;

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

            vk::DispatchLoaderDynamic dispatcher;
            dispatcher.init(this->mInstance, &vkGetInstanceProcAddr);
            this->mDebugReportCallback =
              this->mInstance.createDebugReportCallbackEXT(
                debugCreateInfo, nullptr, dispatcher);
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

        //        /*
        //                C API Vulkan
        //        */

        {
            spdlog::info("Allocating rest of components for backwards compat");
            this->instance = static_cast<VkInstance>(this->mInstance);
            this->physicalDevice = this->mPhysicalDevice;
            this->device = this->mDevice;
            this->queue = this->mComputeQueue;
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
                         bufferSize,
                         computeInput.data());

            createBuffer(vk::BufferUsageFlagBits::eStorageBuffer |
                           vk::BufferUsageFlagBits::eTransferSrc |
                           vk::BufferUsageFlagBits::eTransferDst,
                         vk::MemoryPropertyFlagBits::eDeviceLocal,
                         &deviceBuffer,
                         &deviceMemory,
                         bufferSize);
        }

        /*
                Prepare compute this->pipeline
        */
        {
            std::vector<VkDescriptorPoolSize> poolSizes = {
                vks::initializers::descriptorPoolSize(
                  VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1),
            };

            VkDescriptorPoolCreateInfo descriptorPoolInfo =
              vks::initializers::descriptorPoolCreateInfo(
                static_cast<uint32_t>(poolSizes.size()), poolSizes.data(), 1);
            VK_CHECK_RESULT(vkCreateDescriptorPool(this->device,
                                                   &descriptorPoolInfo,
                                                   nullptr,
                                                   &this->descriptorPool));

            std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings = {
                vks::initializers::descriptorSetLayoutBinding(
                  VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                  VK_SHADER_STAGE_COMPUTE_BIT,
                  0),
            };
            VkDescriptorSetLayoutCreateInfo descriptorLayout =
              vks::initializers::descriptorSetLayoutCreateInfo(
                setLayoutBindings);
            VK_CHECK_RESULT(
              vkCreateDescriptorSetLayout(this->device,
                                          &descriptorLayout,
                                          nullptr,
                                          &this->descriptorSetLayout));

            VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo =
              vks::initializers::pipelineLayoutCreateInfo(
                &this->descriptorSetLayout, 1);
            VK_CHECK_RESULT(vkCreatePipelineLayout(this->device,
                                                   &pipelineLayoutCreateInfo,
                                                   nullptr,
                                                   &this->pipelineLayout));

            VkDescriptorSetAllocateInfo allocInfo =
              vks::initializers::descriptorSetAllocateInfo(
                this->descriptorPool, &this->descriptorSetLayout, 1);
            VK_CHECK_RESULT(vkAllocateDescriptorSets(
              this->device, &allocInfo, &this->descriptorSet));

            VkDescriptorBufferInfo bufferDescriptor = { deviceBuffer,
                                                        0,
                                                        VK_WHOLE_SIZE };
            std::vector<VkWriteDescriptorSet> computeWriteDescriptorSets = {
                vks::initializers::writeDescriptorSet(
                  this->descriptorSet,
                  VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                  0,
                  &bufferDescriptor),
            };
            vkUpdateDescriptorSets(
              this->device,
              static_cast<uint32_t>(computeWriteDescriptorSets.size()),
              computeWriteDescriptorSets.data(),
              0,
              NULL);

            VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
            pipelineCacheCreateInfo.sType =
              VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
            VK_CHECK_RESULT(vkCreatePipelineCache(this->device,
                                                  &pipelineCacheCreateInfo,
                                                  nullptr,
                                                  &this->pipelineCache));

            // Create this->pipeline
            VkComputePipelineCreateInfo computePipelineCreateInfo =
              vks::initializers::computePipelineCreateInfo(this->pipelineLayout,
                                                           0);

            // Pass SSBO size via specialization constant
            struct SpecializationData
            {
                uint32_t BUFFER_ELEMENT_COUNT = BUFFER_ELEMENTS;
            } specializationData;

            VkSpecializationMapEntry specializationMapEntry =
              vks::initializers::specializationMapEntry(0, 0, sizeof(uint32_t));

            VkSpecializationInfo specializationInfo =
              vks::initializers::specializationInfo(1,
                                                    &specializationMapEntry,
                                                    sizeof(SpecializationData),
                                                    &specializationData);

            // TODO: There is no command line arguments parsing (nor Android
            // settings) for this example, so we have no way of picking between
            // GLSL or HLSL shaders. Hard-code to glsl for now.
            const std::string shadersPath = getAssetPath() + "shaders/glsl/";
            std::cout << "Shader path: " << shadersPath << std::endl;

            VkPipelineShaderStageCreateInfo shaderStage = {};
            shaderStage.sType =
              VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStage.stage = VK_SHADER_STAGE_COMPUTE_BIT;

            shaderStage.module = vks::tools::loadShader(
              (shadersPath + "computeheadless.comp.spv").c_str(), this->device);

            shaderStage.pName = "main";
            shaderStage.pSpecializationInfo = &specializationInfo;
            this->shaderModule = shaderStage.module;

            assert(shaderStage.module != VK_NULL_HANDLE);
            computePipelineCreateInfo.stage = shaderStage;
            VK_CHECK_RESULT(vkCreateComputePipelines(this->device,
                                                     this->pipelineCache,
                                                     1,
                                                     &computePipelineCreateInfo,
                                                     nullptr,
                                                     &this->pipeline));

            // Compute command pool
            VkCommandPoolCreateInfo cmdPoolInfo = {};
            cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            cmdPoolInfo.queueFamilyIndex = this->mComputeQueueFamilyIndex;
            cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            VK_CHECK_RESULT(vkCreateCommandPool(
              this->device, &cmdPoolInfo, nullptr, &this->commandPool));

            // Create a command buffer for compute operations
            VkCommandBufferAllocateInfo cmdBufAllocateInfo =
              vks::initializers::commandBufferAllocateInfo(
                this->commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);
            VK_CHECK_RESULT(vkAllocateCommandBuffers(
              this->device, &cmdBufAllocateInfo, &this->commandBuffer));

            // Fence for compute CB sync
            VkFenceCreateInfo fenceCreateInfo =
              vks::initializers::fenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
            VK_CHECK_RESULT(vkCreateFence(
              this->device, &fenceCreateInfo, nullptr, &this->fence));
        }

        {
            // Flush writes to host visible buffer
            void* mapped;
            vkMapMemory(this->device, hostMemory, 0, VK_WHOLE_SIZE, 0, &mapped);
            VkMappedMemoryRange mappedRange =
              vks::initializers::mappedMemoryRange();
            mappedRange.memory = hostMemory;
            mappedRange.offset = 0;
            mappedRange.size = VK_WHOLE_SIZE;
            vkFlushMappedMemoryRanges(this->device, 1, &mappedRange);
            vkUnmapMemory(this->device, hostMemory);
        }

        /*
                Command buffer creation (for compute work submission)
        */
        {
            VkCommandBufferBeginInfo cmdBufInfo =
              vks::initializers::commandBufferBeginInfo();

            VK_CHECK_RESULT(
              vkBeginCommandBuffer(this->commandBuffer, &cmdBufInfo));

            VkBufferCopy copyRegion = {};
            copyRegion.size = bufferSize;
            vkCmdCopyBuffer(
              this->commandBuffer, hostBuffer, deviceBuffer, 1, &copyRegion);

            // Barrier to ensure that input buffer transfer is finished before
            // compute shader reads from it
            VkBufferMemoryBarrier bufferBarrier =
              vks::initializers::bufferMemoryBarrier();
            bufferBarrier.buffer = deviceBuffer;
            bufferBarrier.size = VK_WHOLE_SIZE;
            bufferBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
            bufferBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            bufferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            bufferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

            vkCmdPipelineBarrier(this->commandBuffer,
                                 VK_PIPELINE_STAGE_HOST_BIT,
                                 VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                 VK_FLAGS_NONE,
                                 0,
                                 nullptr,
                                 1,
                                 &bufferBarrier,
                                 0,
                                 nullptr);

            vkCmdBindPipeline(this->commandBuffer,
                              VK_PIPELINE_BIND_POINT_COMPUTE,
                              this->pipeline);

            vkCmdBindDescriptorSets(this->commandBuffer,
                                    VK_PIPELINE_BIND_POINT_COMPUTE,
                                    this->pipelineLayout,
                                    0,
                                    1,
                                    &this->descriptorSet,
                                    0,
                                    0);

            vkCmdDispatch(this->commandBuffer, BUFFER_ELEMENTS / 4, 1, 1);

            // Barrier to ensure that shader writes are finished before buffer
            // is read back from GPU
            bufferBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
            bufferBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            bufferBarrier.buffer = deviceBuffer;
            bufferBarrier.size = VK_WHOLE_SIZE;
            bufferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            bufferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

            vkCmdPipelineBarrier(this->commandBuffer,
                                 VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 VK_FLAGS_NONE,
                                 0,
                                 nullptr,
                                 1,
                                 &bufferBarrier,
                                 0,
                                 nullptr);

            // Read back to host visible buffer
            copyRegion = {};
            copyRegion.size = bufferSize;
            vkCmdCopyBuffer(
              this->commandBuffer, deviceBuffer, hostBuffer, 1, &copyRegion);

            // Barrier to ensure that buffer copy is finished before host
            // reading from it
            bufferBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            bufferBarrier.dstAccessMask = VK_ACCESS_HOST_READ_BIT;
            bufferBarrier.buffer = hostBuffer;
            bufferBarrier.size = VK_WHOLE_SIZE;
            bufferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            bufferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

            vkCmdPipelineBarrier(this->commandBuffer,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 VK_PIPELINE_STAGE_HOST_BIT,
                                 VK_FLAGS_NONE,
                                 0,
                                 nullptr,
                                 1,
                                 &bufferBarrier,
                                 0,
                                 nullptr);

            VK_CHECK_RESULT(vkEndCommandBuffer(this->commandBuffer));

            // Submit compute work
            vkResetFences(this->device, 1, &this->fence);
            const VkPipelineStageFlags waitStageMask =
              VK_PIPELINE_STAGE_TRANSFER_BIT;
            VkSubmitInfo computeSubmitInfo = vks::initializers::submitInfo();
            computeSubmitInfo.pWaitDstStageMask = &waitStageMask;
            computeSubmitInfo.commandBufferCount = 1;
            computeSubmitInfo.pCommandBuffers = &this->commandBuffer;
            VK_CHECK_RESULT(
              vkQueueSubmit(this->queue, 1, &computeSubmitInfo, this->fence));
            VK_CHECK_RESULT(vkWaitForFences(
              this->device, 1, &this->fence, VK_TRUE, UINT64_MAX));

            // Make this->device writes visible to the host
            void* mapped;
            vkMapMemory(this->device, hostMemory, 0, VK_WHOLE_SIZE, 0, &mapped);
            VkMappedMemoryRange mappedRange =
              vks::initializers::mappedMemoryRange();
            mappedRange.memory = hostMemory;
            mappedRange.offset = 0;
            mappedRange.size = VK_WHOLE_SIZE;
            vkInvalidateMappedMemoryRanges(this->device, 1, &mappedRange);

            // Copy to output
            memcpy(computeOutput.data(), mapped, bufferSize);
            vkUnmapMemory(this->device, hostMemory);
        }

        vkQueueWaitIdle(this->queue);

        // Output buffer contents
        LOG("Compute input:\n");
        for (auto v : computeInput) {
            LOG("%d 	", v);
        }
        std::cout << std::endl;

        LOG("Compute output:\n");
        for (auto v : computeOutput) {
            LOG("%d 	", v);
        }
        std::cout << std::endl;

        // Clean up
        vkDestroyBuffer(this->device, deviceBuffer, nullptr);
        vkFreeMemory(this->device, deviceMemory, nullptr);
        vkDestroyBuffer(this->device, hostBuffer, nullptr);
        vkFreeMemory(this->device, hostMemory, nullptr);
    }

    ~VulkanCompute()
    {
        vkDestroyPipelineLayout(this->device, this->pipelineLayout, nullptr);
        vkDestroyDescriptorSetLayout(
          this->device, this->descriptorSetLayout, nullptr);
        vkDestroyDescriptorPool(this->device, this->descriptorPool, nullptr);
        vkDestroyPipeline(this->device, this->pipeline, nullptr);
        vkDestroyPipelineCache(this->device, this->pipelineCache, nullptr);
        vkDestroyFence(this->device, this->fence, nullptr);
        vkDestroyCommandPool(this->device, this->commandPool, nullptr);
        vkDestroyShaderModule(this->device, this->shaderModule, nullptr);
        vkDestroyDevice(this->device, nullptr);
#if DEBUG
        if (this->mDebugReportCallback) {
            PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallback =
              reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(
                vkGetInstanceProcAddr(this->instance,
                                      "vkDestroyDebugReportCallbackEXT"));
            assert(vkDestroyDebugReportCallback);
            vkDestroyDebugReportCallback(
              this->instance, this->mDebugReportCallback, nullptr);
        }
#endif
        vkDestroyInstance(this->instance, nullptr);
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
    VulkanCompute* vulkanExample = new VulkanCompute();
    std::cout << "Finished.";
    delete (vulkanExample);
    return 0;
}
