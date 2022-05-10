# SPDX-License-Identifier: Apache-2.0

if(KOMPUTE_OPT_ANDROID_BUILD)
    find_library(android android)
endif()

if(KOMPUTE_OPT_BUILD_SHADERS)
# all shaders are compiled into cpp files
    kompute_make(build_shaders
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/include)
endif()

if(KOMPUTE_OPT_BUILD_SINGLE_HEADER)
# all headers are compiled into a single header
    kompute_make(build_single_header
        OUTPUT ${PROJECT_SOURCE_DIR}/single_include)
endif()

file(GLOB kompute_CPP
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

if(KOMPUTE_OPT_ANDROID_BUILD)
    set(VK_ANDROID_COMMON_DIR ${ANDROID_NDK}/sources/third_party/vulkan/src/common)
    set(VK_ANDROID_PATCH_DIR ${PROJECT_SOURCE_DIR}/vk_ndk_wrapper_include/)
    set(VK_ANDROID_INCLUDE_DIR ${ANDROID_NDK}/sources/third_party/vulkan/src/include)

    include_directories(
        ${VK_ANDROID_COMMON_DIR}
        ${VK_ANDROID_PATCH_DIR}
        ${VK_ANDROID_INCLUDE_DIR})

    add_library(kompute_vk_ndk_wrapper STATIC
        ${PROJECT_SOURCE_DIR}/vk_ndk_wrapper_include/kompute_vk_ndk_wrapper.cpp)
endif()

if(NOT KOMPUTE_OPT_BUILD_AS_SHARED_LIB)
    add_library(
        kompute STATIC
        ${kompute_CPP})
else()
    add_library(
        kompute SHARED
        ${kompute_CPP})
endif()

target_include_directories(
    kompute PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/single_include>
)

if(NOT KOMPUTE_OPT_ANDROID_BUILD)
    target_link_libraries(
        kompute
        Vulkan::Vulkan
        fmt::fmt
    )
else()
    target_link_libraries(
        kompute
        fmt::fmt
    )
endif()

#####################################################
#################### SPDLOG #########################
#####################################################

if(KOMPUTE_OPT_ENABLE_SPDLOG)
    target_link_libraries(
        kompute
        spdlog::spdlog
    )
endif()

#####################################################
#################### Android ########################
#####################################################

if(KOMPUTE_OPT_ANDROID_BUILD)
    target_link_libraries(
        kompute
        kompute_vk_ndk_wrapper
        log
        android
    )
endif()

#####################################################
########## Built C++ Header SHADERS #################
#####################################################

if(KOMPUTE_OPT_BUILD_SHADERS)
    add_dependencies(kompute
        build_shaders)
endif()

#####################################################
#################### Single Header ##################
#####################################################

if(KOMPUTE_OPT_BUILD_SINGLE_HEADER)
    add_dependencies(kompute
        build_single_header)
endif()


add_library(kompute::kompute ALIAS kompute)

if(KOMPUTE_OPT_INSTALL)
    install(TARGETS kompute EXPORT KomputeTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        INCLUDES DESTINATION include)

    target_include_directories(kompute PUBLIC $<INSTALL_INTERFACE:include>)

    install(DIRECTORY include/ DESTINATION include)

    install(DIRECTORY ${PROJECT_SOURCE_DIR}/single_include/
        DESTINATION include)

    install(EXPORT KomputeTargets
        FILE komputeConfig.cmake
        NAMESPACE kompute::
        DESTINATION lib/cmake/kompute)
endif()
