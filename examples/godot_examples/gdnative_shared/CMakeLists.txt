cmake_minimum_required(VERSION 3.17.0)
project(kompute_linear_reg VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 14)

set(KOMPUTE_EXTRA_CXX_FLAGS "" CACHE STRING "Extra compile flags for Kompute, see docs for full list")

# It is necessary to pass the DEBUG or RELEASE flag accordingly to Kompute
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG=1 ${KOMPUTE_EXTRA_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DRELEASE=1 ${KOMPUTE_EXTRA_CXX_FLAGS}")

find_package(kompute REQUIRED)
find_package(Vulkan REQUIRED)

add_executable(kompute_linear_reg
    src/*.cpp)

target_link_libraries(kompute_linear_reg
    kompute::kompute
    Vulkan::Vulkan
)

target_link_libraries(kompute_linear_reg
    "lib/godot.windows.tools.64.lib"
)

