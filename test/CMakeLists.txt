# SPDX-License-Identifier: Apache-2.0
#######################
cmake_minimum_required(VERSION 3.15)

#####################################################
# Shaders
#####################################################
add_subdirectory(shaders)

#####################################################
# Tests
#####################################################

macro(add_kompute_test _TEST_NAME)
    add_executable("${_TEST_NAME}_tests" "Test${_TEST_NAME}.cpp"
                                         ${ARGN})
    target_link_libraries("${_TEST_NAME}_tests" PRIVATE GTest::GTest
                                                        kompute::kompute
                                                        kp_logger
                                                        test_shaders
                                                        test_shaders_glsl)
    add_test(NAME "kompute_${_TEST_NAME}_tests" COMMAND "${_TEST_NAME}_tests")
    # Group under the "tests" project folder in IDEs such as Visual Studio.
    set_property(TARGET ${_TEST_NAME}_tests PROPERTY FOLDER "tests")
    if(WIN32 AND BUILD_SHARED_LIBS)
        add_custom_command(TARGET ${_TEST_NAME}_tests POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:kompute> $<TARGET_FILE_DIR:${_TEST_NAME}_tests>)
    endif()
endmacro()

add_kompute_test(AsyncOperations)
add_kompute_test(Destroy)
add_kompute_test(LogisticRegression)
add_kompute_test(Manager)
add_kompute_test(MultipleAlgoExecutions)
add_kompute_test(OpShadersFromStringAndFile)
add_kompute_test(OpTensorCopy)
add_kompute_test(OpTensorCreate)
add_kompute_test(PushConstant)
add_kompute_test(Sequence)
add_kompute_test(SpecializationConstant)
add_kompute_test(Workgroup)
