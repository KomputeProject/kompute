cmake_minimum_required(VERSION 3.4.1)

add_subdirectory(../../../../../../../ ${CMAKE_CURRENT_BINARY_DIR}/kompute_build)

set(ANDROID_APP_GLUE_DIR ${ANDROID_NDK}/sources/android/native_app_glue)
set(VK_ANDROID_COMMON_DIR ${ANDROID_NDK}/sources/third_party/vulkan/src/common)
set(VK_ANDROID_INCLUDE_DIR ${ANDROID_NDK}/sources/third_party/vulkan/src/include)

# build native_app_glue as a static lib
include_directories(${ANDROID_APP_GLUE_DIR})
add_library(app-glue STATIC
        ${ANDROID_APP_GLUE_DIR}/android_native_app_glue.c)

# build vulkan app
add_library(kompute_android SHARED
            main.cpp)

include_directories(
        ${VK_ANDROID_COMMON_DIR}
        ${VK_ANDROID_INCLUDE_DIR}
        ../../../../../../../single_include/
        ../../../../../../../vulkan_ndk_patch_include/)

# TODO: Explore:
# * -DVK_NO_PROTOTYPES
# * -DUSE_DEBUG_EXTENTIONS
# * -Wno-unused-variable
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14      \
                     -DVK_USE_PLATFORM_ANDROID_KHR -DUSE_DEBUG_EXTENTIONS")

set(CMAKE_SHARED_LINKER_FLAGS
        "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

target_link_libraries(kompute_android
        kompute
        app-glue
        log
        android_vulkan_wrapper
        android)