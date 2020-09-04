
enable_testing()
find_package(GTest CONFIG REQUIRED)

file(GLOB test_kompute_CPP
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_executable(test_kompute ${test_kompute_CPP})

target_include_directories(
    test_kompute PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/single_include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/compiled_shaders_include>
)

target_link_libraries(test_kompute PRIVATE 
    GTest::gtest 
    GTest::gtest_main)

target_link_libraries(test_kompute PRIVATE kompute)

add_test(NAME test_kompute COMMAND test_kompute)


