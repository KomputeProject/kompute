cmake_minimum_required(VERSION 3.15)

add_executable(kompute_array_mult main.cpp)
target_link_libraries(kompute_array_mult PRIVATE shader kompute::kompute)
