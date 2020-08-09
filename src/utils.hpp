#pragma once

#include <string>

namespace vkx {
const std::string& getAssetPath();

enum class LogLevel
{
    LOG_DEBUG = 0,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
};

void logMessage(LogLevel level, const char* format, ...);
}  // namespace vkx
