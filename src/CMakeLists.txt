
if(KOMPUTE_OPT_ANDOID_BUILD)
    find_library(android android)
endif()

# We don't import Vulkan library if Android build as its build dynamically
# Otherwise it is expected that the Vulkan SDK and dependencies are installed 
if(NOT KOMPUTE_OPT_ANDOID_BUILD)
    find_package(Vulkan REQUIRED)
endif()

if(KOMPUTE_OPT_BUILD_SHADERS)
# all shaders are compiled into cpp files
    kompute_make(build_shaders 
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/include)
endif()

if(KOMPUTE_OPT_BUILD_SINGLE_HEADER)
# all headers are compiled into a single header
    kompute_make(build_single_header
        OUTPUT ${PROJECT_SOURCE_DIR}/single_include)
endif()

file(GLOB kompute_CPP
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

if(KOMPUTE_OPT_ANDOID_BUILD)
    set(VK_ANDROID_COMMON_DIR ${ANDROID_NDK}/sources/third_party/vulkan/src/common)
    set(VK_ANDROID_PATCH_DIR ${PROJECT_SOURCE_DIR}/vk_ndk_wrapper_include/)
    set(VK_ANDROID_INCLUDE_DIR ${ANDROID_NDK}/sources/third_party/vulkan/src/include)

    include_directories(
        ${VK_ANDROID_COMMON_DIR}
        ${VK_ANDROID_PATCH_DIR}
        ${VK_ANDROID_INCLUDE_DIR})

    add_library(kompute_vk_ndk_wrapper STATIC
        ${PROJECT_SOURCE_DIR}/vk_ndk_wrapper_include/kompute_vk_ndk_wrapper.cpp)
endif()

if(NOT KOMPUTE_OPT_BUILD_AS_SHARED_LIB)
    add_library(
        kompute STATIC
        ${kompute_CPP})
else()
    add_library(
        kompute SHARED
        ${kompute_CPP})
endif()

target_include_directories(
    kompute PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/single_include
)

if(NOT KOMPUTE_OPT_ANDOID_BUILD)
    target_link_libraries(
        kompute
        Vulkan::Vulkan
    )
else()
    target_link_libraries(
        kompute 
    )
endif()

if(KOMPUTE_OPT_REPO_SUBMODULE_BUILD)
# Override the default Vulkan::Vulkan headers
# In this case we only use the build interface due to https://github.com/KhronosGroup/Vulkan-Headers/issues/157
    add_subdirectory(${PROJECT_SOURCE_DIR}/external/Vulkan-Headers ${CMAKE_CURRENT_BINARY_DIR}/kompute_vulkan_headers)
    get_target_property(VULKAN_HEADERS_INCLUDES Vulkan-Headers INTERFACE_INCLUDE_DIRECTORIES)
    target_include_directories(
        kompute PUBLIC
        $<BUILD_INTERFACE:${VULKAN_HEADERS_INCLUDES}>)
endif()

#####################################################
#################### fmt #######################
#####################################################

if(KOMPUTE_OPT_REPO_SUBMODULE_BUILD)
    add_subdirectory(${PROJECT_SOURCE_DIR}/external/fmt ${CMAKE_CURRENT_BINARY_DIR}/kompute_fmt)
else()
    find_package(fmt REQUIRED)
endif()

target_link_libraries(
    kompute 
    fmt::fmt
)

#####################################################
#################### SPDLOG #######################
#####################################################

if(KOMPUTE_OPT_ENABLE_SPDLOG)
    if(KOMPUTE_OPT_REPO_SUBMODULE_BUILD)
        add_subdirectory(${PROJECT_SOURCE_DIR}/external/spdlog ${CMAKE_CURRENT_BINARY_DIR}/kompute_spdlog)
    else()
        find_package(spdlog REQUIRED)
    endif()

    target_link_libraries(
        kompute 
        spdlog::spdlog
    )
endif()

#####################################################
#################### Android #######################
#####################################################

if(KOMPUTE_OPT_ANDOID_BUILD)
    target_link_libraries(
        kompute 
        kompute_vk_ndk_wrapper
        log
        android
    )
endif()

#####################################################
########## Built C++ Header SHADERS #################
#####################################################

if(KOMPUTE_OPT_BUILD_SHADERS)
    add_dependencies(kompute
        build_shaders)
endif()

#####################################################
#################### Single Header #######################
#####################################################

if(KOMPUTE_OPT_BUILD_SINGLE_HEADER)
    add_dependencies(kompute
        build_single_header)
endif()

#####################################################
#################### GLSLANG #######################
#####################################################

if(NOT KOMPUTE_OPT_DISABLE_SHADER_UTILS)
    if(KOMPUTE_OPT_REPO_SUBMODULE_BUILD)
        add_subdirectory(${PROJECT_SOURCE_DIR}/external/glslang
            ${CMAKE_CURRENT_BINARY_DIR}/kompute_glslang)

        target_include_directories(
            kompute PRIVATE
            ${PROJECT_SOURCE_DIR}/external/glslang)

        target_link_libraries(kompute
            # Not including hlsl support
            # HLSL
            # glslang includes OGLCompiler, OSDependent, MachineIndependent
            glslang
            SPIRV)
    else()
        find_package(glslang CONFIG REQUIRED)

        target_include_directories(
            kompute PRIVATE
            ${GLSLANG_GENERATED_INCLUDEDIR})

        target_link_libraries(kompute
            # Not including hlsl support
            # glslang::HLSL
            # Adding explicit dependencies to match above
            glslang::glslang
            glslang::SPIRV)
    endif()
endif()


add_library(kompute::kompute ALIAS kompute)

if(KOMPUTE_OPT_INSTALL)
    install(TARGETS kompute EXPORT KomputeTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        INCLUDES DESTINATION include)

    install(DIRECTORY include/ DESTINATION include)

    install(DIRECTORY ${PROJECT_SOURCE_DIR}/single_include/
        DESTINATION include)

    install(EXPORT KomputeTargets
        FILE komputeConfig.cmake
        NAMESPACE kompute::
        DESTINATION lib/cmake/kompute)
endif()

if(KOMPUTE_OPT_BUILD_PYTHON)
    include_directories(${PROJECT_SOURCE_DIR}/python/pybind11/include)
    find_package(PythonLibs REQUIRED)
    include_directories(${PYTHON_INCLUDE_DIRS})
endif()
