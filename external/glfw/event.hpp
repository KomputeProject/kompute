#pragma once
/// \file event.hpp Event
#include <glfw/config.hpp>
#include <glfw/event_type.hpp>
#include <glfw/frame_coordinates.hpp>
#include <glfw/frame_size.hpp>
#include <glfw/key.hpp>
#include <glfw/modifier_keys.hpp>
#include <glfw/mouse_button.hpp>

namespace glfw {

struct event {
  event_type type;
  explicit operator event_type() const { return type; }

 private:
  struct key_t {
    glfw::key key;
    glfw::modifier_keys modifiers;
    int scancode;
  };
  struct mouse_button_t {
    glfw::mouse_button button;
    glfw::modifier_keys modifiers;
  };

 public:
  union {
    key_t key;
    mouse_button_t mouse_button;
    frame_coordinates<double> mouse_wheel_offset;
    frame_coordinates<double> mouse_position;
    frame_size<int> window_frame_size;
    frame_coordinates<int> window_position;
    frame_size<int> window_size;
    unsigned int character;
  };
};

}  // namespace glfw
