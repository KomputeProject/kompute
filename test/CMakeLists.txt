
#####################################################
#################### GETEST #######################
#####################################################
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

#####################################################
#################### CODECOV  #######################
#####################################################

if (KOMPUTE_OPT_CODE_COVERAGE)
    if(NOT UNIX)
        message(
            FATAL_ERROR
            "KOMPUTE_OPT_CODE_COVERAGE can only be enabled in unix based systems due to limitation on gcov")
    endif()

    add_custom_target(codecov_run_tests
        COMMAND make -C ${PROJECT_SOURCE_DIR} mk_run_tests
        DEPENDS test_kompute)

    add_custom_target(codecov_copy_files
        COMMAND ${CMAKE_COMMAND}
            -E copy_directory
            ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/test_kompute.dir/
            ${CODECOV_DIR}
        COMMAND ${CMAKE_COMMAND}
            -E copy_directory
            ${CMAKE_CURRENT_BINARY_DIR}/../src/CMakeFiles/kompute.dir/
            ${CODECOV_DIR}
        DEPENDS test_kompute codecov_run_tests)

    add_custom_target(codecov_gcov
        COMMAND gcov
            -b -c "*.gcno"
        WORKING_DIRECTORY ${CODECOV_DIR}
        DEPENDS codecov_copy_files)

    add_custom_target(codecov_lcov_capture
        COMMAND lcov
            --capture
            -o ${CODECOV_FILENAME_LCOV_INFO_FULL}
            -d .
        WORKING_DIRECTORY ${CODECOV_DIR}
        DEPENDS codecov_gcov)
    add_custom_target(codecov_lcov_extract
        COMMAND lcov
            --extract
            ${CODECOV_FILENAME_LCOV_INFO_FULL}
            -o ${CODECOV_FILENAME_LCOV_INFO}
            -d .
            "*/src/*"
        WORKING_DIRECTORY ${CODECOV_DIR}
        DEPENDS codecov_lcov_capture)

    add_custom_target(codecov_genhtml
        COMMAND genhtml
            ${CODECOV_FILENAME_LCOV_INFO}
            --output-directory ${CODECOV_DIR_HTML}
        WORKING_DIRECTORY ${CODECOV_DIR}
        DEPENDS codecov_lcov_extract)
endif()

