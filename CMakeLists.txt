# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.4.1)
project(kompute VERSION 0.8.1)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VERBOSE_MAKEFILE on)

# Enable or disable targets
option(KOMPUTE_OPT_BUILD_TESTS "Enable if you want to build tests" OFF)
option(KOMPUTE_OPT_CODE_COVERAGE "Enable if you want code coverage" OFF)
option(KOMPUTE_OPT_BUILD_DOCS "Enable if you want to build documentation" OFF)
option(KOMPUTE_OPT_BUILD_SHADERS "Enable if you want to re-build all shader files" OFF)
option(KOMPUTE_OPT_BUILD_SINGLE_HEADER "Enable if you want to build the single header file" OFF)
option(KOMPUTE_OPT_INSTALL "Enable if you want to enable installation" OFF)

# Build options
option(KOMPUTE_OPT_BUILD_PYTHON "Enable if you want to build python bindings" OFF)
option(KOMPUTE_OPT_ENABLE_SPDLOG "Enable to compile with spdlog as the internal logging framework" OFF)
option(KOMPUTE_OPT_ANDROID_BUILD "Enable android compilation flags required" OFF)
option(KOMPUTE_OPT_DISABLE_VK_DEBUG_LAYERS "Explicitly disable debug layers even on debug" OFF)
option(KOMPUTE_OPT_DEPENDENCIES_SHARED_LIBS "Whether to use shared libraries for dependencies for install" OFF)
option(KOMPUTE_OPT_BUILD_AS_SHARED_LIB "Whether to build kompute as shared library" OFF)
option(KOMPUTE_OPT_DISABLE_VULKAN_VERSION_CHECK "Whether to check if your driver supports the Vulkan Header version you are linking against. This might be useful in case you build shared on a different system than you run later." OFF)

# External komponents
option(KOMPUTE_OPT_USE_BUILD_IN_SPDLOG "Use the build in version of Spdlog" ON)
option(KOMPUTE_OPT_USE_BUILD_IN_FMT "Use the build in version of fmt" ON)
option(KOMPUTE_OPT_USE_BUILD_IN_GOOGLE_TEST "Use the build in version of GoogleTest" ON)
option(KOMPUTE_OPT_USE_BUILD_IN_PYBIND11 "Use the build in version of pybind11" ON)
option(KOMPUTE_OPT_USE_BUILD_IN_VULKAN_HEADER "Use the build in version of Vulkan Headers. This could be helpful in case your system Vulkan Headers are to new for your driver. If you set this to false, please make sure your system Vulkan Header are supported by your driver." ON)
set(KOMPUTE_OPT_BUILD_IN_VULKAN_HEADER_TAG "v1.2.203" CACHE STRING "The git tag used for the build in Vulkan Headers when 'KOMPUTE_OPT_USE_BUILD_IN_VULKAN_HEADER' is enabled. A list of tags can be found here: https://github.com/KhronosGroup/Vulkan-Headers/tags")

# Build flags
set(KOMPUTE_EXTRA_CXX_FLAGS "" CACHE STRING "Extra compile flags for Kompute, see docs for full list")

#####################################################
#################### Deprecated Options #############
#####################################################
if(KOMPUTE_OPT_REPO_SUBMODULE_BUILD)
    message(FATAL_ERROR "'KOMPUTE_OPT_REPO_SUBMODULE_BUILD' got replaced by 'KOMPUTE_OPT_USE_BUILD_IN_SPDLOG', 'KOMPUTE_OPT_USE_BUILD_IN_FMT', 'KOMPUTE_OPT_USE_BUILD_IN_GOOGLE_TEST', 'KOMPUTE_OPT_USE_BUILD_IN_PYBIND11' and 'KOMPUTE_OPT_USE_BUILD_IN_VULKAN_HEADER'. Please use them instead.")
endif()

#####################################################
#################### Dependencies ###################
#####################################################
include(FetchContent)
include(cmake/check_vulkan_version.cmake)

# Vulkan Header
# We don't import Vulkan library if Android build as its build dynamically
# Otherwise it is expected that the Vulkan SDK and dependencies are installed
if(NOT KOMPUTE_OPT_ANDROID_BUILD)
    find_package(Vulkan REQUIRED)
endif()
if(KOMPUTE_OPT_USE_BUILD_IN_VULKAN_HEADER)
    FetchContent_Declare(vulkan_header GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Headers.git
                                       GIT_TAG ${KOMPUTE_OPT_BUILD_IN_VULKAN_HEADER_TAG}) # Source: https://github.com/KhronosGroup/Vulkan-Headers/tags
    FetchContent_MakeAvailable(vulkan_header)
    
    if(NOT KOMPUTE_OPT_DISABLE_VULKAN_VERSION_CHECK)
        # Ensure the driver supports this Vulkan version
        check_vulkan_version(INCLUDE_DIR "${vulkan_header_SOURCE_DIR}/include")
    endif()
else()
    if(NOT KOMPUTE_OPT_DISABLE_VULKAN_VERSION_CHECK)
        # Ensure the driver supports this Vulkan version
        check_vulkan_version(INCLUDE_DIR ${Vulkan_INCLUDE_DIR})
    endif()
