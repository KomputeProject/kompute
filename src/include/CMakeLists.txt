cmake_minimum_required(VERSION 3.15)

#####################################################
# Kompute
#####################################################

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
    kompute/operations/OpTensorCopy.hpp
    kompute/operations/OpTensorSyncDevice.hpp
    kompute/operations/OpTensorSyncLocal.hpp

    kompute/shaders/shaderlogisticregression.hpp
    kompute/shaders/shaderopmult.hpp

    kompute/logger/Logger.hpp
)

install(DIRECTORY kompute DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

#####################################################
# Logger
#####################################################

target_include_directories(logger PUBLIC $<INSTALL_INTERFACE:include>
                                         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_sources(logger PRIVATE
     # Header files (useful in IDEs)    
    kompute/logger/Logger.hpp
)

install(DIRECTORY logger DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})