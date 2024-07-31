cmake_minimum_required(VERSION 3.20)

# ####################################################
# Kompute
# ####################################################

# Configure the version header file
set(KOMPUTE_VERSION_MAJOR ${${PROJECT_NAME}_VERSION_MAJOR})
set(KOMPUTE_VERSION_MINOR ${${PROJECT_NAME}_VERSION_MINOR})
set(KOMPUTE_VERSION_PATCH ${${PROJECT_NAME}_VERSION_PATCH})
set(KOMPUTE_VERSION_STRING "${KOMPUTE_VERSION_MAJOR}.${KOMPUTE_VERSION_MINOR}.${KOMPUTE_VERSION_PATCH}")
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/kompute/Version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/kompute/Version.h
    @ONLY
)

target_include_directories(kompute PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_sources(kompute PRIVATE

    # Header files (useful in IDEs)
    kompute/Algorithm.hpp
    kompute/Core.hpp
    kompute/Kompute.hpp
    kompute/Manager.hpp
    kompute/Sequence.hpp
    kompute/Tensor.hpp

    kompute/operations/OpAlgoDispatch.hpp
    kompute/operations/OpBase.hpp
    kompute/operations/OpMemoryBarrier.hpp
    kompute/operations/OpMult.hpp
    kompute/operations/OpTensorCopy.hpp
    kompute/operations/OpTensorSyncDevice.hpp
    kompute/operations/OpTensorSyncLocal.hpp

    kompute/logger/Logger.hpp
)

if(KOMPUTE_OPT_INSTALL)
    install(DIRECTORY kompute DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    # Install the generated version header
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kompute/Version.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/kompute)
endif()

# ####################################################
# Logger
# ####################################################
target_include_directories(kp_logger PUBLIC $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_sources(kp_logger PRIVATE

    # Header files (useful in IDEs)
    kompute/logger/Logger.hpp
)

# This installation requires non-existing dir logger,
# but previous installation at line 30 has already installed kompute/logger
# This will cause an error during installation
# install(DIRECTORY logger DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
