cmake_minimum_required(VERSION 3.4.1)

add_subdirectory(../../../../../../../ ${CMAKE_CURRENT_BINARY_DIR}/kompute_build)

set(VK_ANDROID_INCLUDE_DIR ${ANDROID_NDK}/sources/third_party/vulkan/src/include)

add_library(kompute-jni SHARED
        KomputeJniNative.cpp
        KomputeModelML.cpp)

include_directories(
        ${VK_ANDROID_COMMON_DIR}
        ${VK_ANDROID_INCLUDE_DIR}
        ../../../../../../../single_include/
        ../../../../../../../vk_ndk_wrapper_include/)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 \
                     -DVK_USE_PLATFORM_ANDROID_KHR=1 \
                     -DKOMPUTE_DISABLE_VK_DEBUG_LAYERS=1")

target_link_libraries(kompute-jni
        # Libraries from kompute build
        kompute
        kompute_vk_ndk_wrapper
        # Libraries from android build
        log
        android)