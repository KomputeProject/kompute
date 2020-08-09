#pragma once
/// \file environment.hpp GLFW environment
#include <map>
#include <mutex>
#include <string>
#include <glfw/config.hpp>
#include <glfw/error_handling.hpp>
#include <glfw/event_queue.hpp>

namespace glfw {

struct environment final {
  environment(c::GLFWerrorfun error_callback = glfw::error_callback) {
    detail::handle_glfw_error(c::glfwInit(),
                              "Couldn't create glfw environment!");
    c::glfwSetErrorCallback(error_callback);
  }
  ~environment() { c::glfwTerminate(); }

  struct version_t {
    int major;
    int minor;
    int rev;
  };

  static version_t version() {
    version_t v;
    c::glfwGetVersion(&v.major, &v.minor, &v.rev);
    return v;
  }

  static std::string version_string() { return c::glfwGetVersionString(); }

  struct static_data {
    std::map<c::GLFWwindow*, event_queue> event_queues;
    std::mutex lock;
  };

 private:
  static static_data data;

  static void key_callback(c::GLFWwindow* w, int k, int scancode, int action,
                           int mods) {
    event e;
    switch (action) {
      case GLFW_PRESS: {
        e.type = event_type::key_pressed;
        break;
      }
      case GLFW_RELEASE: {
        e.type = event_type::key_released;
        break;
      }
      case GLFW_REPEAT: {
        e.type = event_type::key_repeated;
        break;
      }
      default: {
        report_logic_error("unknown action on key");
      }
    }
    e.key.key = static_cast<glfw::key>(k);
    e.key.modifiers = modifier_keys{mods};
    e.key.scancode = scancode;
    environment::data.event_queues[w].push(e);
  }

  static void mouse_button_callback(c::GLFWwindow* w, int button, int action,
                                    int mods) {
    event e;
    switch (action) {
      case GLFW_PRESS: {
        e.type = event_type::mouse_button_pressed;
        break;
      }
      case GLFW_RELEASE: {
        e.type = event_type::mouse_button_released;
        break;
      }
      default: {
        report_logic_error("unknown action on mouse button");
      }
    }
    e.mouse_button.button = static_cast<mouse_button>(button);
    e.mouse_button.modifiers = modifier_keys{mods};
    environment::data.event_queues[w].push(e);
  }

  static void mouse_cursor_position_callback(c::GLFWwindow* w, double x,
                                             double y) {
    event e;
    e.type = event_type::mouse_cursor_moved;
    e.mouse_position = frame_coordinates<double>{x, y};
    environment::data.event_queues[w].push(e);
  }

  static void mouse_cursor_enter_callback(c::GLFWwindow* w, int entered) {
    event e;
    if (entered == GL_TRUE) {
      e.type = event_type::mouse_cursor_entered;
    } else {
      assert(entered == GL_FALSE
             && "logic error: entered is either GL_TRUE or GL_FALSE");
      e.type = event_type::mouse_cursor_left;
    }
    environment::data.event_queues[w].push(e);
  }

  static void mouse_scroll_callback(c::GLFWwindow* w, double x, double y) {
    event e;
    e.type = event_type::mouse_wheel_moved;
    e.mouse_wheel_offset = frame_coordinates<double>{x, y};
    environment::data.event_queues[w].push(e);
  }

  static void character_callback(c::GLFWwindow* w, unsigned int codepoint) {
    event e;
    e.type = event_type::character_entered;
    e.character = codepoint;
    environment::data.event_queues[w].push(e);
  }

  static void window_frame_buffer_size_callback(c::GLFWwindow* w, int width,
                                                int height) {
    event e;
    e.type = event_type::window_frame_buffer_size;
    e.window_frame_size = frame_size<int>{width, height};
    environment::data.event_queues[w].push(e);
  }

  static void window_position_callback(c::GLFWwindow* w, int x, int y) {
    event e;
    e.type = event_type::window_moved;
    e.window_position = frame_coordinates<int>{x, y};
    environment::data.event_queues[w].push(e);
  }

  static void window_size_callback(c::GLFWwindow* w, int width, int height) {
    event e;
    e.type = event_type::window_resized;
    e.window_size = frame_size<int>{width, height};
    environment::data.event_queues[w].push(e);
  }

