# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20)
project(kompute VERSION 0.9.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)

# Only change the folder behavior if kompute is not a subproject
if(${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
    set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
endif()

# Avoid the dll boilerplate code for windows
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

set(KOMPUTE_LIBRARIES kompute CACHE INTERNAL "")

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(kompute_opt_install_default_val ON)
else ()
    set(kompute_opt_install_default_val OFF)
endif ()

# ####################################################
# Options
# ####################################################
macro(kompute_option OPTION_NAME OPTION_TEXT OPTION_DEFAULT)
    option(${OPTION_NAME} ${OPTION_TEXT} ${OPTION_DEFAULT})

    if(DEFINED ENV{${OPTION_NAME}})
        # Allow overriding the option through an environment variable
        set(${OPTION_NAME} $ENV{${OPTION_NAME}})
    endif()

    if(${OPTION_NAME})
        add_definitions(-D${OPTION_NAME})
    endif()

    message(STATUS "  ${OPTION_NAME}: ${${OPTION_NAME}}")
endmacro()

macro(kompute_log_level OPTION_NAME OPTION_TEXT OPTION_DEFAULT)
    set(${OPTION_NAME} ${OPTION_DEFAULT} CACHE STRING ${OPTION_TEXT})
    set_property(CACHE ${OPTION_NAME} PROPERTY STRINGS "Trace" "Debug" "Info" "Warn" "Error" "Critical" "Default" "Off")

    if(DEFINED ENV{${OPTION_NAME}})
        # Allow setting the option through an environment variable
        set(${OPTION_NAME} $ENV{${OPTION_NAME}})
    endif()

    if(${OPTION_NAME})
        add_definitions(-D${OPTION_NAME})
    endif()

    # Allow disabling logging completely and prevent linking against it:
    if(${KOMPUTE_OPT_LOG_LEVEL} STREQUAL "Off")
        set(${OPTION_NAME}_DISABLED ON)
        add_compile_definitions(${OPTION_NAME}_DISABLED=1)
    endif()

    message(STATUS "  ${OPTION_NAME}: ${${OPTION_NAME}}")
endmacro()

macro(kompute_option_string OPTION_NAME OPTION_TEXT OPTION_DEFAULT)
    set(${OPTION_NAME} ${OPTION_DEFAULT} CACHE STRING ${OPTION_TEXT})

    if(DEFINED ENV{${OPTION_NAME}})
        # Allow setting the option through an environment variable
        set(${OPTION_NAME} $ENV{${OPTION_NAME}})
    endif()

    if(${OPTION_NAME})
        add_definitions(-D${OPTION_NAME})
    endif()

    message(STATUS "  ${OPTION_NAME}: ${${OPTION_NAME}}")
endmacro()

message(STATUS "General purpose GPU compute framework built on Vulkan")
message(STATUS "=======================================================")

# Enable or disable targets
kompute_option(KOMPUTE_OPT_BUILD_TESTS "Enable if you want to build tests." OFF)
kompute_option(KOMPUTE_OPT_ENABLE_BENCHMARK "Enable if you want to build and enable benchmark." OFF)
kompute_option(KOMPUTE_OPT_CODE_COVERAGE "Enable if you want code coverage." OFF)
kompute_option(KOMPUTE_OPT_BUILD_DOCS "Enable if you want to build documentation." OFF)
kompute_option(KOMPUTE_OPT_INSTALL "Enable if you want to enable installation." ${kompute_opt_install_default_val})

# Build options
kompute_option(KOMPUTE_OPT_BUILD_PYTHON "Enable if you want to build python bindings." OFF)
kompute_log_level(KOMPUTE_OPT_LOG_LEVEL "Internally we use Spdlog or fmt for logging, depending on the value of 'KOMPUTE_OPT_USE_SPDLOG'. The log level used can be changed here. Possible values: 'Trace', 'Debug', 'Info', 'Warn', 'Error', 'Critical', 'Off', 'Default'. If set to 'Off' logging will be deactivated completely. If set to 'Default', the log level will be set to 'Info' for release builds and 'Debug' else." "Default")
kompute_option(KOMPUTE_OPT_USE_SPDLOG "If enabled, logging via KP_LOG_<DEBUG, INFO, etc...> will happen through Spdlog instead of plan fmt." OFF)
kompute_option(KOMPUTE_OPT_ANDROID_BUILD "Enable android compilation flags required." OFF)
kompute_option(KOMPUTE_OPT_DISABLE_VK_DEBUG_LAYERS "Explicitly disable debug layers even on debug." OFF)
kompute_option(KOMPUTE_OPT_DISABLE_VULKAN_VERSION_CHECK "Whether to check if your driver supports the Vulkan Header version you are linking against. This might be useful in case you build shared on a different system than you run later." OFF)
kompute_option(KOMPUTE_OPT_BUILD_SHADERS "Rebuilds all compute shaders during compilation and does not use the already precompiled versions. Requires glslangValidator to be installed on your system." OFF)

# External components
kompute_option(KOMPUTE_OPT_USE_BUILT_IN_SPDLOG "Use the built-in version of Spdlog. Requires 'KOMPUTE_OPT_USE_SPDLOG' to be set to ON in order to have any effect." ON)
kompute_option(KOMPUTE_OPT_SPDLOG_ASYNC_MODE "If spdlog is enabled this allows for selecting whether the default logger setup creates sync or async logger" OFF)
kompute_option(KOMPUTE_OPT_USE_BUILT_IN_FMT "Use the built-in version of fmt." ON)
kompute_option(KOMPUTE_OPT_USE_BUILT_IN_GOOGLE_TEST "Use the built-in version of GoogleTest." ON)
kompute_option(KOMPUTE_OPT_USE_BUILT_IN_PYBIND11 "Use the built-in version of pybind11." ON)
kompute_option(KOMPUTE_OPT_USE_BUILT_IN_VULKAN_HEADER "Use the built-in version of Vulkan Headers. This could be helpful in case your system Vulkan Headers are too new for your driver. If you set this to OFF, please make sure your system Vulkan Headers are supported by your driver." ON)
kompute_option_string(KOMPUTE_OPT_BUILT_IN_VULKAN_HEADER_TAG "The git tag used for the built-in Vulkan Headers when 'KOMPUTE_OPT_USE_BUILT_IN_VULKAN_HEADER' is enabled. A list of tags can be found here: https://github.com/KhronosGroup/Vulkan-Headers/tags" "v1.3.231")
message(STATUS "=======================================================")

# ####################################################
# Deprecated Options
# ####################################################
include(cmake/deprecation_warnings.cmake)

# ####################################################
# Dependencies
# ####################################################
include(cmake/vulkan_shader_compiler.cmake)
include(cmake/check_vulkan_version.cmake)
include(FetchContent)

# Set -fPIC so that ../lib/kp.cpython-310-x86_64-linux-gnu.so links correctly
if(KOMPUTE_OPT_BUILD_PYTHON)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

# Vulkan Header
# We don't import Vulkan library if Android build as it is built dynamically
# Otherwise it is expected that the Vulkan SDK and dependencies are installed
# Has to happen AFTER using the build-in Vulkan headers to prevent multiple targets with the name Vulkan::Headers
if(KOMPUTE_OPT_ANDROID_BUILD)
    add_library(vulkanAndroid INTERFACE)
    if(NOT DEFINED VULKAN_INCLUDE_DIR)
        message(FATAL_ERROR "VULKAN_INCLUDE_DIR is not set. Please set it to the Vulkan SDK include directory.")
    endif()
    target_sources(vulkanAndroid INTERFACE ${VULKAN_INCLUDE_DIR}/vulkan/vulkan.hpp)
    target_include_directories(vulkanAndroid INTERFACE ${VULKAN_INCLUDE_DIR})

    target_compile_definitions(vulkanAndroid INTERFACE VK_NO_PROTOTYPES=1)
    target_compile_definitions(vulkanAndroid INTERFACE VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)
else()
    if(KOMPUTE_OPT_USE_BUILT_IN_VULKAN_HEADER)
        FetchContent_Declare(vulkan_header GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Headers.git
            GIT_TAG ${KOMPUTE_OPT_BUILT_IN_VULKAN_HEADER_TAG}
            GIT_SHALLOW 1) # Source: https://github.com/KhronosGroup/Vulkan-Headers/tags
        FetchContent_MakeAvailable(vulkan_header)

        if(NOT KOMPUTE_OPT_DISABLE_VULKAN_VERSION_CHECK)
            # Ensure the driver supports this Vulkan version
            check_vulkan_version(INCLUDE_DIR "${vulkan_header_SOURCE_DIR}/include")
        endif()
    endif()

    find_package(Vulkan REQUIRED)

    if(Vulkan_FOUND AND NOT TARGET Vulkan::Headers)
        add_library(Vulkan::Headers INTERFACE IMPORTED)
        set_target_properties(Vulkan::Headers PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${Vulkan_INCLUDE_DIRS}")
    endif()

    if(NOT KOMPUTE_OPT_USE_BUILT_IN_VULKAN_HEADER AND NOT KOMPUTE_OPT_DISABLE_VULKAN_VERSION_CHECK)
        # Ensure the driver supports this Vulkan version
        check_vulkan_version(INCLUDE_DIR ${Vulkan_INCLUDE_DIR})
    endif()
endif()

# Spdlog
if(KOMPUTE_OPT_USE_SPDLOG)
    add_compile_definitions(KOMPUTE_OPT_USE_SPDLOG=1)

    if(KOMPUTE_OPT_USE_BUILT_IN_SPDLOG)
        set(SPDLOG_INSTALL ${KOMPUTE_OPT_INSTALL})
        set(SPDLOG_BUILD_SHARED ${BUILD_SHARED_LIBS})

        FetchContent_Declare(spdlog GIT_REPOSITORY https://github.com/gabime/spdlog.git
            GIT_TAG v1.10.0
            GIT_SHALLOW 1) # Source: https://github.com/gabime/spdlog/releases
        FetchContent_MakeAvailable(spdlog)
    else()
        find_package(spdlog REQUIRED)
    endif()
endif()

# fmt
if(KOMPUTE_OPT_USE_BUILT_IN_FMT)
    set(FMT_INSTALL ${KOMPUTE_OPT_INSTALL})
    FetchContent_Declare(fmt GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 11.0.0
        GIT_SHALLOW 1) # Source: https://github.com/fmtlib/fmt/releases
    FetchContent_MakeAvailable(fmt)
else()
    find_package(fmt REQUIRED)
endif()

# GoogleTest
if(KOMPUTE_OPT_BUILD_TESTS OR KOMPUTE_OPT_ENABLE_BENCHMARK)
    if(KOMPUTE_OPT_USE_BUILT_IN_GOOGLE_TEST)
        FetchContent_Declare(googletest GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG release-1.11.0) # Source: https://github.com/google/googletest/releases

        # Use a shared C runtime in case we build shared
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(googletest)

        add_library(gtest_int INTERFACE)
        target_link_libraries(gtest_int INTERFACE gtest)
        target_include_directories(gtest_int INTERFACE ${googletest_SOURCE_DIR}/include)

        add_library(GTest::GTest ALIAS gtest_int)

        # Group under the "tests/gtest" project folder in IDEs such as Visual Studio.
        set_property(TARGET gtest PROPERTY FOLDER "tests/gtest")
        set_property(TARGET gtest_main PROPERTY FOLDER "tests/gtest")
    else()
        find_package(GTest CONFIG REQUIRED)
    endif()
endif()

# pybind11
if(KOMPUTE_OPT_BUILD_PYTHON)
    if(KOMPUTE_OPT_USE_BUILT_IN_PYBIND11)
        FetchContent_Declare(pybind GIT_REPOSITORY https://github.com/pybind/pybind11.git
            GIT_TAG v2.9.2
            GIT_SHALLOW 1) # Source: https://github.com/pybind/pybind11/releases
        FetchContent_MakeAvailable(pybind)
    else()
        find_package(pybind11 REQUIRED)
    endif()

    find_package(PythonLibs REQUIRED)
endif()

# ####################################################
# Preprocessor Macros
# ####################################################
if(KOMPUTE_OPT_ANDROID_BUILD)
    add_compile_definitions(VK_USE_PLATFORM_ANDROID_KHR=1)
endif()

if(KOMPUTE_OPT_BUILD_PYTHON)
    add_compile_definitions(KOMPUTE_BUILD_PYTHON=1)
endif()

if(KOMPUTE_OPT_DISABLE_VK_DEBUG_LAYERS)
    add_compile_definitions(KOMPUTE_DISABLE_VK_DEBUG_LAYERS=1)
endif()

# ####################################################
# Misc Options
# ####################################################
if(KOMPUTE_OPT_INSTALL)
    # Enable install parameters for glslang (overrides parameters passed)
    # When install is enabled the glslang libraries become shared
    set(ENABLE_GLSLANG_INSTALL ON CACHE BOOL "Enables install of glslang" FORCE)
endif()

if(MSVC)
    # MSVC is true when the compiler is msvc(cl) or clang-cl, both receive msvc-like commandline flags
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror")
endif()

# If glslang is cloned, then SPIRV/GlslangToSpv.h will be used instead of glslang/SPIRV/GlslangToSpv.h
# As after installation, SPIRV/ header files will be found in glslang/SPIRV/ , more info in #193
if(KOMPUTE_OPT_REPO_SUBMODULE_BUILD)
    add_definitions(-DUSE_EXTERNAL_GLSLANG)
endif()

# Allow scripts to call main kompute Makefile
function(kompute_make KOMPUTE_MAKE_TARGET)
    add_custom_target(${KOMPUTE_MAKE_TARGET}
        COMMAND make -C ${PROJECT_SOURCE_DIR} ${KOMPUTE_MAKE_TARGET})
endfunction()

add_subdirectory(src)

if(KOMPUTE_OPT_BUILD_TESTS OR KOMPUTE_OPT_ENABLE_BENCHMARK)
    enable_testing()
endif()

if(KOMPUTE_OPT_BUILD_TESTS)
    add_subdirectory(test)
endif()

if(KOMPUTE_OPT_ENABLE_BENCHMARK)
    add_subdirectory(benchmark)
endif()

if(KOMPUTE_OPT_CODE_COVERAGE)
    if(NOT UNIX)
        message(FATAL_ERROR "KOMPUTE_OPT_CODE_COVERAGE can only be enabled in unix based systems due to limitation on gcov.")
    endif()

    include(cmake/code_coverage.cmake)
endif()

if(KOMPUTE_OPT_BUILD_DOCS)
    set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/config" ${CMAKE_MODULE_PATH})
    add_subdirectory(docs)
endif()

if(KOMPUTE_OPT_BUILD_PYTHON)
    add_subdirectory(python)
endif()

if(KOMPUTE_OPT_INSTALL)
    # Many other targets may be exported to komputeTargets in subdirs, so install the export in the last
    # Generates komputeTargets.cmake, which is required by komputeConfig.cmake in a find_package call.
    install(EXPORT komputeTargets
        FILE komputeTargets.cmake
        NAMESPACE kompute::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/kompute)

    # Copy CMake files needed to `vulkan_compile_shader`
    install(FILES
        cmake/vulkan_shader_compiler.cmake
        cmake/bin_file_to_header.cmake
        cmake/bin2h.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/kompute)
endif()


include(cmake/set_package_info.cmake)
