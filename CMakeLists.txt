cmake_minimum_required(VERSION 3.17.0)
project(kompute VERSION 0.3.0)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VERBOSE_MAKEFILE on)

# Enable or disable targets
option(KOMPUTE_OPT_BUILD_TESTS "Enable if you want to build tests" 1)
option(KOMPUTE_OPT_BUILD_DOCS "Enable if you want to build documentation" 1)
option(KOMPUTE_OPT_ENABLE_SPDLOG "Extra compile flags for Kompute, see docs for full list" 1)

set(KOMPUTE_EXTRA_CXX_FLAGS "" CACHE STRING "Extra compile flags for Kompute, see docs for full list")

if(KOMPUTE_OPT_ENABLE_SPDLOG)
    set(KOMPUTE_EXTRA_CXX_FLAGS "${KOMPUTE_EXTRA_CXX_FLAGS} -DKOMPUTE_ENABLE_SPDLOG=1")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG=1 ${KOMPUTE_EXTRA_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DRELEASE=1 ${KOMPUTE_EXTRA_CXX_FLAGS}")

# Allow scripts to call main kompute Makefile
function(kompute_make KOMPUTE_MAKE_TARGET)
    add_custom_target(${KOMPUTE_MAKE_TARGET}
        COMMAND make -C ${PROJECT_SOURCE_DIR} ${KOMPUTE_MAKE_TARGET})
endfunction()

add_subdirectory(src)

if(KOMPUTE_OPT_BUILD_DOCS)
    set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/config" ${CMAKE_MODULE_PATH})
    add_subdirectory(docs)
endif()

if(KOMPUTE_OPT_BUILD_TESTS)
    add_subdirectory(test)
endif()

