cmake_minimum_required(VERSION 3.18.0)
project(kompute VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(Vulkan REQUIRED)

file(GLOB kompute_SRC
    "${CMAKE_CURRENT_LIST_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/shaders/*.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/shaders/*.cpp"
)

add_executable(kompute
    ${kompute_SRC})

target_include_directories(
    kompute PUBLIC
    fmt
    spdlog
    Vulkan
)

target_link_libraries(
    kompute 
    fmt::fmt
    spdlog::spdlog
    Vulkan::Vulkan
)

