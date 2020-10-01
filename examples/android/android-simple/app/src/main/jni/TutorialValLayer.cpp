// Copyright 2016 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "TutorialValLayer.hpp"
#include <android/log.h>
#include <cassert>

// Android log function wrappers
static const char* kTAG = "Vulkan-Tutorial02";
#define LOGI(...) \
  ((void)__android_log_print(ANDROID_LOG_INFO, kTAG, __VA_ARGS__))
#define LOGW(...) \
  ((void)__android_log_print(ANDROID_LOG_WARN, kTAG, __VA_ARGS__))
#define LOGE(...) \
  ((void)__android_log_print(ANDROID_LOG_ERROR, kTAG, __VA_ARGS__))

// Vulkan call wrapper
#define CALL_VK(func)                                                 \
  if (VK_SUCCESS != (func)) {                                         \
    __android_log_print(ANDROID_LOG_ERROR, "Tutorial ",               \
                        "Vulkan error. File[%s], line[%d]", __FILE__, \
                        __LINE__);                                    \
    assert(false);                                                    \
  }

/*
 * Validation Layer names at this time (info purpose)
   static const char*  kValLayerNames[] = {
        "VK_LAYER_GOOGLE_threading",
        "VK_LAYER_LUNARG_device_limits",
        "VK_LAYER_LUNARG_core_validation",
        "VK_LAYER_LUNARG_image",
        "VK_LAYER_LUNARG_object_tracker",
        "VK_LAYER_LUNARG_parameter_validation",
        "VK_LAYER_LUNARG_swapchain",
        "VK_LAYER_GOOGLE_unique_objects",
   };
*/
static const char* kUniqueObjectLayer = "VK_LAYER_GOOGLE_unique_objects";
static const char* kGoogleThreadingLayer = "VK_LAYER_GOOGLE_threading";

// Debug Extension names in use.
// assumed usage:
//   1) app calls GetDbgExtName()
//   2) app calls IsExtSupported() to make sure it is supported
//   3) app tucks dbg extension name into InstanceCreateInfo to create instance
//   4) app calls CreateDbgExtCallback() to hook up debug print message
static const char* kDbgExtName = "VK_EXT_debug_report";

// Simple Dbg Callback function to be used by Vk engine
static VkBool32 VKAPI_PTR vkDebugReportCallbackEX_impl(
    VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType,
    uint64_t object, size_t location, int32_t messageCode,
    const char* pLayerPrefix, const char* pMessage, void* pUserData) {
  if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) {
    __android_log_print(ANDROID_LOG_INFO, "Vulkan-Debug-Message: ", "%s -- %s",
                        pLayerPrefix, pMessage);
  }
  if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
    __android_log_print(ANDROID_LOG_WARN, "Vulkan-Debug-Message: ", "%s -- %s",
                        pLayerPrefix, pMessage);
  }
  if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
    __android_log_print(ANDROID_LOG_WARN, "Vulkan-Debug-Message-(Perf): ",
                        "%s -- %s", pLayerPrefix, pMessage);
  }
  if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
    __android_log_print(ANDROID_LOG_ERROR, "Vulkan-Debug-Message: ", "%s -- %s",
                        pLayerPrefix, pMessage);
  }
  if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) {
    __android_log_print(ANDROID_LOG_DEBUG, "Vulkan-Debug-Message: ", "%s -- %s",
                        pLayerPrefix, pMessage);
  }

  return VK_FALSE;
}
// Reinforce the layer name sequence to meet load sequence
// requirement if there is any
void LayerAndExtensions::CheckLayerLoadingSequence(
    std::vector<char*>* layerPtr) {
  // VK_LAYER_GOOGLE_unique_objects need to be after
  // VK_LAYER_LUNARG_core_validation
  // VK_GOOGLE_THREADING_LAYER better to be the very first one
  std::vector<char*>& layers = *layerPtr;
  uint32_t uniqueObjIdx = -1;
  uint32_t threadingIdx = -1;
  uint32_t size = layers.size();
  for (uint32_t idx = 0; idx < size; ++idx) {
    if (!strcmp(layers[idx], kUniqueObjectLayer)) uniqueObjIdx = idx;
    if (!strcmp(layers[idx], kGoogleThreadingLayer)) threadingIdx = idx;
  }
  if (uniqueObjIdx != -1) {
    char* tmp = layers[uniqueObjIdx];
    layers[uniqueObjIdx] = layers[size - 1];
    layers[size - 1] = tmp;
  }

  if (threadingIdx != -1) {
    char* tmp = layers[threadingIdx];
    layers[threadingIdx] = layers[0];
    layers[0] = tmp;
  }
}

