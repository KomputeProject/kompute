#pragma once
/// \file event_type.hpp Event type
#include <glfw/config.hpp>
#include <glfw/error_handling.hpp>

namespace glfw {

enum class event_type : int {
  key_pressed,               //
  key_released,              //
  key_repeated,              //
  mouse_button_pressed,      //
  mouse_button_released,     //
  mouse_cursor_entered,      //
  mouse_cursor_left,         //
  mouse_cursor_moved,        //
  mouse_wheel_moved,         //
  none,                      //
  character_entered,         //
  window_closed,             //
  window_resized,            //
  window_refreshed,          //
  window_moved,              //
  window_focused,            //
  window_unfocused,          //
  window_frame_buffer_size,  //
  window_minimized,          //
  window_restored,           //
};

std::string to_string(event_type e) {
  switch (e) {
    case event_type::key_pressed: {
      return "event_type::key_pressed";
    }
    case event_type::key_released: {
      return "event_type::key_released";
    }
    case event_type::key_repeated: {
      return "event_type::key_repeated";
    }
    case event_type::mouse_button_pressed: {
      return "event_type::mouse_button_pressed";
    }
    case event_type::mouse_button_released: {
      return "event_type::mouse_button_released";
    }
    case event_type::mouse_cursor_entered: {
      return "event_type::mouse_cursor_entered";
    }
    case event_type::mouse_cursor_left: {
      return "event_type::mouse_cursor_left";
    }
    case event_type::mouse_cursor_moved: {
      return "event_type::mouse_cursor_moved";
    }
    case event_type::mouse_wheel_moved: {
      return "event_type::mouse_wheel_moved";
    }
    case event_type::none: {
      return "event_type::none";
    }
    case event_type::character_entered: {
      return "event_type::character_entered";
    }
    case event_type::window_closed: {
      return "event_type::window_closed";
    }
    case event_type::window_resized: {
      return "event_type::window_resized";
    }
    case event_type::window_refreshed: {
      return "event_type::window_refreshed";
    }
    case event_type::window_moved: {
      return "event_type::window_moved";
    }
    case event_type::window_focused: {
      return "event_type::window_focused";
    }
    case event_type::window_unfocused: {
      return "event_type::window_unfocused";
    }
    case event_type::window_frame_buffer_size: {
      return "event_type::window_frame_buffer_size";
    }
    case event_type::window_minimized: {
      return "event_type::window_minimized";
    }
    case event_type::window_restored: {
      return "event_type::window_restored";
    }
    default: { report_runtime_error("unknown event type"); }
  }
}

}  // namespace glfw
