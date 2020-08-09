#pragma once
/// \file event_queue.hpp Event queue
#include <boost/lockfree/queue.hpp>
#include <glfw/config.hpp>
#include <glfw/event.hpp>

namespace glfw {

using event_queue
 = boost::lockfree::queue<event,
                          boost::lockfree::capacity<GLFW_CXX_MAX_EVENTS>>;

}  // namespace glfw
