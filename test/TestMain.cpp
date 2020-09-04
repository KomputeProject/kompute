
#include <gmock/gmock.h>

#include <kompute/Kompute.hpp>

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    testing::InitGoogleMock(&argc, argv);

#if KOMPUTE_ENABLE_SPDLOG
    spdlog::set_level(static_cast<spdlog::level::level_enum>(SPDLOG_ACTIVE_LEVEL));
    spdlog::error("default active level {}", SPDLOG_ACTIVE_LEVEL);
#endif

    return RUN_ALL_TESTS();
}
