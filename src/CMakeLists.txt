

if(KOMPUTE_OPT_ENABLE_SPDLOG)
    find_package(spdlog REQUIRED)
    find_package(fmt REQUIRED)
endif()

if(KOMPUTE_OPT_ANDOID_BUILD)
    find_library(android android)
endif()

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
    set(ANDROID_APP_GLUE_DIR ${ANDROID_NDK}/sources/android/native_app_glue)
    set(VK_ANDROID_COMMON_DIR ${ANDROID_NDK}/sources/third_party/vulkan/src/common)
    set(VK_ANDROID_PATCH_DIR ${PROJECT_SOURCE_DIR}/vulkan_ndk_patch_include/)
    set(VK_ANDROID_INCLUDE_DIR ${ANDROID_NDK}/sources/third_party/vulkan/src/include)

    include_directories(
        ${ANDROID_APP_GLUE_DIR}
        ${VK_ANDROID_COMMON_DIR}
        ${VK_ANDROID_PATCH_DIR}
        ${VK_ANDROID_INCLUDE_DIR})

    add_library(android_app_glue STATIC
        ${ANDROID_APP_GLUE_DIR}/android_native_app_glue.c)

    #add_library(android_vulkan_wrapper STATIC
    #    ${VK_ANDROID_COMMON_DIR}/vulkan_wrapper.cpp)
endif()

if(KOMPUTE_OPT_ANDOID_BUILD)
    add_library(kompute
        ${kompute_CPP}
        ${PROJECT_SOURCE_DIR}/vulkan_ndk_patch_include/kompute_vulkan_wrapper.cpp)
else()
    add_library(kompute
        ${kompute_CPP})
endif()

target_include_directories(
    kompute PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

if(NOT KOMPUTE_OPT_ANDOID_BUILD)
    target_link_libraries(
        kompute 
        Vulkan::Vulkan
    )
else()
    target_link_libraries(
        kompute 
        "libvulkan.so"
    )
endif()

if(KOMPUTE_OPT_ENABLE_SPDLOG)
    target_link_libraries(
        kompute 
        fmt::fmt
        spdlog::spdlog
    )
endif()

if(KOMPUTE_OPT_ANDOID_BUILD)
    target_link_libraries(
        kompute 
        android_app_glue
        log
        android
    )
endif()

if(KOMPUTE_OPT_BUILD_SHADERS)
    add_dependencies(kompute
        build_shaders)
endif()

if(KOMPUTE_OPT_BUILD_SINGLE_HEADER)
    add_dependencies(kompute
        build_single_header)
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

