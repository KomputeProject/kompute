
enable_testing()
if(KOMPUTE_OPT_REPO_SUBMODULE_BUILD)
    add_subdirectory(${PROJECT_SOURCE_DIR}/external/googletest EXCLUDE_FROM_ALL
        ${CMAKE_CURRENT_BINARY_DIR}/kompute_googletest)
else()
    find_package(GTest CONFIG REQUIRED)
endif()

file(GLOB test_kompute_CPP
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_executable(test_kompute ${test_kompute_CPP})

target_include_directories(
    test_kompute PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/single_include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/compiled_shaders_include>
)

if(KOMPUTE_OPT_REPO_SUBMODULE_BUILD)
    target_include_directories(
        test_kompute PRIVATE
        ${gtest_SOURCE_DIR}/include)

    target_link_libraries(test_kompute PRIVATE
        gtest_main)
else()
    target_link_libraries(test_kompute PRIVATE 
        GTest::gtest)
endif()

target_link_libraries(test_kompute PRIVATE kompute)

add_test(NAME test_kompute COMMAND test_kompute)


