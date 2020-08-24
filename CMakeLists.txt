cmake_minimum_required(VERSION 3.18.0)
project(kompute VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Custom commands
option(KOMPUTE_BUILD_TESTS "Enable if you want to build tests" ON)

add_subdirectory(src)

if(KOMPUTE_BUILD_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(test)
endif()

