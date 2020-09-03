#define CATCH_CONFIG_RUNNER

// clang-format: SPDLOG_ACTIVE_LEVEL must be defined before spdlog.h import
#if DEBUG
#ifndef SPDLOG_ACTIVE_LEVEL
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#endif
#endif

//#include <spdlog/spdlog.h>
//// clang-format: ranges.h must come after spdlog.h
//#include <fmt/ranges.h>

#include "catch2/catch.hpp"

int main( int argc, char* argv[] ) {

  int result = Catch::Session().run( argc, argv );

  // global clean-up...

  return result;
}

