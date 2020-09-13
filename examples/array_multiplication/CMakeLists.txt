cmake_minimum_required(VERSION 3.17.0)
project(kompute_array_mult VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

option(KOMPUTE_OPT_ENABLE_SPDLOG "Extra compile flags for Kompute, see docs for full list" 0)
set(KOMPUTE_EXTRA_CXX_FLAGS "" CACHE STRING "Extra compile flags for Kompute, see docs for full list")

if(KOMPUTE_OPT_ENABLE_SPDLOG)
    set(KOMPUTE_EXTRA_CXX_FLAGS "${KOMPUTE_EXTRA_CXX_FLAGS} -DKOMPUTE_ENABLE_SPDLOG=1")
endif()

# It is necessary to pass the DEBUG or RELEASE flag accordingly to Kompute
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG=1 ${KOMPUTE_EXTRA_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DRELEASE=1 ${KOMPUTE_EXTRA_CXX_FLAGS}")

find_package(kompute REQUIRED)
find_package(Vulkan REQUIRED)

if(KOMPUTE_OPT_ENABLE_SPDLOG)
    find_package(spdlog REQUIRED)
    find_package(fmt REQUIRED)
endif()

add_executable(kompute_array_mult
    src/Main.cpp)

target_link_libraries(kompute_array_mult
    kompute::kompute
    Vulkan::Vulkan
)

if(KOMPUTE_OPT_ENABLE_SPDLOG)
    target_link_libraries(kompute_array_mult
        kompute::kompute
        fmt::fmt
        spdlog::spdlog
    )
endif()

