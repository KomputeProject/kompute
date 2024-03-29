# Code coverage
set(CMAKE_BUILD_TYPE COVERAGE CACHE INTERNAL "Coverage build enabled")
message(STATUS "Enabling gcov support")

if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(COVERAGE_FLAG "--coverage")
endif()

set(CMAKE_CXX_FLAGS_COVERAGE
    "-g -O0 ${COVERAGE_FLAG} -fprofile-arcs -ftest-coverage"
    CACHE STRING "Flags used by the C++ compiler during coverage builds."
    FORCE)
set(CMAKE_C_FLAGS_COVERAGE
    "-g -O0 ${COVERAGE_FLAG} -fprofile-arcs -ftest-coverage"
    CACHE STRING "Flags used by the C compiler during coverage builds."
    FORCE)
set(CMAKE_EXE_LINKER_FLAGS_COVERAGE
    ""
    CACHE STRING "Flags used for linking binaries during coverage builds."
    FORCE)
set(CMAKE_SHARED_LINKER_FLAGS_COVERAGE
    ""
    CACHE STRING "Flags used by the shared libraries linker during coverage builds."
    FORCE)

set(CODECOV_DIR ${CMAKE_CURRENT_BINARY_DIR}/codecov/)
set(CODECOV_DIR_LCOV ${CODECOV_DIR}lcov/)
set(CODECOV_FILENAME_LCOV_INFO lcov.info)
set(CODECOV_FILENAME_LCOV_INFO_FULL lcov_full.info)
set(CODECOV_DIR_HTML ${CODECOV_DIR}html/)

mark_as_advanced(CMAKE_CXX_FLAGS_COVERAGE
    CMAKE_C_FLAGS_COVERAGE
    CMAKE_EXE_LINKER_FLAGS_COVERAGE
    CMAKE_SHARED_LINKER_FLAGS_COVERAGE)

add_custom_target(codecov_run_tests
    COMMAND make -C ${PROJECT_SOURCE_DIR} mk_run_tests
    DEPENDS kompute_tests)

add_custom_target(codecov_copy_files
    COMMAND ${CMAKE_COMMAND}
        -E copy_directory
        ${CMAKE_CURRENT_BINARY_DIR}/test/CMakeFiles/kompute_tests.dir/
        ${CODECOV_DIR}
    COMMAND ${CMAKE_COMMAND}
        -E copy_directory
        ${CMAKE_CURRENT_BINARY_DIR}/src/CMakeFiles/kompute.dir/
        ${CODECOV_DIR}
    DEPENDS kompute_tests codecov_run_tests)

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
