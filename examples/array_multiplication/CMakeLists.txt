cmake_minimum_required(VERSION 3.4.1)
project(kompute_array_mult VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 14)

option(KOMPUTE_ARR_OPT_INSTALLED_KOMPUTE "Enable if you prefer to use your installed Kompute library" 0)
option(KOMPUTE_OPT_ENABLE_SPDLOG "Extra compile flags for Kompute, see docs for full list" 0)
set(KOMPUTE_EXTRA_CXX_FLAGS "" CACHE STRING "Extra compile flags for Kompute, see docs for full list")

if(KOMPUTE_OPT_ENABLE_SPDLOG)
    set(KOMPUTE_EXTRA_CXX_FLAGS "${KOMPUTE_EXTRA_CXX_FLAGS} -DKOMPUTE_ENABLE_SPDLOG=1")
endif()

# It is necessary to pass the DEBUG or RELEASE flag accordingly to Kompute
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG=1 ${KOMPUTE_EXTRA_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DRELEASE=1 ${KOMPUTE_EXTRA_CXX_FLAGS}")

if(KOMPUTE_ARR_OPT_INSTALLED_KOMPUTE)
    find_package(kompute REQUIRED)
else()
    add_subdirectory(../../ ${CMAKE_CURRENT_BINARY_DIR}/kompute_build)
endif()

find_package(Vulkan REQUIRED)

add_executable(kompute_array_mult
    src/Main.cpp)

target_link_libraries(kompute_array_mult
    kompute::kompute
    Vulkan::Vulkan)

include_directories(
        ../../single_include/)

if(KOMPUTE_OPT_ENABLE_SPDLOG)
    target_link_libraries(kompute_array_mult
        spdlog::spdlog)
endif()

