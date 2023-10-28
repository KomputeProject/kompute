# This file is for generating archive packages.
# Up to v3.20, CPack supports .7z, .tar.bz2, .tar.gz, .tar.xz, .tar.z, .tar.zst and .zip
# For details, see https://cmake.org/cmake/help/v3.20/cpack_gen/archive.html

# The number of available cores on the machine will be used to compress
# Only available for tar.xz.
set(CPACK_ARCHIVE_THREADS 0)

# Other variables:
# set(CPACK_ARCHIVE_FILE_NAME)
# set(CPACK_ARCHIVE_<component>_FILE_NAME)
# set(CPACK_ARCHIVE_COMPONENT_INSTALL)