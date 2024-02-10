# SPDX-License-Identifier: Apache-2.0
# ######################
cmake_minimum_required(VERSION 3.20)

# ####################################################
# Shaders
# ####################################################
add_subdirectory(shaders)

# ####################################################
# Tests
# ####################################################
add_executable(kompute_benchmark
    TestBenchmark.cpp)

target_link_libraries(kompute_benchmark PRIVATE GTest::gtest_main
    kompute::kompute
    kp_logger
    test_benchmark_shaders)
add_test(NAME kompute_benchmark COMMAND kompute_benchmark)

# Group under the "tests" project folder in IDEs such as Visual Studio.
set_property(TARGET kompute_benchmark PROPERTY FOLDER "tests")

if(WIN32 AND BUILD_SHARED_LIBS) # Install dlls in the same directory as the executable on Windows so one can simply double click them
    add_custom_command(TARGET kompute_benchmark POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:test_shaders> $<TARGET_FILE_DIR:kompute_benchmark>)
    add_custom_command(TARGET kompute_benchmark POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:kompute::kompute> $<TARGET_FILE_DIR:kompute_benchmark>)
    add_custom_command(TARGET kompute_benchmark POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:spdlog> $<TARGET_FILE_DIR:kompute_benchmark>)
    add_custom_command(TARGET kompute_benchmark POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:kp_logger> $<TARGET_FILE_DIR:kompute_benchmark>)
endif()


