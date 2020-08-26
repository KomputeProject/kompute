#define CATCH_CONFIG_RUNNER

// clang-format: SPDLOG_ACTIVE_LEVEL must be defined before spdlog.h import
#if DEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#endif

#include <spdlog/spdlog.h>
// clang-format: ranges.h must come after spdlog.h
#include <fmt/ranges.h>

#include "catch2/catch.hpp"

int main( int argc, char* argv[] ) {

#if DEBUG
    spdlog::set_level(spdlog::level::debug);
#else
    spdlog::set_level(spdlog::level::info);
#endif

  int result = Catch::Session().run( argc, argv );

  // global clean-up...

  return result;
}

