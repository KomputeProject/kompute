#pragma once

#if VK_USE_PLATFORM_ANDROID_KHR
#include <android/log.h>
#include <kompute_vk_ndk_wrapper.hpp>
// VK_NO_PROTOTYPES required before vulkan import but after wrapper.hpp
#undef VK_NO_PROTOTYPES
static const char* KOMPUTE_LOG_TAG = "KomputeLog";
#endif

#include <fmt/core.h>

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
#define KP_LOG_DEBUG(...) SPDLOG_DEBUG(__VA_ARGS__)
#define KP_LOG_INFO(...) SPDLOG_INFO(__VA_ARGS__)
#define KP_LOG_WARN(...) SPDLOG_WARN(__VA_ARGS__)
#define KP_LOG_ERROR(...) SPDLOG_ERROR(__VA_ARGS__)
#else
#include <iostream>
#if SPDLOG_ACTIVE_LEVEL > 1
#define KP_LOG_DEBUG(...)
#else
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
#define KP_LOG_DEBUG(...)                                                      \
    ((void)__android_log_write(                                                \
      ANDROID_LOG_DEBUG, KOMPUTE_LOG_TAG, fmt::format(__VA_ARGS__).c_str()))
#elif defined(KOMPUTE_BUILD_PYTHON)
#define KP_LOG_DEBUG(...) kp_debug(fmt::format(__VA_ARGS__))
#else
#define KP_LOG_DEBUG(...)                                                      \
    fmt::print("[{} {}] [debug] [{}:{}] {}\n",                                 \
               __DATE__,                                                       \
               __TIME__,                                                       \
               __FILE__,                                                       \
               __LINE__,                                                       \
               fmt::format(__VA_ARGS__))
#endif // VK_USE_PLATFORM_ANDROID_KHR
#endif // SPDLOG_ACTIVE_LEVEL > 1

#if SPDLOG_ACTIVE_LEVEL > 2
#define KP_LOG_INFO(...)
#else
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
#define KP_LOG_INFO(...)                                                       \
    ((void)__android_log_write(                                                \
      ANDROID_LOG_INFO, KOMPUTE_LOG_TAG, fmt::format(__VA_ARGS__).c_str()))
#elif defined(KOMPUTE_BUILD_PYTHON)
#define KP_LOG_INFO(...) kp_info(fmt::format(__VA_ARGS__))
#else
#define KP_LOG_INFO(...)                                                       \
    fmt::print("[{} {}] [debug] [{}:{}] {}\n",                                 \
               __DATE__,                                                       \
               __TIME__,                                                       \
               __FILE__,                                                       \
               __LINE__,                                                       \
               fmt::format(__VA_ARGS__))
#endif // VK_USE_PLATFORM_ANDROID_KHR
#endif // SPDLOG_ACTIVE_LEVEL > 2

#if SPDLOG_ACTIVE_LEVEL > 3
#define KP_LOG_WARN(...)
#else
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
#define KP_LOG_WARN(...)                                                       \
    ((void)__android_log_write(                                                \
      ANDROID_LOG_WARN, KOMPUTE_LOG_TAG, fmt::format(__VA_ARGS__).c_str()))
#elif defined(KOMPUTE_BUILD_PYTHON)
#define KP_LOG_WARN(...) kp_warning(fmt::format(__VA_ARGS__))
#else
#define KP_LOG_WARN(...)                                                       \
    fmt::print("[{} {}] [debug] [{}:{}] {}\n",                                 \
               __DATE__,                                                       \
               __TIME__,                                                       \
               __FILE__,                                                       \
               __LINE__,                                                       \
               fmt::format(__VA_ARGS__))
#endif // VK_USE_PLATFORM_ANDROID_KHR
#endif // SPDLOG_ACTIVE_LEVEL > 3

#if SPDLOG_ACTIVE_LEVEL > 4
#define KP_LOG_ERROR(...)
#else
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
#define KP_LOG_ERROR(...)                                                      \
    ((void)__android_log_write(                                                \
      ANDROID_LOG_ERROR, KOMPUTE_LOG_TAG, fmt::format(__VA_ARGS__).c_str()))
#elif defined(KOMPUTE_BUILD_PYTHON)
#define KP_LOG_ERROR(...) kp_error(fmt::format(__VA_ARGS__))
#else
#define KP_LOG_ERROR(...)                                                      \
    fmt::print("[{} {}] [debug] [{}:{}] {}\n",                                 \
               __DATE__,                                                       \
               __TIME__,                                                       \
               __FILE__,                                                       \
               __LINE__,                                                       \
               fmt::format(__VA_ARGS__))
#endif // VK_USE_PLATFORM_ANDROID_KHR
#endif // SPDLOG_ACTIVE_LEVEL > 4
#endif // KOMPUTE_SPDLOG_ENABLED
#endif // KOMPUTE_LOG_OVERRIDE
