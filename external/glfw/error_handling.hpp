#pragma once
/// \file error_handling.hpp Error handling utilities
#include <cassert>
#include <glfw/exception.hpp>

namespace glfw {

#ifndef GLFW_CXX_CUSTOM_ERROR_HANDLING
[[noreturn]] inline void report_logic_error(std::string msg) {
  throw logic_error(msg);
}

[[noreturn]] inline void report_runtime_error(std::string msg) {
  throw runtime_error(msg);
}

[[noreturn]] inline void error_callback(int code, const char* msg) {
  if (code & GLFW_NOT_INITIALIZED) {
    fprintf(stderr, "GLFW has not been initialized.");
  }
  if (code & GLFW_NO_CURRENT_CONTEXT) {
    fprintf(stderr, "No context is current for this thread.");
  }

  if (code & GLFW_INVALID_ENUM) {
    fprintf(
     stderr,
     "One of the enum parameters for the function was given an invalid enum.");
  }

  if (code & GLFW_INVALID_VALUE) {
    fprintf(
     stderr,
     "One of the parameters for the function was given an invalid value.");
  }

  if (code & GLFW_OUT_OF_MEMORY) {
    fprintf(stderr, "A memory allocation failed.");
  }

  if (code & GLFW_API_UNAVAILABLE) {
    fprintf(
     stderr,
     "GLFW could not find support for the requested client API on the system.");
  }

  if (code & GLFW_VERSION_UNAVAILABLE) {
    fprintf(stderr, "The requested client API version is not available.");
  }

  if (code & GLFW_PLATFORM_ERROR) {
    fprintf(stderr, "A platform-specific error occurred that does not match "
                    "any of the more specific categories.");
  }

  if (code & GLFW_FORMAT_UNAVAILABLE) {
    fprintf(stderr,
            "The clipboard did not contain data in the requested format.");
  }

  fprintf(stderr, "%s", msg);
  std::terminate();
}

#endif  // GLFW_CXX_CUSTOM_ERROR_HANDLING

namespace detail {

/// Error handling helper for GLFW c functions
auto handle_glfw_error(int value, std::string msg) -> void {
  if (value == GL_FALSE) { throw runtime_error(msg); }
}

/// Error handling helper for GLFW c functions
template <class T> auto handle_glfw_error(T* ptr, std::string msg) -> T * {
  if (!ptr) { throw runtime_error(msg); }
  return ptr;
}

}  // namespace detail

}  // namespace glfw
