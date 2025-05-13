// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <vulkan/vulkan.hpp>

#ifdef _MSC_VER
    #define UNUSED(x) x
#else
    #define UNUSED(x) __attribute__((unused)) x
#endif

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
#define KOMPUTE_VK_API_MINOR_VERSION 2
#endif // KOMPUTE_VK_API_MINOR_VERSION
#define KOMPUTE_VK_API_VERSION                                                 \
    VK_MAKE_VERSION(                                                           \
      KOMPUTE_VK_API_MAJOR_VERSION, KOMPUTE_VK_API_MINOR_VERSION, 0)
#endif // KOMPUTE_VK_API_VERSION

#if defined(KOMPUTE_BUILD_PYTHON)
#include <pybind11/pybind11.h>
namespace py = pybind11;
// from python/src/main.cpp
extern py::object kp_trace, kp_debug, kp_info, kp_warning, kp_error;
#endif
