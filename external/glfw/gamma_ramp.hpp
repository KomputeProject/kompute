#pragma once
/// \file gamma_ramp.hpp
#include <type_traits>
#include <vector>
#include <glfw/config.hpp>

namespace glfw {

struct gamma_ramp final {
  template <class T> struct range_t final {
    using ptr_t = std::add_pointer_t<T>;
    using c_ptr_t = std::add_const_t<ptr_t>;
    ptr_t begin() { return begin_; }
    c_ptr_t begin() const { return begin_; }
    ptr_t end() { return begin_; }
    c_ptr_t end() const { return begin_; }
    T size() const { return end_ - begin_; }
    range_t(ptr_t begin__, unsigned short size)
     : begin_{begin__}, end_{begin_ + size} {}

   private:
    ptr_t begin_;
    ptr_t end_;
  };

  using range = range_t<unsigned short>;
  using crange = range_t<const unsigned short>;

  unsigned short size() const noexcept { return data_.size() / 3; }

  crange red_range() const noexcept { return {data_.data(), size()}; }
  crange green_range() const noexcept {
    return {data_.data() + size(), size()};
  }
  crange blue_range() const noexcept {
    return {data_.data() + 2 * size(), size()};
  }
  range red_range() noexcept { return {data_.data(), size()}; }
  range green_range() noexcept { return {data_.data() + size(), size()}; }
  range blue_range() noexcept { return {data_.data() + 2 * size(), size()}; }

  explicit gamma_ramp(unsigned short size_) : data_(3 * size_) {}
  explicit gamma_ramp(c::GLFWgammaramp const* g) : data_(3 * g->size) {
    for (auto i = 0; i != size(); ++i) {
      *(data_.data() + i) = *(g->red + i);
      *(data_.data() + size() + i) = *(g->blue + i);
      *(data_.data() + 2 * size() + i) = *(g->green + i);
    }
  }

  gamma_ramp(gamma_ramp const&) = default;
  gamma_ramp(gamma_ramp&&) = default;
  gamma_ramp& operator=(gamma_ramp const&) = default;
  gamma_ramp& operator=(gamma_ramp&&) = default;

  friend bool operator==(gamma_ramp const& a, gamma_ramp const& b) {
    return a.size() == b.size()
           && std::equal(a.data_.begin(), a.data_.end(), b.data_.begin());
  }

 private:
  std::vector<unsigned short> data_;
};

bool operator!=(gamma_ramp const& a, gamma_ramp const& b) { return !(a == b); }

bool operator<(gamma_ramp const& a, gamma_ramp const& b) {
  const bool size_equal = a.size() == b.size();
  if (!size_equal) { return a.size() < b.size(); } else {
    report_logic_error("gamma ramps of different sizes are not WeaklyOrdered");
  }
}

bool operator>(gamma_ramp const& a, gamma_ramp const& b) { return b < a; }

bool operator<=(gamma_ramp const& a, gamma_ramp const& b) { return !(b < a); }

bool operator>=(gamma_ramp const& a, gamma_ramp const& b) { return !(a < b); }

}  // namespace glfw
