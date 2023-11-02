# This file is for generating .exe installer (NSIS) for Windows

# Here are all variables for NSIS generators.
# Commented variables are not used, or their default values are ok.
# Also see https://cmake.org/cmake/help/v3.20/cpack_gen/nsis.html

# The compressing method. Supported values: ZLIB, BZIP2 and LZMA
set(CPACK_NSIS_COMPRESSOR "LZMA")

# The name appears in Windows Apps & features in Control Panel
set(CPACK_NSIS_DISPLAY_NAME "Kompute")

# Ask the user whether to uninstall the previous versions
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)

# The icon of exe installer
set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/docs/images/kompute.ico")

# The icon of exe uninstaller
set(CPACK_NSIS_MUI_UNIICON ${CPACK_NSIS_MUI_ICON})

set(CPACK_NSIS_URL_INFO_ABOUT "https://kompute.cc")
set(CPACK_NSIS_HELP_LINK "https://kompute.cc/overview/community.html")

#set(CPACK_NSNS_CONTACT)
#set(CPACK_NSIS_MUI_WELCOMEFINISHPAGE_BITMAP)
#set(CPACK_NSIS_MUI_UNWELCOMEFINISHPAGE_BITMAP)