#pragma once

#include <set>
#include <spdlog/spdlog.h>
#include <string>
#include <vector>

#define KP_LOG_TRACE(...) SPDLOG_TRACE(__VA_ARGS__)
#define KP_LOG_DEBUG(...) SPDLOG_DEBUG(__VA_ARGS__)
#define KP_LOG_INFO(...) SPDLOG_INFO(__VA_ARGS__)
#define KP_LOG_WARN(...) SPDLOG_WARN(__VA_ARGS__)
#define KP_LOG_ERROR(...) SPDLOG_ERROR(__VA_ARGS__)

namespace logger {
const std::string logFolder("kompute_logs");
// Setup the logger, note the loglevel can not be set below the CMake log level
// (To change this use -DKOMPUTE_OPT_LOG_LEVEL=...)
void
setupLogger();
void
setLogLevel(spdlog::level::level_enum level);
void
deactivateLogger();

spdlog::level::level_enum
getLogLevel();

std::string
setToString(const std::set<std::string>& set);

std::string
vecToString(const std::vector<const char*>& vec);

std::string
vecToString(const std::vector<std::string>& vec);
} // namespace logger