  static void window_close_callback(c::GLFWwindow* w) {
    event e;
    e.type = event_type::window_closed;
    environment::data.event_queues[w].push(e);
  }

  static void window_refresh_callback(c::GLFWwindow* w) {
    event e;
    e.type = event_type::window_refreshed;
    environment::data.event_queues[w].push(e);
  }

  static void window_focus_callback(c::GLFWwindow* w, int focused) {
    event e;
    if (focused == GL_TRUE) { e.type = event_type::window_focused; } else {
      assert(focused == GL_FALSE
             && "logic error: entered is either GL_TRUE or GL_FALSE");
      e.type = event_type::window_unfocused;
    }
    environment::data.event_queues[w].push(e);
  }

  static void window_minimize_callback(c::GLFWwindow* w, int iconified) {
    event e;
    if (iconified == GL_TRUE) { e.type = event_type::window_minimized; } else {
      assert(iconified == GL_FALSE
             && "logic error: entered is either GL_TRUE or GL_FALSE");
      e.type = event_type::window_restored;
    }
    environment::data.event_queues[w].push(e);
  }

 public:
  static event_queue* register_window(c::GLFWwindow* w) {
    if (!w) { report_runtime_error("cannot register an empty window"); }
    event_queue* q;
    if (data.event_queues.count(w) == 0) {
      std::unique_lock<std::mutex> lock(data.lock);
      q = &data.event_queues[w];
    } else {
      report_logic_error("Trying to register the same window twice");
    }  // find w in map

    // note: cannot test for errors when setting the callbacks since
    // the functions return NULL if an error occurred, _or_, no
    // callback was previously set... which means the error code is useless.
    // (in debug mode, try to set the callback twice, and make sure it
    //  returns what was previously set)

    // set-up window callbacks: key
    c::glfwSetKeyCallback(w, key_callback);
    assert(key_callback == c::glfwSetKeyCallback(w, key_callback));
    // et-up window callbacks: mouse
    c::glfwSetMouseButtonCallback(w, mouse_button_callback);
    assert(mouse_button_callback
           == glfwSetMouseButtonCallback(w, mouse_button_callback));
    c::glfwSetCursorPosCallback(w, mouse_cursor_position_callback);
    assert(mouse_cursor_position_callback
           == glfwSetCursorPosCallback(w, mouse_cursor_position_callback));
    c::glfwSetCursorEnterCallback(w, mouse_cursor_enter_callback);
    assert(mouse_cursor_enter_callback
           == glfwSetCursorEnterCallback(w, mouse_cursor_enter_callback));
    c::glfwSetScrollCallback(w, mouse_scroll_callback);
    assert(mouse_scroll_callback
           == glfwSetScrollCallback(w, mouse_scroll_callback));
    // set-up window callbacks: window
    c::glfwSetFramebufferSizeCallback(w, window_frame_buffer_size_callback);
    assert(
     window_frame_buffer_size_callback
     == glfwSetFramebufferSizeCallback(w, window_frame_buffer_size_callback));
    c::glfwSetWindowPosCallback(w, window_position_callback);
    assert(window_position_callback
           == glfwSetWindowPosCallback(w, window_position_callback));
    c::glfwSetWindowSizeCallback(w, window_size_callback);
    assert(window_size_callback
           == glfwSetWindowSizeCallback(w, window_size_callback));
    c::glfwSetWindowCloseCallback(w, window_close_callback);
    assert(window_close_callback
           == glfwSetWindowCloseCallback(w, window_close_callback));
    c::glfwSetWindowRefreshCallback(w, window_refresh_callback);
    assert(window_refresh_callback
           == glfwSetWindowRefreshCallback(w, window_refresh_callback));
    c::glfwSetWindowFocusCallback(w, window_focus_callback);
    assert(window_focus_callback
           == glfwSetWindowFocusCallback(w, window_focus_callback));
    c::glfwSetWindowIconifyCallback(w, window_minimize_callback);
    assert(window_minimize_callback
           == glfwSetWindowIconifyCallback(w, window_minimize_callback));
    // set-up window callbacks: character
    c::glfwSetCharCallback(w, character_callback);
    assert(character_callback == glfwSetCharCallback(w, character_callback));
    return q;
  }
};

}  // namespace glfw
