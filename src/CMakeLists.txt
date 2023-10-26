# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20)

if(KOMPUTE_OPT_ANDROID_BUILD)
    find_library(android android)
endif()

cmake_minimum_required(VERSION 3.20)

add_library(kompute Algorithm.cpp
    Manager.cpp
    OpAlgoDispatch.cpp
    OpMemoryBarrier.cpp
    OpTensorCopy.cpp
    OpTensorSyncDevice.cpp
    OpTensorSyncLocal.cpp
    Sequence.cpp
    Tensor.cpp
    Core.cpp)

add_library(kompute::kompute ALIAS kompute)

# Set version for shared libraries.
set_target_properties(kompute
    PROPERTIES
    VERSION ${${PROJECT_NAME}_VERSION}
    SOVERSION ${${PROJECT_NAME}_VERSION_MAJOR})

# Import GNU common install directory variables
include(GNUInstallDirs)

install(TARGETS kompute
    EXPORT komputeTargets # Export kompute to komputeTargets. komputeTargets can hold multiple targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

# Include CMake helpers for package config files
# Follow this installation guideline: https://cmake.org/cmake/help/latest/manual/cmake-packages.7.html
include(CMakePackageConfigHelpers)

configure_package_config_file(${PROJECT_SOURCE_DIR}/cmake/komputeConfig.cmake.in
    "${PROJECT_BINARY_DIR}/kompute/komputeConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/kompute)

# Generated komputeConfigVersion.cmake that is required in the following install command
# This will fix issue #324
write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/kompute/komputeConfigVersion.cmake"
    VERSION ${CMAKE_PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion# All possible values: SameMajorVersion, SameMinorVersion, ExactVersion, AnyNewerVersion
)

install(FILES ${PROJECT_BINARY_DIR}/kompute/komputeConfig.cmake
    ${PROJECT_BINARY_DIR}/kompute/komputeConfigVersion.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/kompute)

# ####################################################
# Linking
# ####################################################
if(KOMPUTE_OPT_ANDROID_BUILD)
    target_link_libraries(kompute PUBLIC vulkanAndroid
        android
        kp_logger
        kp_shader
        fmt::fmt)
else()
    target_link_libraries(kompute PUBLIC Vulkan::Vulkan
        kp_logger
        kp_shader
        fmt::fmt)
endif()

if(KOMPUTE_OPT_BUILD_PYTHON)
    include_directories(${PYTHON_INCLUDE_DIRS})

    target_link_libraries(kompute PRIVATE pybind11::headers ${PYTHON_LIBRARIES})
endif()

if(KOMPUTE_OPT_USE_BUILT_IN_VULKAN_HEADER)
    target_link_libraries(kompute PUBLIC Vulkan-Headers)
endif()

# ####################################################
# Misc
# ####################################################
add_subdirectory(logger)
add_subdirectory(shaders)
add_subdirectory(include)

if (KOMPUTE_OPT_USE_BUILT_IN_FMT)
    # We can't export fmt::fmt because it's alias target, so we unwrap the alias and install it.
    get_target_property(fmt_aliased_target fmt::fmt ALIASED_TARGET)
    install(TARGETS ${fmt_aliased_target}
        EXPORT komputeTargets)
endif ()
if(KOMPUTE_OPT_USE_BUILT_IN_VULKAN_HEADER)
    # We can't export Vulkan::Headers because it's alias target, so we unwrap the alias and install it.
    get_target_property(vk_header_target Vulkan::Headers ALIASED_TARGET)

    # Before installation, reset the INTERFACE_INCLUDE_DIRECTORIES to a generator expression
    # Otherwise INTERFACE_INCLUDE_DIRECTORIES will point to an absolute path even if the software package
    # is copied to another computer. If we don't do this, cmake will complains about it in the configuration step.
    # Get the build interface of include dir
    get_target_property(vk_header_include_dir ${vk_header_target} INTERFACE_INCLUDE_DIRECTORIES)
    #message(STATUS "vk_header_include_dir = ${vk_header_include_dir}")
    # Reset INTERFACE_INCLUDE_DIRECTORIES to a generator expression.
    set_target_properties(${vk_header_target}
        PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "$<INSTALL_INTERFACE:include>;$<BUILD_INTERFACE:${vk_header_include_dir}>")
    #message(STATUS "vk_header_include_dir = ${vk_header_include_di r}")
    install(TARGETS ${vk_header_target}
        EXPORT komputeTargets)
endif ()