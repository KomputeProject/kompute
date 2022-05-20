#include "kompute/logger/Logger.hpp"

#include <cassert>
#include <iostream>
#include <memory>
#include <mutex>
#include <spdlog/async.h>
#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <sys/stat.h>
#include <sys/types.h>

#ifdef _WIN32
#include <iso646.h>
#endif // _WIN32

namespace logger {
constexpr int THREAD_QUEUE_LENGTH = 8192;
constexpr int FILE_ROTATION_TIME = 1048576 * 5;

/**
 * Should be replaced with:
 * std::filesystem::exists(path)
 * when switching to cpp17
 **/
bool
exists(const std::string& path)
{
    struct stat info
    {};
    if (stat(path.c_str(), &info) != 0) {
        // std::cerr << "Failed to check if '" << path
        //           << "' exists. Cannot access!\n";
        // assert(false);
        return false;
    }
    return info.st_mode & S_IFDIR;
}

/**
 * Based on: https://stackoverflow.com/a/35109823
 * Should be replaced with:
 * std::filesystem::create_directory(path);
 * when switching to cpp17
 **/
void
createDir(const std::string& path)
{
    mode_t nMode = 0733; // UNIX style permissions
    int nError = 0;
#if defined(_WIN32)
    nError = _mkdir(sPath.c_str()); // can be used on Windows
#else
    nError = mkdir(path.c_str(), nMode); // can be used on non-Windows
#endif
    if (nError != 0) {
        std::cerr << "Failed to create '" << path << "' with: " << nError
                  << '\n';
        assert(false);
    }
}

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

    if (!exists(logger::logFolder)) {
        createDir(logger::logFolder);
    }
    spdlog::init_thread_pool(THREAD_QUEUE_LENGTH, 1);
    spdlog::sink_ptr console_sink =
      std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_pattern("[%H:%M:%S %z] [%=8l] [thread %t] [%@]\t%v");
#ifdef _WIN32
    std::string s = (logger::log_folder / "kompute.log").string();
    spdlog::sink_ptr file_sink =
      std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
        s, FILE_ROTATION_TIME, 3);
#else // _WIN32
    spdlog::sink_ptr file_sink =
      std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
        logger::logFolder + "/kompute.log", FILE_ROTATION_TIME, 3);
#endif
    file_sink->set_pattern("[%H:%M:%S %z] [%=8l] [thread %t] [%@]\t%v");
    std::vector<spdlog::sink_ptr> sinks{ file_sink, console_sink };
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

void
deactivateLogger()
{
    logger::setLogLevel(spdlog::level::off);
}

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
