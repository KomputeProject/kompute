cmake_minimum_required(VERSION 3.20)

file(GLOB SHADERS *.comp)
foreach(SHADER ${SHADERS})
    get_filename_component(SHADER_NAME ${SHADER} NAME_WE)
    set(OUTPUT_FILE_NO_EXT ${CMAKE_CURRENT_BINARY_DIR}/${SHADER_NAME})

    vulkan_compile_shader(INFILE ${SHADER_NAME}.comp
    OUTFILE ${SHADER_NAME}.hpp
    NAMESPACE "shaders"
    RELATIVE_PATH "${kompute_SOURCE_DIR}/cmake")

    
    list(APPEND SHADER_OUTPUTS ${OUTPUT_FILE_NO_EXT}.hpp) 
endforeach()

add_library(shaders INTERFACE  ${SHADER_OUTPUTS})
target_include_directories(shaders INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)
