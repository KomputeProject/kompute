#pragma once
/// \file config.hpp Includes the GLFW into the c namespace

namespace glfw {

namespace c {

#define GLFW_NO_GLU
#include <GLFW/glfw3.h>
#undef GLFW_NO_GLU

}  // namespace c

}  // namespace glfw

#ifndef GLFW_CXX_MAX_EVENTS
/// Max number of events per window
#define GLFW_CXX_MAX_EVENTS 1024
#endif
