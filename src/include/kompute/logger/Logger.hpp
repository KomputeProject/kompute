#pragma once

#define KOMPUTE_LOG_LEVEL_TRACE 0
#define KOMPUTE_LOG_LEVEL_DEBUG 1
#define KOMPUTE_LOG_LEVEL_INFO 2
#define KOMPUTE_LOG_LEVEL_WARN 3
#define KOMPUTE_LOG_LEVEL_ERROR 4
#define KOMPUTE_LOG_LEVEL_CRITICAL 5
#define KOMPUTE_LOG_LEVEL_OFF 6

// Logging is disabled entirely.
#if KOMPUTE_OPT_LOG_LEVEL_DISABLED
#define KP_LOG_TRACE(...)
#define KP_LOG_DEBUG(...)
#define KP_LOG_INFO(...)
#define KP_LOG_WARN(...)
#define KP_LOG_ERROR(...)
#else

#if !KOMPUTE_OPT_USE_SPDLOG
#include <fmt/core.h>
#else
#include <spdlog/spdlog.h>
#endif // !KOMPUTE_OPT_USE_SPDLOG
#include <set>
#include <string>
#include <vector>
namespace logger {
// Setup the logger, note the loglevel can not be set below the CMake log level
// (To change this use -DKOMPUTE_OPT_LOG_LEVEL=...)
void
setupLogger();

// Logging is enabled, but we do not use Spdlog. So we use fmt in case nothing
// else is defined, overriding logging.
#if !KOMPUTE_OPT_USE_SPDLOG

#ifndef KP_LOG_TRACE
#if KOMPUTE_OPT_LOG_LEVEL <= KOMPUTE_LOG_LEVEL_TRACE
#define KP_LOG_TRACE(...)                                                      \
    fmt::print("[{} {}] [trace] [{}:{}] {}\n",                                 \
               __DATE__,                                                       \
               __TIME__,                                                       \
               __FILE__,                                                       \
               __LINE__,                                                       \
               fmt::format(__VA_ARGS__))
#else
#define KP_LOG_TRACE(...)
#endif
#endif // !KP_LOG_TRACE

#ifndef KP_LOG_DEBUG
#if KOMPUTE_OPT_LOG_LEVEL <= KOMPUTE_LOG_LEVEL_DEBUG
#define KP_LOG_DEBUG(...)                                                      \
    fmt::print("[{} {}] [debug] [{}:{}] {}\n",                                 \
               __DATE__,                                                       \
               __TIME__,                                                       \
               __FILE__,                                                       \
               __LINE__,                                                       \
               fmt::format(__VA_ARGS__))
#else
#define KP_LOG_DEBUG(...)
#endif
#endif // !KP_LOG_DEBUG

#ifndef KP_LOG_INFO
#if KOMPUTE_OPT_LOG_LEVEL <= KOMPUTE_LOG_LEVEL_INFO
#define KP_LOG_INFO(...)                                                       \
    fmt::print("[{} {}] [info] [{}:{}] {}\n",                                  \
               __DATE__,                                                       \
               __TIME__,                                                       \
               __FILE__,                                                       \
               __LINE__,                                                       \
               fmt::format(__VA_ARGS__))
#else
#define KP_LOG_INFO(...)
#endif
#endif // !KP_LOG_INFO

#ifndef KP_LOG_WARN
#if KOMPUTE_OPT_LOG_LEVEL <= KOMPUTE_LOG_LEVEL_WARN
#define KP_LOG_WARN(...)                                                       \
    fmt::print("[{} {}] [warn] [{}:{}] {}\n",                                  \
               __DATE__,                                                       \
               __TIME__,                                                       \
               __FILE__,                                                       \
               __LINE__,                                                       \
               fmt::format(__VA_ARGS__))
#else
#define KP_LOG_WARN(...)
#endif
#endif // !KP_LOG_WARN

#ifndef KP_LOG_ERROR
#if KOMPUTE_OPT_LOG_LEVEL <= KOMPUTE_LOG_LEVEL_ERROR
#define KP_LOG_ERROR(...)                                                      \
    fmt::print("[{} {}] [error] [{}:{}] {}\n",                                 \
               __DATE__,                                                       \
               __TIME__,                                                       \
               __FILE__,                                                       \
               __LINE__,                                                       \
               fmt::format(__VA_ARGS__))
#else
#define KP_LOG_ERROR(...)
#endif
#endif // !KP_LOG_ERROR
#else

#define KP_LOG_TRACE(...) SPDLOG_TRACE(__VA_ARGS__)
#define KP_LOG_DEBUG(...) SPDLOG_DEBUG(__VA_ARGS__)
#define KP_LOG_INFO(...) SPDLOG_INFO(__VA_ARGS__)
#define KP_LOG_WARN(...) SPDLOG_WARN(__VA_ARGS__)
#define KP_LOG_ERROR(...) SPDLOG_ERROR(__VA_ARGS__)

void
setLogLevel(spdlog::level::level_enum level);

spdlog::level::level_enum
getLogLevel();

#endif // !KOMPUTE_OPT_USE_SPDLOG

std::string
setToString(const std::set<std::string>& set);

std::string
vecToString(const std::vector<const char*>& vec);

std::string
vecToString(const std::vector<std::string>& vec);
} // namespace logger

#endif // KOMPUTE_OPT_LOG_LEVEL_DISABLED