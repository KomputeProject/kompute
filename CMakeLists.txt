cmake_minimum_required(VERSION 3.18.0)
project(kompute VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(KOMPUTE_OPT_INSTALL_PYTHON_REQS "Enable if you want to install python reqs" OFF)
option(KOMPUTE_OPT_BUILD_TESTS "Enable if you want to build tests" ON)

# Allow scripts to call main kompute Makefile
function(kompute_make KOMPUTE_MAKE_TARGET)
    add_custom_target(${KOMPUTE_MAKE_TARGET}
        COMMAND make -C ${PROJECT_SOURCE_DIR} ${KOMPUTE_MAKE_TARGET} ${ARGN})
endfunction()

add_subdirectory(src)

if(KOMPUTE_OPT_BUILD_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(test)
endif()

