#if defined(_WIN32)
#pragma comment(linker, "/subsystem:console")
#endif

#include <algorithm>
#include <assert.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "VulkanTools.h"
#include <vulkan/vulkan.h>

#define DEBUG (!NDEBUG)

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

class VulkanExample
{
  public:
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    uint32_t queueFamilyIndex;
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

    VkDebugReportCallbackEXT debugReportCallback{};

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
        VK_CHECK_RESULT(vkAllocateMemory(this->device, &memAlloc, nullptr, memory));

        if (data != nullptr) {
            void* mapped;
            VK_CHECK_RESULT(vkMapMemory(this->device, *memory, 0, size, 0, &mapped));
            memcpy(mapped, data, size);
            vkUnmapMemory(this->device, *memory);
        }

        VK_CHECK_RESULT(vkBindBufferMemory(this->device, *buffer, *memory, 0));

        return VK_SUCCESS;
    }

    VulkanExample()
    {
        LOG("Running headless compute example\n");

        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Vulkan headless example";
        appInfo.pEngineName = "VulkanExample";
        appInfo.apiVersion = VK_API_VERSION_1_0;

        /*
                Vulkan this->instance creation (without surface extensions)
        */
        VkInstanceCreateInfo instanceCreateInfo = {};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pApplicationInfo = &appInfo;

        uint32_t layerCount = 0;
        const char* validationLayers[] = {
            "VK_LAYER_LUNARG_standard_validation"
        };
        layerCount = 1;
#if DEBUG
        // Check if layers are available
        uint32_t instanceLayerCount;
        vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);
        std::vector<VkLayerProperties> instanceLayers(instanceLayerCount);
        vkEnumerateInstanceLayerProperties(&instanceLayerCount,
                                           instanceLayers.data());

        bool layersAvailable = true;
        for (auto layerName : validationLayers) {
            bool layerAvailable = false;
            for (auto instanceLayer : instanceLayers) {
                if (strcmp(instanceLayer.layerName, layerName) == 0) {
                    layerAvailable = true;
                    break;
                }
            }
            if (!layerAvailable) {
                layersAvailable = false;
                break;
            }
        }

        if (layersAvailable) {
            instanceCreateInfo.ppEnabledLayerNames = validationLayers;
            const char* validationExt = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
            instanceCreateInfo.enabledLayerCount = layerCount;
            instanceCreateInfo.enabledExtensionCount = 1;
            instanceCreateInfo.ppEnabledExtensionNames = &validationExt;
        }
#endif
        VK_CHECK_RESULT(
          vkCreateInstance(&instanceCreateInfo, nullptr, &this->instance));

#if DEBUG
        if (layersAvailable) {
            VkDebugReportCallbackCreateInfoEXT debugReportCreateInfo = {};
            debugReportCreateInfo.sType =
              VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
            debugReportCreateInfo.flags =
              VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
            debugReportCreateInfo.pfnCallback =
              (PFN_vkDebugReportCallbackEXT)debugMessageCallback;

            // We have to explicitly load this function.
            PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT =
              reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(
                vkGetInstanceProcAddr(this->instance,
                                      "vkCreateDebugReportCallbackEXT"));
            assert(vkCreateDebugReportCallbackEXT);
            VK_CHECK_RESULT(vkCreateDebugReportCallbackEXT(
              this->instance, &debugReportCreateInfo, nullptr, &debugReportCallback));
        }
