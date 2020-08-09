/*
* Vulkan examples debug wrapper
*
* Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#include "debug.hpp"
#include <iostream>
#include <sstream>
#include <mutex>

#ifdef __ANDROID__
#include <android/log.h>
#define LOG_TAG "org.saintandreas.vulkan"
#endif

using namespace vks;

namespace vks { namespace debug {

#if defined(__ANDROID__)
std::list<std::string> validationLayerNames = { "VK_LAYER_GOOGLE_threading",      "VK_LAYER_LUNARG_parameter_validation",
                                                "VK_LAYER_LUNARG_object_tracker", "VK_LAYER_LUNARG_core_validation",
                                                "VK_LAYER_LUNARG_swapchain",      "VK_LAYER_GOOGLE_unique_objects" };
#else
std::list<std::string> validationLayerNames = {
    // This is a meta layer that enables all of the standard
    // validation layers in the correct order :
    // threading, parameter_validation, device_limits, object_tracker, image, core_validation, swapchain, and unique_objects
    "VK_LAYER_LUNARG_assistant_layer", "VK_LAYER_LUNARG_standard_validation"
};
#endif

static std::once_flag dispatcherInitFlag;
vk::DispatchLoaderDynamic dispatcher;
vk::DebugReportCallbackEXT msgCallback;

VkBool32 messageCallback(VkDebugReportFlagsEXT flags,
                         VkDebugReportObjectTypeEXT objType,
                         uint64_t srcObject,
                         size_t location,
                         int32_t msgCode,
                         const char* pLayerPrefix,
                         const char* pMsg,
                         void* pUserData) {
    std::string message;
    {
        std::stringstream buf;
        if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
            buf << "ERROR: ";
        } else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
            buf << "WARNING: ";
        } else if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
            buf << "PERF: ";
        } else {
            return false;
        }
        buf << "[" << pLayerPrefix << "] Code " << msgCode << " : " << pMsg;
        message = buf.str();
    }

    std::cout << message << std::endl;

#ifdef __ANDROID__
    __android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, message.c_str());
#endif
#ifdef _MSC_VER
    OutputDebugStringA(message.c_str());
    OutputDebugStringA("\n");
#endif
    return false;
}

void setupDebugging(const vk::Instance& instance, const vk::DebugReportFlagsEXT& flags, const MessageHandler& handler) {
    std::call_once(dispatcherInitFlag, [&] { dispatcher.init(instance, &vkGetInstanceProcAddr); });
    vk::DebugReportCallbackCreateInfoEXT dbgCreateInfo = {};
    dbgCreateInfo.pfnCallback = (PFN_vkDebugReportCallbackEXT)messageCallback;
    dbgCreateInfo.flags = flags;
    msgCallback = instance.createDebugReportCallbackEXT(dbgCreateInfo, nullptr, dispatcher);
}

void freeDebugCallback(const vk::Instance& instance) {
    std::call_once(dispatcherInitFlag, [&] { dispatcher.init(instance, &vkGetInstanceProcAddr); });
    instance.destroyDebugReportCallbackEXT(msgCallback, nullptr, dispatcher);
}

namespace marker {
bool active = false;
static std::once_flag markerDispatcherInitFlag;
vk::DispatchLoaderDynamic markerDispatcher;

void setup(const vk::Instance& instance, const vk::Device& device) {
    std::call_once(markerDispatcherInitFlag, [&] { markerDispatcher.init(instance, &vkGetInstanceProcAddr, device, &vkGetDeviceProcAddr); });
    // Set flag if at least one function pointer is present
    active = (markerDispatcher.vkDebugMarkerSetObjectNameEXT != VK_NULL_HANDLE);
}

void setObjectName(const vk::Device& device, uint64_t object, vk::DebugReportObjectTypeEXT objectType, const char* name) {
    // Check for valid function pointer (may not be present if not running in a debugging application)
    if (markerDispatcher.vkDebugMarkerSetObjectNameEXT) {
        device.debugMarkerSetObjectNameEXT({ objectType, object, name }, markerDispatcher);
    }
}

void setObjectTag(const vk::Device& device, uint64_t object, vk::DebugReportObjectTypeEXT objectType, uint64_t name, size_t tagSize, const void* tag) {
    // Check for valid function pointer (may not be present if not running in a debugging application)
    if (markerDispatcher.vkDebugMarkerSetObjectTagEXT) {
        device.debugMarkerSetObjectTagEXT({ objectType, object, name, tagSize, tag }, markerDispatcher);
    }
}

static std::array<float, 4> toFloatArray(const glm::vec4& color) {
    return { color.r, color.g, color.b, color.a };
}

void beginRegion(const vk::CommandBuffer& cmdbuffer, const std::string& markerName, const glm::vec4& color) {
    // Check for valid function pointer (may not be present if not running in a debugging application)
    if (markerDispatcher.vkCmdDebugMarkerBeginEXT) {
        cmdbuffer.debugMarkerBeginEXT({ markerName.c_str(), toFloatArray(color) }, markerDispatcher);
    }
}

void insert(const vk::CommandBuffer& cmdbuffer, const std::string& markerName, const glm::vec4& color) {
    // Check for valid function pointer (may not be present if not running in a debugging application)
    if (markerDispatcher.vkCmdDebugMarkerInsertEXT) {
        cmdbuffer.debugMarkerInsertEXT({ markerName.c_str(), toFloatArray(color) }, markerDispatcher);
    }
}

void endRegion(const vk::CommandBuffer& cmdbuffer) {
    // Check for valid function (may not be present if not runnin in a debugging application)
    if (markerDispatcher.vkCmdDebugMarkerEndEXT) {
        cmdbuffer.debugMarkerEndEXT(markerDispatcher);
    }
}

void setCommandBufferName(const vk::Device& device, const VkCommandBuffer& cmdBuffer, const char* name) {
    setObjectName(device, (uint64_t)cmdBuffer, vk::DebugReportObjectTypeEXT::eCommandBuffer, name);
}

void setQueueName(const vk::Device& device, const VkQueue& queue, const char* name) {
    setObjectName(device, (uint64_t)queue, vk::DebugReportObjectTypeEXT::eQueue, name);
}

void setImageName(const vk::Device& device, const VkImage& image, const char* name) {
    setObjectName(device, (uint64_t)image, vk::DebugReportObjectTypeEXT::eImage, name);
}

void setSamplerName(const vk::Device& device, const VkSampler& sampler, const char* name) {
    setObjectName(device, (uint64_t)sampler, vk::DebugReportObjectTypeEXT::eSampler, name);
}

void setBufferName(const vk::Device& device, const VkBuffer& buffer, const char* name) {
    setObjectName(device, (uint64_t)buffer, vk::DebugReportObjectTypeEXT::eBuffer, name);
}

void setDeviceMemoryName(const vk::Device& device, const VkDeviceMemory& memory, const char* name) {
    setObjectName(device, (uint64_t)memory, vk::DebugReportObjectTypeEXT::eDeviceMemory, name);
}

void setShaderModuleName(const vk::Device& device, const VkShaderModule& shaderModule, const char* name) {
    setObjectName(device, (uint64_t)shaderModule, vk::DebugReportObjectTypeEXT::eShaderModule, name);
}

void setPipelineName(const vk::Device& device, const VkPipeline& pipeline, const char* name) {
    setObjectName(device, (uint64_t)pipeline, vk::DebugReportObjectTypeEXT::ePipeline, name);
}

void setPipelineLayoutName(const vk::Device& device, const VkPipelineLayout& pipelineLayout, const char* name) {
    setObjectName(device, (uint64_t)pipelineLayout, vk::DebugReportObjectTypeEXT::ePipelineLayout, name);
}

void setRenderPassName(const vk::Device& device, const VkRenderPass& renderPass, const char* name) {
    setObjectName(device, (uint64_t)renderPass, vk::DebugReportObjectTypeEXT::eRenderPass, name);
}

void setFramebufferName(const vk::Device& device, const VkFramebuffer& framebuffer, const char* name) {
    setObjectName(device, (uint64_t)framebuffer, vk::DebugReportObjectTypeEXT::eFramebuffer, name);
}

void setDescriptorSetLayoutName(const vk::Device& device, const VkDescriptorSetLayout& descriptorSetLayout, const char* name) {
    setObjectName(device, (uint64_t)descriptorSetLayout, vk::DebugReportObjectTypeEXT::eDescriptorSetLayout, name);
}

void setDescriptorSetName(const vk::Device& device, const VkDescriptorSet& descriptorSet, const char* name) {
    setObjectName(device, (uint64_t)descriptorSet, vk::DebugReportObjectTypeEXT::eDescriptorSet, name);
}

void setSemaphoreName(const vk::Device& device, const VkSemaphore& semaphore, const char* name) {
    setObjectName(device, (uint64_t)semaphore, vk::DebugReportObjectTypeEXT::eSemaphore, name);
}

void setFenceName(const vk::Device& device, const VkFence& fence, const char* name) {
    setObjectName(device, (uint64_t)fence, vk::DebugReportObjectTypeEXT::eFence, name);
}

void setEventName(const vk::Device& device, const VkEvent& _event, const char* name) {
    setObjectName(device, (uint64_t)_event, vk::DebugReportObjectTypeEXT::eEvent, name);
}
};  // namespace marker
}}  // namespace vks::debug
