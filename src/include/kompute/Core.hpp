#pragma once

#ifdef VK_USE_PLATFORM_ANDROID_KHR
#include <android/log.h>
#include <kompute_vk_ndk_wrapper.hpp>
// VK_NO_PROTOTYPES required before vulkan import but after wrapper.hpp
#undef VK_NO_PROTOTYPES
static const char* KOMPUTE_LOG_TAG = "KomputeLog";
#endif

#include <vulkan/vulkan.hpp>

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

// SPDLOG_ACTIVE_LEVEL must be defined before spdlog.h import
#ifndef SPDLOG_ACTIVE_LEVEL
#if DEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#else
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#endif
#endif

#if defined(KOMPUTE_BUILD_PYTHON)
#include <pybind11/pybind11.h>
namespace py = pybind11;
// from python/src/main.cpp
extern py::object kp_debug, kp_info, kp_warning, kp_error;
#endif

#ifndef KOMPUTE_LOG_OVERRIDE
#if KOMPUTE_ENABLE_SPDLOG
#include <spdlog/spdlog.h>
#else
#include <iostream>
#if SPDLOG_ACTIVE_LEVEL > 1
#define SPDLOG_DEBUG(message, ...)
#else
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
#define SPDLOG_DEBUG(message, ...)                                             \
    ((void)__android_log_print(ANDROID_LOG_DEBUG, KOMPUTE_LOG_TAG, message))
#elif defined(KOMPUTE_BUILD_PYTHON)
#define SPDLOG_DEBUG(message, ...) kp_debug(message);
#else
#define SPDLOG_DEBUG(message, ...)                                             \
    std::cout << "DEBUG: " << message << std::endl
#endif // VK_USE_PLATFORM_ANDROID_KHR
#endif // SPDLOG_ACTIVE_LEVEL > 1

#if SPDLOG_ACTIVE_LEVEL > 2
#define SPDLOG_INFO(message, ...)
#else
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
#define SPDLOG_INFO(message, ...)                                              \
    ((void)__android_log_print(ANDROID_LOG_INFO, KOMPUTE_LOG_TAG, message))
#elif defined(KOMPUTE_BUILD_PYTHON)
#define SPDLOG_INFO(message, ...) kp_info(message);
#else
#define SPDLOG_INFO(message, ...) std::cout << "INFO: " << message << std::endl
#endif // VK_USE_PLATFORM_ANDROID_KHR
#endif // SPDLOG_ACTIVE_LEVEL > 2

#if SPDLOG_ACTIVE_LEVEL > 3
#define SPDLOG_WARN(message, ...)
#else
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
#define SPDLOG_WARN(message, ...)                                              \
    ((void)__android_log_print(ANDROID_LOG_INFO, KOMPUTE_LOG_TAG, message))
#elif defined(KOMPUTE_BUILD_PYTHON)
#define SPDLOG_WARN(message, ...) kp_warning(message);
#else
#define SPDLOG_WARN(message, ...)                                              \
    std::cout << "WARNING: " << message << std::endl
#endif // VK_USE_PLATFORM_ANDROID_KHR
#endif // SPDLOG_ACTIVE_LEVEL > 3

#if SPDLOG_ACTIVE_LEVEL > 4
#define SPDLOG_ERROR(message, ...)
#else
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
#define SPDLOG_ERROR(message, ...)                                             \
    ((void)__android_log_print(ANDROID_LOG_INFO, KOMPUTE_LOG_TAG, message))
#elif defined(KOMPUTE_BUILD_PYTHON)
#define SPDLOG_ERROR(message, ...) kp_error(message);
#else
#define SPDLOG_ERROR(message, ...)                                             \
    std::cout << "ERROR: " << message << std::endl
#endif // VK_USE_PLATFORM_ANDROID_KHR
#endif // SPDLOG_ACTIVE_LEVEL > 4
#endif // KOMPUTE_SPDLOG_ENABLED
#endif // KOMPUTE_LOG_OVERRIDE
