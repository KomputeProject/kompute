#pragma once
/// \file mouse_button.hpp Mouse button
#include <glfw/config.hpp>

namespace glfw {

enum class mouse_button : int {
  B1 = GLFW_MOUSE_BUTTON_1,           //
  B2 = GLFW_MOUSE_BUTTON_2,           //
  B3 = GLFW_MOUSE_BUTTON_3,           //
  B4 = GLFW_MOUSE_BUTTON_4,           //
  B5 = GLFW_MOUSE_BUTTON_5,           //
  B6 = GLFW_MOUSE_BUTTON_6,           //
  B7 = GLFW_MOUSE_BUTTON_7,           //
  B8 = GLFW_MOUSE_BUTTON_8,           //
  last = GLFW_MOUSE_BUTTON_LAST,      //
  left = GLFW_MOUSE_BUTTON_LEFT,      //
  right = GLFW_MOUSE_BUTTON_RIGHT,    //
  middle = GLFW_MOUSE_BUTTON_MIDDLE,  //
};

}  // namespace glfw
