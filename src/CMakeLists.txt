# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.15)

if(KOMPUTE_OPT_ANDROID_BUILD)
    find_library(android android)
endif()

cmake_minimum_required(VERSION 3.15)

add_library(kompute Algorithm.cpp
                    Manager.cpp
                    OpAlgoDispatch.cpp
                    OpMemoryBarrier.cpp
                    OpTensorCopy.cpp
                    OpTensorSyncDevice.cpp
                    OpTensorSyncLocal.cpp
                    Sequence.cpp
                    Tensor.cpp)

add_library(kompute::kompute ALIAS kompute)

# Set version for shared libraries.
set_target_properties(kompute
     PROPERTIES
     VERSION ${${PROJECT_NAME}_VERSION}
     SOVERSION ${${PROJECT_NAME}_VERSION_MAJOR})

# Import GNU common install directory variables
include(GNUInstallDirs)

if(CPR_FORCE_USE_SYSTEM_CURL)
    install(TARGETS kompute
            EXPORT komputeTargets
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

    # Include CMake helpers for package config files
    # Follow this installation guideline: https://cmake.org/cmake/help/latest/manual/cmake-packages.7.html
    include(CMakePackageConfigHelpers)

    configure_package_config_file(${PROJECT_SOURCE_DIR}/cmake/komputeConfig.cmake.in
                                  "${PROJECT_BINARY_DIR}/kompute/komputeConfig.cmake"
                                  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/kompute)

    install(EXPORT komputeTargets
            FILE komputeTargets.cmake
            NAMESPACE kompute::
	        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/kompute)

    install(FILES ${PROJECT_BINARY_DIR}/kompute/komputeConfig.cmake
            ${PROJECT_BINARY_DIR}/kompute/komputeConfigVersion.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/kompute)
     
else()
    install(TARGETS kompute
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()

#####################################################
# Android
#####################################################

if(KOMPUTE_OPT_ANDROID_BUILD)
    set(VK_ANDROID_COMMON_DIR ${ANDROID_NDK}/sources/third_party/vulkan/src/common)
    set(VK_ANDROID_PATCH_DIR ${PROJECT_SOURCE_DIR}/vk_ndk_wrapper_include/)
    set(VK_ANDROID_INCLUDE_DIR ${ANDROID_NDK}/sources/third_party/vulkan/src/include)

    include_directories(${VK_ANDROID_COMMON_DIR}
                        ${VK_ANDROID_PATCH_DIR}
                        ${VK_ANDROID_INCLUDE_DIR})

    add_library(kompute_vk_ndk_wrapper STATIC ${PROJECT_SOURCE_DIR}/vk_ndk_wrapper_include/kompute_vk_ndk_wrapper.cpp)
endif()

#####################################################
# Linking
#####################################################

if(KOMPUTE_OPT_ANDROID_BUILD)
    target_link_libraries(kompute PUBLIC kompute_vk_ndk_wrapper
                                         android
                                  PRIVATE kp_logger
                                          fmt::fmt)
else()
    target_link_libraries(kompute PUBLIC Vulkan::Vulkan
                                  PRIVATE fmt::fmt
                                          kp_logger)
endif()

if(KOMPUTE_OPT_USE_BUILD_IN_VULKAN_HEADER)
    target_link_libraries(kompute PUBLIC Vulkan-Headers)
endif()

#####################################################
# Misc
#####################################################
add_subdirectory(logger)
add_subdirectory(include)
