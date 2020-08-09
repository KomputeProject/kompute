#pragma once
/// \file frame_size.hpp Frame size type

namespace glfw {

template <class T> struct frame_size final {
  T width;
  T height;
};

}  // namespace glfw
