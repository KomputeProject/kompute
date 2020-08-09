#pragma once
/// \file video_mode.hpp
#include <glfw/config.hpp>
#include <glfw/error_handling.hpp>
#include <glfw/frame_size.hpp>

namespace glfw {

struct video_mode final {
  int blue_bits() const noexcept { return v.blueBits; }
  int green_bits() const noexcept { return v.greenBits; }
  int red_bits() const noexcept { return v.redBits; }
  int height() const noexcept { return v.height; }
  int width() const noexcept { return v.width; }
  int refresh_rate() const noexcept { return v.refreshRate; }
  frame_size<int> size() const noexcept { return {width(), height()}; }

  int& blue_bits() noexcept { return v.blueBits; }
  int& green_bits() noexcept { return v.greenBits; }
  int& red_bits() noexcept { return v.redBits; }
  int& height() noexcept { return v.height; }
  int& width() noexcept { return v.width; }
  int& refresh_rate() noexcept { return v.refreshRate; }
  frame_size<int&> size() noexcept { return {width(), height()}; }

  video_mode() {}
  explicit video_mode(c::GLFWvidmode const* other_ptr) {
    blue_bits() = other_ptr->blueBits;
    green_bits() = other_ptr->greenBits;
    red_bits() = other_ptr->redBits;
    height() = other_ptr->height;
    width() = other_ptr->width;
    refresh_rate() = other_ptr->refreshRate;
  }
  video_mode(video_mode const&) = default;
  video_mode(video_mode&&) = default;
  video_mode& operator=(video_mode const&) = default;
  video_mode& operator=(video_mode&&) = default;

 private:
  c::GLFWvidmode v;
};

bool operator==(video_mode const& a, video_mode const& b) {
  return a.blue_bits() == b.blue_bits() && a.green_bits() == b.green_bits()
         && a.red_bits() == b.red_bits() && a.height() == b.height()
         && a.width() == b.width() && a.refresh_rate() == b.refresh_rate();
}

bool operator!=(video_mode const& a, video_mode const& b) { return !(a == b); }

/// If the #of bits per pixel is equal, WeakTotalOrdering on (width, height),
/// otherwise, WeakTotalordering based on the #of bits per pixel
///
/// If the #of bits per channel differs, and the number of bits per channel is
/// not equal, there is no ordering (reports logic_error).
bool operator<(video_mode const& a, video_mode const& b) {
  const bool equal_bits = a.blue_bits() == b.blue_bits()
                          && a.green_bits() == b.green_bits()
                          && a.red_bits() == b.red_bits();

  if (equal_bits) {
    if (a.width() == b.width()) { return a.height() < b.height(); } else {
      return a.width() < b.width();
    }
  } else {
    const bool a_equal_bits = a.blue_bits() == a.green_bits()
                              && a.blue_bits() == a.red_bits();

    const bool b_equal_bits = a.blue_bits() == a.green_bits()
                              && a.blue_bits() == a.red_bits();

    if (!a_equal_bits || !b_equal_bits) {
      report_logic_error("video modes with different #of bits per pixel per "
                         "channel are WeaklyOrdered");
    } else { return a.blue_bits() < b.blue_bits(); }
  }
}

bool operator>(video_mode const& a, video_mode const& b) { return b < a; }

bool operator<=(video_mode const& a, video_mode const& b) { return !(b < a); }

bool operator>=(video_mode const& a, video_mode const& b) { return !(a < b); }

}  // namespace glfw
