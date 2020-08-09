#pragma once
/// \file modifier_keys.hpp Modifier keys
#include <glfw/config.hpp>

namespace glfw {

struct modifier_keys {
  explicit modifier_keys(int mods) : m{mods} {}
  modifier_keys() = default;
  modifier_keys(modifier_keys const&) = default;
  modifier_keys(modifier_keys&&) = default;
  modifier_keys& operator=(modifier_keys const&) = default;
  modifier_keys& operator=(modifier_keys&&) = default;

  bool control() const { return m & GLFW_MOD_CONTROL; }
  bool shift() const { return m & GLFW_MOD_SHIFT; }
  bool alt() const { return m & GLFW_MOD_ALT; }
  bool super() const { return m & GLFW_MOD_SUPER; }

 private:
  int m;
};

}  // namespace glfw