bool LayerAndExtensions::InitExtNames(const std::vector<ExtInfo>& prop,
                                      std::vector<char*>* names) {
  names->clear();
  for (auto& ext : prop) {
    for (uint32_t i = 0; i < ext.count; ++i) {
      // skip the one already inside
      bool duplicate = false;
      for (uint32_t j = 0; j < names->size(); ++j) {
        if (!strcmp(ext.prop[i].extensionName, (*names)[j])) {
          duplicate = true;
          break;
        }
      }
      if (duplicate) continue;
      // save this unique one
      names->push_back(ext.prop[i].extensionName);
      LOGI("Ext name: %s", ext.prop[i].extensionName);
    }
  }
  return true;
}

bool LayerAndExtensions::InitInstExts(void) {
  ExtInfo extInfo{0, nullptr};
  CALL_VK(
      vkEnumerateInstanceExtensionProperties(nullptr, &extInfo.count, nullptr));
  if (extInfo.count) {
    extInfo.prop = new VkExtensionProperties[extInfo.count];
    assert(extInfo.prop);
    CALL_VK(vkEnumerateInstanceExtensionProperties(nullptr, &extInfo.count,
                                                   extInfo.prop));
    instExtProp_.push_back(extInfo);
  }

  for (int i = 0; i < instLayerCount_; i++) {
    extInfo.count = 0;
    CALL_VK(vkEnumerateInstanceExtensionProperties(instLayerProp_[i].layerName,
                                                   &extInfo.count, nullptr));
    if (extInfo.count == 0) continue;
    extInfo.prop = new VkExtensionProperties[extInfo.count];
    CALL_VK(vkEnumerateInstanceExtensionProperties(
        instLayerProp_[i].layerName, &extInfo.count, extInfo.prop));
    instExtProp_.push_back(extInfo);
  }
  return InitExtNames(instExtProp_, &instExts_);
}

char** LayerAndExtensions::InstLayerNames(void) {
  if (instLayers_.size()) {
    return static_cast<char**>(instLayers_.data());
  }
  return nullptr;
}
uint32_t LayerAndExtensions::InstLayerCount(void) {
  LOGI("InstLayerCount = %d", static_cast<int32_t>(instLayers_.size()));
  return static_cast<int32_t>(instLayers_.size());
}
char** LayerAndExtensions::InstExtNames(void) {
  if (instExts_.size()) {
    return static_cast<char**>(instExts_.data());
  }
  return nullptr;
}
uint32_t LayerAndExtensions::InstExtCount(void) {
  LOGI("InstExtCount: %d", static_cast<uint32_t>(instExts_.size()));
  return static_cast<uint32_t>(instExts_.size());
}

bool LayerAndExtensions::IsInstExtSupported(const char* extName) {
  for (auto name : instExts_) {
    if (!strcmp(name, extName)) {
      return true;
    }
  }
  return false;
}

bool LayerAndExtensions::AddInstanceExt(const char* extName) {
  if (!extName) return false;

  // enable all available extensions, plus the one asked
  if (!IsInstExtSupported(extName)) {
#ifdef ENABLE_NON_ENUMERATED_EXT
    instExts.push_back(static_cast<char*>(extName));
    instExtCount++;
    return true;
#else
    return false;
#endif
  }
  return true;
}
bool LayerAndExtensions::IsInstLayerSupported(const char* layerName) {
  InstLayerNames();
  for (auto name : instLayers_) {
    if (!strcmp(name, layerName)) return true;
  }
  return false;
}

LayerAndExtensions::LayerAndExtensions(void) {
  instance_ = VK_NULL_HANDLE;
  vkCallbackHandle_ = VK_NULL_HANDLE;
  CALL_VK(vkEnumerateInstanceLayerProperties(&instLayerCount_, nullptr));
  if (instLayerCount_) {
    instLayerProp_ = new VkLayerProperties[instLayerCount_];
    assert(instLayerProp_);
    CALL_VK(
        vkEnumerateInstanceLayerProperties(&instLayerCount_, instLayerProp_));
  }
  for (uint32_t i = 0; i < instLayerCount_; i++) {
    instLayers_.push_back(instLayerProp_[i].layerName);
    LOGI("InstLayer name: %s", instLayerProp_[i].layerName);
  }
  CheckLayerLoadingSequence(&instLayers_);

  InitInstExts();
}

LayerAndExtensions::~LayerAndExtensions() {
  if (instLayerProp_) {
    delete[] instLayerProp_;
  }
  for (auto ext : instExtProp_) {
    delete[] ext.prop;
  }
  if (vkCallbackHandle_) {
    vkDestroyDebugReportCallbackEXT(instance_, vkCallbackHandle_, nullptr);
  }
}

