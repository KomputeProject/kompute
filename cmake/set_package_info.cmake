# This file is to setup information for cpack
# CPack is a package tool distributed with cmake.
# For details about CPack, see https://cmake.org/cmake/help/book/mastering-cmake/chapter/Packaging%20With%20CPack.html


# Common variables for all generators
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CPACK_PACKAGE_NAME "kompute-debug")
else()
    set(CPACK_PACKAGE_NAME "kompute")
endif ()
set(CPACK_PACKAGE_VENDOR "KomputeProject")
set(CPACK_PACKAGE_DESCRIPTION "General purpose GPU compute framework built on Vulkan to support 1000s of cross vendor graphics cards (AMD, Qualcomm, NVIDIA & friends). Blazing fast, mobile-enabled, asynchronous and optimized for advanced GPU data processing usecases. Backed by the Linux Foundation. ")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "GPGPU framework built on Vulkan.")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://kompute.cc/")

# Other common variables, in most cases they are not used,
# otherwise their default values are ok.
# Also see https://cmake.org/cmake/help/v3.20/module/CPack.html
#
# set(CPACK_PACKAGE_DIRECTORY)
# set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
# set(CPACK_PACKAGE_DIRECTORY)
# set(CPACK_PACKAGE_VERSION_MAJOR)
# set(CPACK_PACKAGE_VERSION_MINOR)
# set(CPACK_PACKAGE_VERSION_PATCH)
# set(CPACK_PACKAGE_DESCRIPTION_FILE)
# set(CPACK_PACKAGE_FILE_NAME)
# set(CPACK_PACKAGE_INSTALL_DIRECTORY)
# set(CPACK_PACKAGE_ICON)
# set(CPACK_PACKAGE_CHECKSUM)
# set(CPACK_PROJECT_CONFIG_FILE)
# set(CPACK_RESOURCE_FILE_LICENSE)
# set(CPACK_RESOURCE_FILE_README)
# set(CPACK_RESOURCE_FILE_WELCOME)
# set(CPACK_MONOLITHIC_INSTALL)
# set(CPACK_GENERATOR)
# set(CPACK_OUTPUT_CONFIG_FILE)
# set(CPACK_PACKAGE_EXECUTABLES)
# set(CPACK_STRIP_FILES)
# set(CPACK_VERBATIM_VARIABLES)
# set(CPACK_THREADS)



# Setup detailed package info for any cpack generator.
# Each file correspond to a cpack generator
file(GLOB config_files "${CMAKE_SOURCE_DIR}/cmake/cpack-configs/*.cmake")
foreach (config_file ${config_files})
    include(${config_file})
endforeach ()

# This should be include at last, and all unset cpack variables will be setup to their default value.
include(CPack)