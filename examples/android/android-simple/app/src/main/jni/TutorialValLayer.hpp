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
#ifndef __VALLAYER_HPP__
#define __VALLAYER_HPP__
#include <vulkan_wrapper.h>
#include <iostream>
#include <map>
#include <vector>

// Some loader only report one layer for device layers, enable this to
// workaround it: it will copy all instance layers into device layers
// and NOT enumerating device layers
//#define LOADER_DEVICE_LAYER_REPORT_BUG_WA

// If extension is not enumerated, it should not be enabled in general;
// If you know an extension is on the device, but loader does not report it,
// it could be forced in by enabling
// the following compile flag and call AddInstanceExt().
// #define ENABLE_NON_ENUMERATED_EXT 1

// A Helper class to manage validation layers and extensions
// Supposed usage:
//   1) validation layers: app should enable them with
//        InstanceLayerCount()
//        InstanceLayerNames()
//   2) Extension layers:  app should check for supportability
//        and then enable in app code ( not in this class )
//   3) DbgExtension: once instance is created, just call
//        HookDbgReportExt
class LayerAndExtensions {
 public:
  LayerAndExtensions(void);
  ~LayerAndExtensions();

  uint32_t InstLayerCount(void);
  char** InstLayerNames(void);
  uint32_t InstExtCount(void);
  char** InstExtNames(void);
  bool IsInstExtSupported(const char* extName);
  bool IsInstLayerSupported(const char* layerName);

  const char* GetDbgExtName(void);
  bool AddInstanceExt(const char* extName);
  bool HookDbgReportExt(VkInstance instance);

  void InitDevLayersAndExt(VkPhysicalDevice physicalDevice);
  char** DevLayerNames(void);
  uint32_t DevLayerCount(void);
  char** DevExtNames(void);
  uint32_t DevExtCount(void);

 private:
  // internal helper data structure
  struct ExtInfo {
    uint32_t count;
    VkExtensionProperties* prop;
  };

  VkInstance instance_;
  VkDebugReportCallbackEXT vkCallbackHandle_;
  VkPhysicalDevice physicalDev_;

  std::vector<char*> instLayers_;
  std::vector<char*> instExts_;
  std::vector<char*> devLayers_;
  std::vector<char*> devExts_;

  VkLayerProperties* instLayerProp_{nullptr};
  uint32_t instLayerCount_{0};
  std::vector<ExtInfo> instExtProp_;

  VkLayerProperties* devLayerProp_{nullptr};
  uint32_t devLayerCount_{0};
  std::vector<ExtInfo> devExtProp_;

  bool InitInstExts(void);
  bool InitExtNames(const std::vector<ExtInfo>& props,
                    std::vector<char*>* names);
  void CheckLayerLoadingSequence(std::vector<char*>* layers);
};

#endif  // __VALLAYER_HPP__
