
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(Vulkan REQUIRED)

kompute_make(install_python_reqs)
kompute_make(build_shaders)
kompute_make(build_single_header)

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
    fmt::fmt
    spdlog::spdlog
    Vulkan::Vulkan
)

add_dependencies(kompute
    build_shaders
    build_single_header)

add_dependencies(kompute
    install_python_reqs)

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

