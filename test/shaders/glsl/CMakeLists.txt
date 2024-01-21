# SPDX-License-Identifier: Apache-2.0
# ######################
cmake_minimum_required(VERSION 3.20)

vulkan_compile_shader(INFILE test_logistic_regression_shader.comp
    OUTFILE test_logistic_regression_shader.hpp
    NAMESPACE "kp")

vulkan_compile_shader(INFILE test_op_custom_shader.comp
    OUTFILE test_op_custom_shader.hpp
    NAMESPACE "kp")

vulkan_compile_shader(INFILE test_workgroup_shader.comp
    OUTFILE test_workgroup_shader.hpp
    NAMESPACE "kp")

vulkan_compile_shader(INFILE test_shader.comp
    OUTFILE test_shader.hpp
    NAMESPACE "kp")

add_library(test_shaders_glsl INTERFACE test_logistic_regression_shader.hpp
    test_op_custom_shader.hpp
    test_workgroup_shader.hpp
    test_shader.hpp)

target_include_directories(test_shaders_glsl INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
