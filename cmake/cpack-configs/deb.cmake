# This file is for generating .deb packages for debian Linux

# Here are all variables for deb generators.
# Commented variables are not used, or their default values are ok.
# Also see https://cmake.org/cmake/help/v3.20/cpack_gen/deb.html

# Kompute depends on the vulkan development package
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libvulkan-dev")
# set(CPACK_DEBIAN_<COMPONENT>_PACKAGE_DEPENDS)

# The maintainer of this deb package
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "axsaucedo")


# These variables may be used.
# set(CPACK_DEBIAN_ARCHIVE_TYPE) # The archive format used for creating the Debian package.
# set(CPACK_DEBIAN_COMPRESSION_TYPE) # Possible values: lzma, xz, bzip2, gzip. gzip by default.

# Set Priority control field e.g. required, important, standard, optional, extra. Default value is optional.
# set(CPACK_DEBIAN_PACKAGE_PRIORITY)
# set(CPACK_DEBIAN_<COMPONENT>_PACKAGE_PRIORITY)

# Predepends is like depends, but it forces dpkg to complete installation of predepend packages even before starting
# the installation of our package.
# set(CPACK_DEBIAN_PACKAGE_PREDEPENDS)
# set(CPACK_DEBIAN_<COMPONENT>_PACKAGE_PREDEPENDS)

# Recommend other packages after user installs our package
# set(CPACK_DEBIAN_PACKAGE_RECOMMENDS)
# set(CPACK_DEBIAN_<COMPONENT>_PACKAGE_RECOMMENDS)

# Suggest other packages before user installs our package
# set(CPACK_DEBIAN_PACKAGE_SUGGESTS)
# set(CPACK_DEBIAN_<COMPONENT>_PACKAGE_SUGGESTS)

# Packages that will be broken by our package
# set(CPACK_DEBIAN_PACKAGE_BREAKS)
# set(CPACK_DEBIAN_<COMPONENT>_PACKAGE_BREAKS)

# Packages that conflicts with our package
# set(CPACK_DEBIAN_PACKAGE_CONFLICTS)
# set(CPACK_DEBIAN_<COMPONENT>_PACKAGE_CONFLICTS)

# Used for virtual packages
# set(CPACK_DEBIAN_PACKAGE_PROVIDES)
# set(CPACK_DEBIAN_<COMPONENT>_PACKAGE_PROVIDES)

# Packages that should be overwritten by our package
# set(CPACK_DEBIAN_PACKAGE_REPLACES)
# set(CPACK_DEBIAN_<COMPONENT>_PACKAGE_REPLACES)