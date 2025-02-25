// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <vulkan/vulkan.hpp>

// Typedefs to simplify interaction with core types
namespace kp {
typedef std::array<uint32_t, 3> Workgroup;
typedef std::vector<float> Constants;
}

// Must be after vulkan is included
#ifndef KOMPUTE_VK_API_VERSION
#ifndef KOMPUTE_VK_API_MAJOR_VERSION
#define KOMPUTE_VK_API_MAJOR_VERSION 1
#endif // KOMPUTE_VK_API_MAJOR_VERSION
#ifndef KOMPUTE_VK_API_MINOR_VERSION
#define KOMPUTE_VK_API_MINOR_VERSION 1
#endif // KOMPUTE_VK_API_MINOR_VERSION
#define KOMPUTE_VK_API_VERSION                                                 \
    VK_MAKE_VERSION(                                                           \
      KOMPUTE_VK_API_MAJOR_VERSION, KOMPUTE_VK_API_MINOR_VERSION, 0)
#endif // KOMPUTE_VK_API_VERSION

#if defined(KOMPUTE_BUILD_PYTHON)
// from python/src/main.cpp

extern void py_log_trace(const std::string& msg);
extern void py_log_debug(const std::string& msg);
extern void py_log_info(const std::string& msg);
extern void py_log_warning(const std::string& msg);
extern void py_log_error(const std::string& msg);
#endif
