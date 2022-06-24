cmake_minimum_required(VERSION 3.15)

add_executable(kompute_logistic_regression main.cpp)
target_link_libraries(kompute_logistic_regression PRIVATE shader kompute::kompute)