endif()

# Spdlog
if(KOMPUTE_OPT_ENABLE_SPDLOG)
    if(KOMPUTE_OPT_USE_BUILD_IN_SPDLOG)
        set(SPDLOG_INSTALL ${KOMPUTE_OPT_INSTALL})
        set(SPDLOG_BUILD_SHARED ${KOMPUTE_OPT_DEPENDENCIES_SHARED_LIBS})

        FetchContent_Declare(spdlog GIT_REPOSITORY https://github.com/gabime/spdlog.git
                                    GIT_TAG v1.10.0) # Source: https://github.com/gabime/spdlog/releases
        FetchContent_MakeAvailable(spdlog)
    else()
        find_package(spdlog REQUIRED)
    endif()
endif()

# fmt
if(KOMPUTE_OPT_USE_BUILD_IN_FMT)
    set(FMT_INSTALL ${KOMPUTE_OPT_INSTALL})
    set(BUILD_SHARED_LIBS_BKP ${KOMPUTE_OPT_DEPENDENCIES_SHARED_LIBS})
    set(SPDLOG_BUILD_SHARED ${KOMPUTE_OPT_DEPENDENCIES_SHARED_LIBS})
    FetchContent_Declare(fmt GIT_REPOSITORY https://github.com/fmtlib/fmt.git
                             GIT_TAG 8.1.1) # Source: https://github.com/fmtlib/fmt/releases
    FetchContent_MakeAvailable(fmt)
    set(BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS_BKP})
else()
    find_package(fmt REQUIRED)
endif()

# GoogleTest
if(KOMPUTE_OPT_BUILD_TESTS)
    if(KOMPUTE_OPT_USE_BUILD_IN_GOOGLE_TEST)
        FetchContent_Declare(googletest GIT_REPOSITORY https://github.com/google/googletest.git
                                        GIT_TAG release-1.11.0) # Source: https://github.com/google/googletest/releases
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
    if(KOMPUTE_OPT_USE_BUILD_IN_PYBIND11)
        FetchContent_Declare(pybind GIT_REPOSITORY https://github.com/pybind/pybind11.git
                                    GIT_TAG v2.9.2) # Source: https://github.com/pybind/pybind11/releases
        FetchContent_MakeAvailable(pybind)
    else()
        find_package(pybind11 REQUIRED)
    endif()
    find_package(PythonLibs REQUIRED)
endif()

#####################################################
#################### Other Options ##################
#####################################################

if(KOMPUTE_OPT_ANDROID_BUILD)
    set(KOMPUTE_EXTRA_CXX_FLAGS "${KOMPUTE_EXTRA_CXX_FLAGS} -DVK_USE_PLATFORM_ANDROID_KHR")
endif()

if(KOMPUTE_OPT_BUILD_PYTHON)
    set(KOMPUTE_EXTRA_CXX_FLAGS "${KOMPUTE_EXTRA_CXX_FLAGS} -DKOMPUTE_BUILD_PYTHON")
endif()

if(KOMPUTE_OPT_DISABLE_VK_DEBUG_LAYERS)
    set(KOMPUTE_EXTRA_CXX_FLAGS "${KOMPUTE_EXTRA_CXX_FLAGS} -DKOMPUTE_DISABLE_VK_DEBUG_LAYERS=1")
endif()

if(KOMPUTE_OPT_INSTALL)
    # Enable install parameters for glslang (overrides parameters passed)
    # When install is enabled the glslang libraries become shared
    set(ENABLE_GLSLANG_INSTALL ON CACHE BOOL "Enables install of glslang" FORCE)

    # By default we enable shared library based installation
    if(KOMPUTE_OPT_DEPENDENCIES_SHARED_LIBS)
        set(BUILD_SHARED_LIBS ON CACHE BOOL "Enables build of shared libraries" FORCE)
    endif()
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG=1 ${KOMPUTE_EXTRA_CXX_FLAGS} -DUSE_DEBUG_EXTENTIONS")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DRELEASE=1 ${KOMPUTE_EXTRA_CXX_FLAGS}")

if(KOMPUTE_OPT_CODE_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage --coverage")

    set(CODECOV_DIR
        ${CMAKE_CURRENT_BINARY_DIR}/codecov/)
    set(CODECOV_DIR_LCOV
        ${CODECOV_DIR}lcov/)
    set(CODECOV_FILENAME_LCOV_INFO
        lcov.info)
    set(CODECOV_FILENAME_LCOV_INFO_FULL
        lcov_full.info)
    set(CODECOV_DIR_HTML
        ${CODECOV_DIR}html/)
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

if(KOMPUTE_OPT_BUILD_TESTS)
    add_subdirectory(test)
endif()

if(KOMPUTE_OPT_BUILD_DOCS)
    set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/config" ${CMAKE_MODULE_PATH})
    add_subdirectory(docs)
endif()

if(KOMPUTE_OPT_BUILD_PYTHON)
    add_subdirectory(python)
endif()
