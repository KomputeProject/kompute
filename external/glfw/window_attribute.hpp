#pragma once
/// \file window_attribute.hpp Window attributes
#include <glfw/config.hpp>

namespace glfw {

enum class window_attribute : int {
  resizable = GLFW_RESIZABLE,                          //
  visible = GLFW_VISIBLE,                              //
  decorated = GLFW_DECORATED,                          //
  focused = GLFW_FOCUSED,                              //
  iconified = GLFW_ICONIFIED,                          //
  red_bits = GLFW_RED_BITS,                            //
  green_bits = GLFW_GREEN_BITS,                        //
  blue_bits = GLFW_BLUE_BITS,                          //
  alpha_bits = GLFW_ALPHA_BITS,                        //
  depth_bits = GLFW_DEPTH_BITS,                        //
  stencil_bits = GLFW_STENCIL_BITS,                    //
  accum_red_bits = GLFW_ACCUM_RED_BITS,                //
  accum_green_bits = GLFW_ACCUM_GREEN_BITS,            //
  accum_blue_bits = GLFW_ACCUM_BLUE_BITS,              //
  accum_alpha_bits = GLFW_ACCUM_ALPHA_BITS,            //
  aux_buffers = GLFW_AUX_BUFFERS,                      //
  samples = GLFW_SAMPLES,                              //
  refresh_rate = GLFW_REFRESH_RATE,                    //
  stereo = GLFW_STEREO,                                //
  srgb_capable = GLFW_SRGB_CAPABLE,                    //
  client_api = GLFW_CLIENT_API,                        //
  context_version_major = GLFW_CONTEXT_VERSION_MAJOR,  //
  context_version_minor = GLFW_CONTEXT_VERSION_MINOR,  //
  context_robustness = GLFW_CONTEXT_ROBUSTNESS,        //
  opengl_forward_compat = GLFW_OPENGL_FORWARD_COMPAT,  //
  opengl_debug_context = GLFW_OPENGL_DEBUG_CONTEXT,    //
  opengl_profile = GLFW_OPENGL_PROFILE                 //
};

std::string to_string(window_attribute i) {
  switch (i) {
    case window_attribute::resizable: {
      return "window_attribute::resizable";
    }
    case window_attribute::visible: {
      return "window_attribute::visible";
    }
    case window_attribute::decorated: {
      return "window_attribute::decorated";
    }
    case window_attribute::focused: {
      return "window_attribute::focused";
    }
    case window_attribute::iconified: {
      return "window_attribute::iconified";
    }
    case window_attribute::red_bits: {
      return "window_attribute::red_bits";
    }
    case window_attribute::green_bits: {
      return "window_attribute::green_bits";
    }
    case window_attribute::blue_bits: {
      return "window_attribute::blue_bits";
    }
    case window_attribute::alpha_bits: {
      return "window_attribute::alpha_bits";
    }
    case window_attribute::depth_bits: {
      return "window_attribute::depth_bits";
    }
    case window_attribute::stencil_bits: {
      return "window_attribute::stencil_bits";
    }
    case window_attribute::accum_red_bits: {
      return "window_attribute::accum_red_bits";
    }
    case window_attribute::accum_green_bits: {
      return "window_attribute::accum_green_bits";
    }
    case window_attribute::accum_blue_bits: {
      return "window_attribute::accum_blue_bits";
    }
    case window_attribute::accum_alpha_bits: {
      return "window_attribute::accum_alpha_bits";
    }
    case window_attribute::aux_buffers: {
      return "window_attribute::aux_buffers";
    }
    case window_attribute::samples: {
      return "window_attribute::samples";
    }
    case window_attribute::refresh_rate: {
      return "window_attribute::refresh_rate";
    }
    case window_attribute::stereo: {
      return "window_attribute::stereo";
    }
    case window_attribute::srgb_capable: {
      return "window_attribute::srgb_capable";
    }
    case window_attribute::client_api: {
      return "window_attribute::client_api";
    }
    case window_attribute::context_version_major: {
      return "window_attribute::context_version_major";
    }
    case window_attribute::context_version_minor: {
      return "window_attribute::context_version_minor";
    }
    case window_attribute::context_robustness: {
      return "window_attribute::context_robustness";
    }
    case window_attribute::opengl_forward_compat: {
      return "window_attribute::opengl_forward_compat";
    }
    case window_attribute::opengl_debug_context: {
      return "window_attribute::opengl_debug_context";
    }
    case window_attribute::opengl_profile: {
      return "window_attribute::opengl_profile";
    }
    default: { report_logic_error("unkown window_attribute"); }
  }
}

}  // namespace glfw
