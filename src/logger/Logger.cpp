#include "kompute/logger/Logger.hpp"

#if !KOMPUTE_OPT_LOG_LEVEL_DISABLED
#if !KOMPUTE_OPT_USE_SPDLOG
#else
#include <cassert>
#include <iostream>
#include <memory>
#include <mutex>
#include <spdlog/async.h>
#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <string>
#endif // !KOMPUTE_OPT_USE_SPDLOG

namespace logger {
#if !KOMPUTE_OPT_USE_SPDLOG

void
setupLogger()
{
}

#else
constexpr int THREAD_QUEUE_LENGTH = 8192;

void
setupLogger()
{
    // Ensure we setup the logger only once
    static bool setup = false;
    static std::mutex setupMutex{};
    setupMutex.lock();
    if (setup) {
        setupMutex.unlock();
        return;
    }
    setup = true;
    setupMutex.unlock();

    spdlog::init_thread_pool(THREAD_QUEUE_LENGTH, 1);
    spdlog::sink_ptr console_sink =
      std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_pattern("[%H:%M:%S %z] [%=8l] [thread %t] [%@]\t%v");
    std::vector<spdlog::sink_ptr> sinks{ console_sink };
    std::shared_ptr<spdlog::logger> logger =
      std::make_shared<spdlog::async_logger>(
        "",
        sinks.begin(),
        sinks.end(),
        spdlog::thread_pool(),
        spdlog::async_overflow_policy::block);

    logger->set_level(getLogLevel());
    spdlog::set_default_logger(logger);
}

spdlog::level::level_enum
getLogLevel()
{
#if SPDLOG_ACTIVE_LEVEL == SPDLOG_LEVEL_TRACE
    return spdlog::level::trace;
#endif

#if SPDLOG_ACTIVE_LEVEL == SPDLOG_LEVEL_DEBUG
    return spdlog::level::debug;
#endif

#if SPDLOG_ACTIVE_LEVEL == SPDLOG_LEVEL_INFO
    return spdlog::level::info;
#endif

#if SPDLOG_ACTIVE_LEVEL == SPDLOG_LEVEL_WARN
    return spdlog::level::warn;
#endif

#if SPDLOG_ACTIVE_LEVEL == SPDLOG_LEVEL_ERROR
    return spdlog::level::error;
#endif

#if SPDLOG_ACTIVE_LEVEL == SPDLOG_LEVEL_CRITICAL
    return spdlog::level::critical;
#endif

    return spdlog::level::off;
}

void
setLogLevel(const spdlog::level::level_enum level)
{
    spdlog::default_logger()->set_level(level);
}
#endif // !KOMPUTE_OPT_USE_SPDLOG

std::string
setToString(const std::set<std::string>& set)
{
    std::string result;
    for (const std::string& entry : set) {
        result += entry + ", ";
    }
    if (result.empty()) {
        return result;
    }
    return result.substr(0, result.size() - 2); // Remove the tailing ", "
}

std::string
vecToString(const std::vector<const char*>& vec)
{
    std::string result;
    for (const char* entry : vec) {
        result += std::string(entry) + ", ";
    }
    if (result.empty()) {
        return result;
    }
    return result.substr(0, result.size() - 2); // Remove the tailing ", "
}

std::string
vecToString(const std::vector<std::string>& vec)
{
    std::string result;
    for (const std::string& entry : vec) {
        result += entry + ", ";
    }
    if (result.empty()) {
        return result;
    }
    return result.substr(0, result.size() - 2); // Remove the tailing ", "
}
} // namespace logger

#endif
