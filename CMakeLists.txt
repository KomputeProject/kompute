cmake_minimum_required(VERSION 3.4.1)
project(kompute VERSION 0.7.0)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VERBOSE_MAKEFILE on)

# Enable or disable targets
option(KOMPUTE_OPT_BUILD_TESTS "Enable if you want to build tests" 0)
option(KOMPUTE_OPT_CODE_COVERAGE "Enable if you want code coverage" 0)
option(KOMPUTE_OPT_BUILD_DOCS "Enable if you want to build documentation" 0)
option(KOMPUTE_OPT_BUILD_SHADERS "Enable if you want to re-build all shader files" 0)
option(KOMPUTE_OPT_BUILD_SINGLE_HEADER "Enable if you want to build the single header file" 0)
option(KOMPUTE_OPT_INSTALL "Enable if you want to enable installation" 0)
# Build options
option(KOMPUTE_OPT_BUILD_PYTHON "Enable if you want to build python bindings" 0)
option(KOMPUTE_OPT_ENABLE_SPDLOG "Extra compile flags for Kompute, see docs for full list" 0)
option(KOMPUTE_OPT_REPO_SUBMODULE_BUILD, "Use the submodule repos instead of external package manager" 0)
option(KOMPUTE_OPT_ANDOID_BUILD "Enable android compilation flags required" 0)
option(KOMPUTE_OPT_DISABLE_VK_DEBUG_LAYERS "Explicitly disable debug layers even on debug" 0)
option(KOMPUTE_OPT_DISABLE_SHADER_UTILS "Remove shader util code and dependencies including glslang" 0)
option(KOMPUTE_OPT_DEPENDENCIES_SHARED_LIBS "Whether to use shared libraries for dependencies for install" 0)
option(KOMPUTE_OPT_BUILD_AS_SHARED_LIB "Whether to build kompute as shared library" 0)
# Build flags
set(KOMPUTE_EXTRA_CXX_FLAGS "" CACHE STRING "Extra compile flags for Kompute, see docs for full list")

if(KOMPUTE_OPT_ENABLE_SPDLOG)
    set(KOMPUTE_EXTRA_CXX_FLAGS "${KOMPUTE_EXTRA_CXX_FLAGS} -DKOMPUTE_ENABLE_SPDLOG=1")
    set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "Enables external fmt as its current dep" FORCE)
    if(KOMPUTE_OPT_INSTALL)
        # Enable install parameters for spdlog (overrides parameters passed)
        set(SPDLOG_INSTALL ON CACHE BOOL "Enables install of spdlot" FORCE)

        if(KOMPUTE_OPT_DEPENDENCIES_SHARED_LIBS)
            set(SPDLOG_BUILD_SHARED ON CACHE BOOL "Enables build of shared libraries" FORCE)
        endif()
    endif()
endif()

if(KOMPUTE_OPT_INSTALL)
	# Enable install parameters for fmt (overrides parameters passed)
	set(FMT_INSTALL ON CACHE BOOL "Enables install of fmt" FORCE)
endif()

if(KOMPUTE_OPT_ANDOID_BUILD)
    set(KOMPUTE_EXTRA_CXX_FLAGS "${KOMPUTE_EXTRA_CXX_FLAGS} -DVK_USE_PLATFORM_ANDROID_KHR")
endif()

if(KOMPUTE_OPT_BUILD_PYTHON)
    set(KOMPUTE_EXTRA_CXX_FLAGS "${KOMPUTE_EXTRA_CXX_FLAGS} -DKOMPUTE_BUILD_PYTHON")
endif()

if(KOMPUTE_OPT_DISABLE_VK_DEBUG_LAYERS)
    set(KOMPUTE_EXTRA_CXX_FLAGS "${KOMPUTE_EXTRA_CXX_FLAGS} -DKOMPUTE_DISABLE_VK_DEBUG_LAYERS=1")
endif()

if(NOT KOMPUTE_OPT_DISABLE_SHADER_UTILS)
    if(KOMPUTE_OPT_INSTALL)
        # Enable install parameters for glslang (overrides parameters passed)
        # When install is enabled the glslang libraries become shared
        set(ENABLE_GLSLANG_INSTALL ON CACHE BOOL "Enables install of glslang" FORCE)

        # By default we enable shared library based installation
        if(KOMPUTE_OPT_DEPENDENCIES_SHARED_LIBS)
            set(BUILD_SHARED_LIBS ON CACHE BOOL "Enables build of shared libraries" FORCE)
        endif()
    endif()
else()
    set(KOMPUTE_EXTRA_CXX_FLAGS "${KOMPUTE_EXTRA_CXX_FLAGS} -DKOMPUTE_DISABLE_SHADER_UTILS=1")
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
