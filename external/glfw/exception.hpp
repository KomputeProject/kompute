#pragma once
/// \file exception.hpp Implements GLFW exceptions
#include <exception>
#include "config.hpp"

namespace glfw {

struct logic_error final : std::logic_error {
  using std::logic_error::logic_error;
};

struct runtime_error final : std::runtime_error {
  using std::runtime_error::runtime_error;
};

}  // namespace glfw
