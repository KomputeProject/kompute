// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include <kompute/Kompute.hpp>

int
main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);

#if KOMPUTE_ENABLE_SPDLOG
    spdlog::set_level(
      static_cast<spdlog::level::level_enum>(KOMPUTE_LOG_LEVEL));
#endif

    return RUN_ALL_TESTS();
}
