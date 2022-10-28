cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(kompute GIT_REPOSITORY https://github.com/COM8/kompute.git
                             GIT_TAG 675f6dc771cea044ead99a5467a9b817c2d8feb6) # The commit hash for a dev version before v0.9.0. Replace with the latest from: https://github.com/KomputeProject/kompute/releases
set(KOMPUTE_OPT_ANDROID_BUILD ON)
set(KOMPUTE_OPT_DISABLE_VK_DEBUG_LAYERS ON)
FetchContent_MakeAvailable(kompute)
include_directories(${kompute_SOURCE_DIR}/src/include)

# Add to the list, so CMake can later find the code to compile shaders to header files
list(APPEND CMAKE_PREFIX_PATH "${kompute_SOURCE_DIR}/cmake")
add_subdirectory(shader)

add_library(kompute-jni SHARED KomputeJniNative.cpp
                               KomputeModelML.cpp
                               KomputeModelML.hpp)

target_link_libraries(kompute-jni PRIVATE kompute::kompute shader log android)

