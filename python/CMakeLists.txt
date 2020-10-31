cmake_minimum_required(VERSION 2.8.12)
project(cmake_example)

add_subdirectory(pybind11)
pybind11_add_module(cmake_example src/main.cpp)
