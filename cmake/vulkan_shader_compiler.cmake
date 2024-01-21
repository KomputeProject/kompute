# Setting CWD as later on it's not possible if called from outside
set_property(GLOBAL PROPERTY
    VULKAN_COMPILE_SHADER_CWD_PROPERTY "${CMAKE_CURRENT_LIST_DIR}")

function(vulkan_compile_shader)
     find_program(GLS_LANG_VALIDATOR_PATH NAMES glslangValidator)
     if(GLS_LANG_VALIDATOR_PATH STREQUAL "GLS_LANG_VALIDATOR_PATH-NOTFOUND")
          message(FATAL_ERROR "glslangValidator not found.")
          return()
     endif()

     cmake_parse_arguments(SHADER_COMPILE "" "INFILE;OUTFILE;NAMESPACE" "" ${ARGN})
     set(SHADER_COMPILE_INFILE_FULL "${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_COMPILE_INFILE}")
     set(SHADER_COMPILE_SPV_FILE_FULL "${CMAKE_CURRENT_BINARY_DIR}/${SHADER_COMPILE_INFILE}.spv")
     set(SHADER_COMPILE_HEADER_FILE_FULL "${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_COMPILE_OUTFILE}")

     # .comp -> .spv
     add_custom_command(OUTPUT "${SHADER_COMPILE_SPV_FILE_FULL}"
                        COMMAND "${GLS_LANG_VALIDATOR_PATH}"
                        ARGS "-V"
                             "${SHADER_COMPILE_INFILE_FULL}"
                             "-o"
                             "${SHADER_COMPILE_SPV_FILE_FULL}"
                        COMMENT "Compile vulkan compute shader from file '${SHADER_COMPILE_INFILE_FULL}' to '${SHADER_COMPILE_SPV_FILE_FULL}'."
                        MAIN_DEPENDENCY "${SHADER_COMPILE_INFILE_FULL}")

     # Check if big or little endian
     include (TestBigEndian)
     TEST_BIG_ENDIAN(IS_BIG_ENDIAN)

     # .spv -> .hpp
     ## Property needs to be retrieved explicitly from globals
     get_property(VULKAN_COMPILE_SHADER_CWD GLOBAL PROPERTY VULKAN_COMPILE_SHADER_CWD_PROPERTY)
     ## The directory may not be created so we need to ensure its present 
     get_filename_component(SHADER_COMPILE_SPV_PATH ${SHADER_COMPILE_SPV_FILE_FULL} DIRECTORY)
     if(NOT EXISTS ${SHADER_COMPILE_SPV_PATH})
         add_custom_target(build-time-make-directory ALL
             COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADER_COMPILE_SPV_PATH})
     endif()
     ## Requires custom command function as this is the only way to call 
     ## a function during compile time from cmake (ie through cmake script)
     add_custom_command(OUTPUT "${SHADER_COMPILE_HEADER_FILE_FULL}"
                        COMMAND ${CMAKE_COMMAND}
                        ARGS "-DINPUT_SHADER_FILE=${SHADER_COMPILE_SPV_FILE_FULL}"
                             "-DOUTPUT_HEADER_FILE=${SHADER_COMPILE_HEADER_FILE_FULL}"
                             "-DHEADER_NAMESPACE=${SHADER_COMPILE_NAMESPACE}"
                             "-DIS_BIG_ENDIAN=${IS_BIG_ENDIAN}"
                             "-P"
                             "${VULKAN_COMPILE_SHADER_CWD}/bin_file_to_header.cmake"
                        WORKING_DIRECTORY "${VULKAN_COMPILE_SHADER_CWD}"
                        COMMENT "Converting compiled shader '${SHADER_COMPILE_SPV_FILE_FULL}' to header file '${SHADER_COMPILE_HEADER_FILE_FULL}'."
                        MAIN_DEPENDENCY "${SHADER_COMPILE_SPV_FILE_FULL}")
endfunction()
