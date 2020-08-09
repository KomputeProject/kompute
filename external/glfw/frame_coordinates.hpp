#pragma once
/// \file frame_coordinates.hpp Frame coordinates

namespace glfw {

template <class T> struct frame_coordinates final {
  T x;
  T y;
};

}  // namespace glfw
