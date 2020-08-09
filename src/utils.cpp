#include "utils.hpp"

#include <mutex>
#include <algorithm>
#include <stdarg.h>

#ifdef WIN32
#include <Windows.h>
#endif
#include <iostream>

#if defined(__ANDROID__)
inline int logLevelToAndroidPriority(vkx::LogLevel level) {
    switch (level) {
        case vkx::LogLevel::LOG_DEBUG:
            return ANDROID_LOG_DEBUG;
        case vkx::LogLevel::LOG_INFO:
            return ANDROID_LOG_INFO;
        case vkx::LogLevel::LOG_WARN:
            return ANDROID_LOG_WARN;
        case vkx::LogLevel::LOG_ERROR:
            return ANDROID_LOG_ERROR;
    }
}
#endif

void vkx::logMessage(vkx::LogLevel level, const char* format, ...) {
    va_list arglist;
    va_start(arglist, format);

#if defined(__ANDROID__)
    int prio = logLevelToAndroidPriority(level);
    __android_log_vprint(prio, "vulkanExample", format, arglist);
#else
    char buffer[8192];
    vsnprintf(buffer, 8192, format, arglist);
#ifdef WIN32
    OutputDebugStringA(buffer);
    OutputDebugStringA("\n");
#endif
    std::cout << buffer << std::endl;
#endif
    va_end(arglist);
}

const std::string& vkx::getAssetPath() {
#if defined(__ANDROID__)
    static const std::string NOTHING;
    return NOTHING;
#else
    static std::string path;
    static std::once_flag once;
    std::call_once(once, [] {
        std::string file(__FILE__);
        std::replace(file.begin(), file.end(), '\\', '/');
        std::string::size_type lastSlash = file.rfind("/");
        file = file.substr(0, lastSlash);
        path = file + "/../data/";
    });
    return path;
#endif
}
