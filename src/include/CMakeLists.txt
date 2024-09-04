cmake_minimum_required(VERSION 3.20)

# ####################################################
# Kompute
# ####################################################
target_include_directories(kompute PUBLIC $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

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
    kompute/operations/OpCopy.hpp
    kompute/operations/OpSyncDevice.hpp
    kompute/operations/OpSyncLocal.hpp

    kompute/logger/Logger.hpp
)

if(KOMPUTE_OPT_INSTALL)
    install(DIRECTORY kompute DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
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