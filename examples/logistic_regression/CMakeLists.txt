cmake_minimum_required(VERSION 3.20)
project(kompute_logistic_regression)

set(CMAKE_CXX_STANDARD 14)

# Options
option(KOMPUTE_OPT_GIT_TAG "The tag of the repo to use for the example" v0.9.0)
option(KOMPUTE_OPT_FROM_SOURCE "Whether to build example from source or from git fetch repo" ON)

if(KOMPUTE_OPT_FROM_SOURCE)
    add_subdirectory(../../ ${CMAKE_CURRENT_BINARY_DIR}/kompute_build)
else()
    include(FetchContent)
    FetchContent_Declare(kompute GIT_REPOSITORY https://github.com/KomputeProject/kompute.git
        GIT_TAG ${KOMPUTE_OPT_GIT_TAG})
    FetchContent_MakeAvailable(kompute)
    include_directories(${kompute_SOURCE_DIR}/src/include)
endif()

# Compiling shader
# To add more shaders simply copy the vulkan_compile_shader command and replace it with your new shader
vulkan_compile_shader(
    INFILE shader/my_shader.comp
    OUTFILE shader/my_shader.hpp
    NAMESPACE "shader")

# Then add it to the library, so you can access it later in your code
add_library(shader INTERFACE "shader/my_shader.hpp")
target_include_directories(shader INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

# Setting up main example code
add_executable(kompute_logistic_regression src/main.cpp)
target_link_libraries(kompute_logistic_regression PRIVATE shader kompute::kompute)