#endif

        /*
                Vulkan this->device creation
        */
        // Physical this->device (always use first)
        uint32_t deviceCount = 0;
        VK_CHECK_RESULT(
          vkEnumeratePhysicalDevices(this->instance, &deviceCount, nullptr));

        std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
        VK_CHECK_RESULT(vkEnumeratePhysicalDevices(
          this->instance, &deviceCount, physicalDevices.data()));

        this->physicalDevice = physicalDevices[0];

        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(this->physicalDevice, &deviceProperties);
        LOG("GPU: %s\n", deviceProperties.deviceName);

        // Request a single compute this->queue
        const float defaultQueuePriority(0.0f);
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        uint32_t queueFamilyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(
          this->physicalDevice, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilyProperties(
          queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(
          this->physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

        for (uint32_t i = 0;
             i < static_cast<uint32_t>(queueFamilyProperties.size());
             i++) {
            if (queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
                queueFamilyIndex = i;
                queueCreateInfo.sType =
                  VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queueCreateInfo.queueFamilyIndex = i;
                queueCreateInfo.queueCount = 1;
                queueCreateInfo.pQueuePriorities = &defaultQueuePriority;
                break;
            }
        }
        // Create logical this->device
        VkDeviceCreateInfo deviceCreateInfo = {};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.queueCreateInfoCount = 1;
        deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
        VK_CHECK_RESULT(vkCreateDevice(
          this->physicalDevice, &deviceCreateInfo, nullptr, &this->device));

        // Get a compute this->queue
        vkGetDeviceQueue(this->device, this->queueFamilyIndex, 0, &this->queue);

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

        VkBuffer deviceBuffer, hostBuffer;
        VkDeviceMemory deviceMemory, hostMemory;

        // Copy input data to VRAM using a staging buffer
        {
            createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
                           VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                         &hostBuffer,
                         &hostMemory,
                         bufferSize,
                         computeInput.data());

            createBuffer(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                           VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
                           VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
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
            VK_CHECK_RESULT(vkCreateDescriptorPool(
              this->device, &descriptorPoolInfo, nullptr, &this->descriptorPool));

            std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings = {
                vks::initializers::descriptorSetLayoutBinding(
                  VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                  VK_SHADER_STAGE_COMPUTE_BIT,
                  0),
            };
            VkDescriptorSetLayoutCreateInfo descriptorLayout =
              vks::initializers::descriptorSetLayoutCreateInfo(
                setLayoutBindings);
            VK_CHECK_RESULT(vkCreateDescriptorSetLayout(
              this->device, &descriptorLayout, nullptr, &this->descriptorSetLayout));

            VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo =
              vks::initializers::pipelineLayoutCreateInfo(&this->descriptorSetLayout,
                                                          1);
            VK_CHECK_RESULT(vkCreatePipelineLayout(
              this->device, &pipelineLayoutCreateInfo, nullptr, &this->pipelineLayout));

            VkDescriptorSetAllocateInfo allocInfo =
              vks::initializers::descriptorSetAllocateInfo(
                this->descriptorPool, &this->descriptorSetLayout, 1);
            VK_CHECK_RESULT(
              vkAllocateDescriptorSets(this->device, &allocInfo, &this->descriptorSet));

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
            cmdPoolInfo.queueFamilyIndex = this->queueFamilyIndex;
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
            VK_CHECK_RESULT(
              vkCreateFence(this->device, &fenceCreateInfo, nullptr, &this->fence));
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

            VK_CHECK_RESULT(vkBeginCommandBuffer(this->commandBuffer, &cmdBufInfo));

            VkBufferCopy copyRegion = {};
            copyRegion.size = bufferSize;
            vkCmdCopyBuffer(this->commandBuffer, hostBuffer, deviceBuffer, 1, &copyRegion);

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

            vkCmdBindPipeline(
              this->commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, this->pipeline);

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
            VK_CHECK_RESULT(vkQueueSubmit(this->queue, 1, &computeSubmitInfo, this->fence));
            VK_CHECK_RESULT(
              vkWaitForFences(this->device, 1, &this->fence, VK_TRUE, UINT64_MAX));

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

    ~VulkanExample()
    {
        vkDestroyPipelineLayout(this->device, this->pipelineLayout, nullptr);
        vkDestroyDescriptorSetLayout(this->device, this->descriptorSetLayout, nullptr);
        vkDestroyDescriptorPool(this->device, this->descriptorPool, nullptr);
        vkDestroyPipeline(this->device, this->pipeline, nullptr);
        vkDestroyPipelineCache(this->device, this->pipelineCache, nullptr);
        vkDestroyFence(this->device, this->fence, nullptr);
        vkDestroyCommandPool(this->device, this->commandPool, nullptr);
        vkDestroyShaderModule(this->device, this->shaderModule, nullptr);
        vkDestroyDevice(this->device, nullptr);
#if DEBUG
        if (debugReportCallback) {
            PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallback =
              reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(
                vkGetInstanceProcAddr(this->instance,
                                      "vkDestroyDebugReportCallbackEXT"));
            assert(vkDestroyDebugReportCallback);
            vkDestroyDebugReportCallback(
              this->instance, debugReportCallback, nullptr);
        }
#endif
        vkDestroyInstance(this->instance, nullptr);
    }
};

int
main()
{
    VulkanExample* vulkanExample = new VulkanExample();
    std::cout << "Finished.";
    delete (vulkanExample);
    return 0;
}
