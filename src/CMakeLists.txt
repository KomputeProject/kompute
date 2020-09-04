

if(KOMPUTE_OPT_ENABLE_SPDLOG)
    find_package(spdlog REQUIRED)
    find_package(fmt REQUIRED)
endif()

find_package(Vulkan REQUIRED)

# In production builds all shaders are compiled into cpp files
kompute_make(build_shaders 
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/include)
# In production builds all headers are compiled into a single header
kompute_make(build_single_header
    OUTPUT ${PROJECT_SOURCE_DIR}/single_include)

file(GLOB kompute_CPP
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_library(kompute
    ${kompute_CPP})

target_include_directories(
    kompute PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(
    kompute 
    Vulkan::Vulkan
)

if(KOMPUTE_OPT_ENABLE_SPDLOG)
    target_link_libraries(
        kompute 
        fmt::fmt
        spdlog::spdlog
    )
endif()

add_dependencies(kompute
    build_shaders
    build_single_header)


add_library(kompute::kompute ALIAS kompute)


install(TARGETS kompute EXPORT KomputeTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include)

install(DIRECTORY include/ DESTINATION include)

install(EXPORT KomputeTargets
    FILE KomputeTargets.cmake
    NAMESPACE kp::
    DESTINATION lib/cmake/kompute)