void LayerAndExtensions::InitDevLayersAndExt(VkPhysicalDevice physicalDevice) {
  physicalDev_ = physicalDevice;
  if (physicalDev_ == VK_NULL_HANDLE) return;

  // get all supported layers props
  devLayerCount_ = 0;
  CALL_VK(
      vkEnumerateDeviceLayerProperties(physicalDev_, &devLayerCount_, nullptr));
  if (devLayerCount_) {
    devLayerProp_ = new VkLayerProperties[devLayerCount_];
    assert(devLayerProp_);
    CALL_VK(vkEnumerateDeviceLayerProperties(physicalDev_, &devLayerCount_,
                                             devLayerProp_));
  }

#ifdef LOADER_DEVICE_LAYER_REPORT_BUG_WA
  // validation layer for device only report out for one layer,
  // but it seems we could ask for all layers that asked for instance
  // so we just add them all in brutally
  // assume all device layers are also implemented for device layers
  if (devLayerCount_ == 1) {
    LOGI("Only Reported One layer for device");
    if (devLayerProp_) delete[] devLayerProp_;
    devLayerProp_ =
        (instLayerCount_ ? (new VkLayerProperties[instLayerCount_]) : nullptr);
    memcpy(devLayerProp_, instLayerProp_,
           instLayerCount_ * sizeof(VkLayerProperties));
    devLayerCount_ = instLayerCount_;
  }
#endif

  for (int i = 0; i < devLayerCount_; i++) {
    LOGI("deviceLayerName: %s", devLayerProp_[i].layerName);
    devLayers_.push_back(devLayerProp_[i].layerName);
  }
  CheckLayerLoadingSequence(&devLayers_);

  // get all supported ext props
  ExtInfo ext{0, nullptr};
  CALL_VK(vkEnumerateDeviceExtensionProperties(physicalDev_, nullptr,
                                               &ext.count, nullptr));
  if (ext.count) {
    ext.prop = new VkExtensionProperties[ext.count];
    assert(ext.prop);
    CALL_VK(vkEnumerateDeviceExtensionProperties(physicalDev_, nullptr,
                                                 &ext.count, ext.prop));
    devExtProp_.push_back(ext);
  }
  for (int i = 0; i < devLayerCount_; i++) {
    ext.count = 0;
    CALL_VK(vkEnumerateDeviceExtensionProperties(
        physicalDev_, devLayerProp_[i].layerName, &ext.count, nullptr));
    if (ext.count) {
      ext.prop = new VkExtensionProperties[ext.count];
      assert(ext.prop);
      CALL_VK(vkEnumerateDeviceExtensionProperties(
          physicalDev_, devLayerProp_[i].layerName, &ext.count, ext.prop));
      devExtProp_.push_back(ext);
    }
  }

  InitExtNames(devExtProp_, &devExts_);
}

char** LayerAndExtensions::DevLayerNames(void) {
  assert(physicalDev_ != VK_NULL_HANDLE);
  if (devLayers_.size()) return devLayers_.data();

  return nullptr;
}
uint32_t LayerAndExtensions::DevLayerCount(void) {
  assert(physicalDev_ != VK_NULL_HANDLE);
  return devLayers_.size();
}
char** LayerAndExtensions::DevExtNames(void) {
  assert(physicalDev_ != VK_NULL_HANDLE);
  if (devExts_.size()) return devExts_.data();
  return nullptr;
}
uint32_t LayerAndExtensions::DevExtCount(void) {
  assert(physicalDev_ != VK_NULL_HANDLE);
  return devExts_.size();
}

const char* LayerAndExtensions::GetDbgExtName(void) { return kDbgExtName; }
bool LayerAndExtensions::HookDbgReportExt(VkInstance instance) {
  instance_ = instance;
  if (!IsInstExtSupported(GetDbgExtName())) {
    return false;
  }
  if (!vkCreateDebugReportCallbackEXT) {
    vkCreateDebugReportCallbackEXT =
        (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(
            instance, "vkCreateDebugReportCallbackEXT");
    vkDestroyDebugReportCallbackEXT =
        (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(
            instance, "vkDestroyDebugReportCallbackEXT");
    vkDebugReportMessageEXT =
        (PFN_vkDebugReportMessageEXT)vkGetInstanceProcAddr(
            instance, "vkDebugReportMessageEXT");
  }

  VkDebugReportCallbackCreateInfoEXT dbgInfo = {
      .sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT,
      .pNext = nullptr,
      .flags = VK_DEBUG_REPORT_WARNING_BIT_EXT |
               VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
               VK_DEBUG_REPORT_ERROR_BIT_EXT,
      .pfnCallback = vkDebugReportCallbackEX_impl,
      .pUserData = this,  // we provide the debug object as context
  };
  CALL_VK(vkCreateDebugReportCallbackEXT(instance, &dbgInfo, nullptr,
                                         &vkCallbackHandle_));
  return true;
}
