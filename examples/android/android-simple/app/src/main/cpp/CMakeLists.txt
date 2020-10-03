cmake_minimum_required(VERSION 3.4.1)

add_subdirectory(../../../../../../../ ${CMAKE_CURRENT_BINARY_DIR}/kompute_build)

set(ANDROID_APP_GLUE_DIR ${ANDROID_NDK}/sources/android/native_app_glue)
set(VK_ANDROID_COMMON_DIR ${ANDROID_NDK}/sources/third_party/vulkan/src/common)
set(VK_ANDROID_INCLUDE_DIR ${ANDROID_NDK}/sources/third_party/vulkan/src/include)

## build native_app_glue as a static lib
#include_directories(${ANDROID_APP_GLUE_DIR})
#add_library(app-glue STATIC
#        ${ANDROID_APP_GLUE_DIR}/android_native_app_glue.c)

# build vulkan app
add_library(kompute-jni SHARED
        kompute-jni.cpp)

include_directories(
        ${VK_ANDROID_COMMON_DIR}
        ${VK_ANDROID_INCLUDE_DIR}
        ../../../../../../../single_include/
        ../../../../../../../vk_ndk_wrapper_include/)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14      \
                     -DVK_USE_PLATFORM_ANDROID_KHR=1 -DKOMPUTE_DISABLE_VK_DEBUG_LAYERS=1")

#set(CMAKE_SHARED_LINKER_FLAGS
#        "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

target_link_libraries(kompute-jni
        kompute
#        app-glue
        log
        kompute_vk_ndk_wrapper
        android)