cmake_minimum_required(VERSION 3.17.0)
project(kompute VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG=1")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DRELEASE=1")


set(CMAKE_VERBOSE_MAKEFILE on)

option(KOMPUTE_OPT_BUILD_TESTS "Enable if you want to build tests" ON)
option(KOMPUTE_OPT_BUILD_DOCS "Enable if you want to build documentation" ON)
option(KOMPUTE_OPT_DEBUG_SYMBOLS "Enable if you want to build debug with symbols" 0)

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
    include(CTest)
    enable_testing()
    add_subdirectory(test)
endif()

