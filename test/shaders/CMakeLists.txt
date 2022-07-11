# SPDX-License-Identifier: Apache-2.0
# ######################
cmake_minimum_required(VERSION 3.14)

add_library(test_shaders "Utils.cpp"
    "Utils.hpp")

add_subdirectory(glsl)