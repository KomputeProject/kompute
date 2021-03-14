
C++ Build System Deep Dive
======================

The recommended approach to build the project is as out-of-source build in the ``build`` folder. This project comes with a ``Makefile`` that provides a set of commands that help with developer workflows. You can see some of the commands if you want to add some of the more advanced commands.

For a base build you just have to run:

.. code-block::

   cmake -Bbuild

This by default configures without any of the extra build tasks (such as building shaders) and compiles without the optional dependencies. The table below provides more detail.

.. list-table::
   :header-rows: 1

   * - Flag
     - Description
   * - -DCMAKE_INSTALL_PREFIX="build/src/CMakefiles/Export/"
     - Enables local installation (which won't require admin privileges)
   * - -DCMAKE_TOOLCHAIN_FILE="..."
     - This is the path for your package manager if you use it such as vcpkg
   * - -DKOMPUTE_OPT_BUILD_TESTS=1
     - Enable if you wish to build and run the tests (must have deps installed.
   * - -DKOMPUTE_OPT_BUILD_DOCS=1
     - Enable if you wish to build the docs (must have docs deps installed)
   * - -DKOMPUTE_OPT_BUILD_SINGLE_HEADER=1
     - Option to build the single header file using "quom" utility
   * - -DKOMPUTE_EXTRA_CXX_FLAGS="..."
     - Allows you to pass extra config flags to compiler
   * - -DKOMPUTE_OPT_INSTALL=0
     - Disables the install step in the cmake file (useful for android build)
   * - -DKOMPUTE_OPT_ANDROID_BUILD=1
     - Enables android build which includes and excludes relevant libraries
   * - -DKOMPUTE_OPT_DEPENDENCIES_SHARED_LIBS=1
     - Ensures dependencies are referenced as shared libraries for kompute install
   * - -DKOMPUTE_OPT_BUILD_AS_SHARED_LIB=1
     - Whether to build Kompute as shared lib instead of static


Compile Flags
~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1

   * - Flag
     - Description
   * - KOMPUTE_CREATE_PIPELINE_RESULT_VALUE
     - Ensure the return value of createPipeline is processed as ResultValue instead of Result
   * - -DKOMPUTE_VK_API_VERSION="..."
     - Sets the default api version to use for vulkan kompute api
   * - -DKOMPUTE_VK_API_MAJOR_VERSION=1
     - Major version to use for the Vulkan API
   * - -DKOMPUTE_VK_API_MINOR_VERSION=1
     - Minor version to use for the Vulkan API
   * - -DKOMPUTE_ENABLE_SPDLOG=1
     - Enables the build with SPDLOG and FMT dependencies (must be installed)
   * - -DKOMPUTE_LOG_VERRIDE=1
     - Does not define the SPDLOG_\ :raw-html-m2r:`<LEVEL>` macros if these are to be overridden
   * - -DSPDLOG_ACTIVE_LEVEL
     - The level for the log level on compile level (whether spdlog is enabled)
   * - -DVVK_USE_PLATFORM_ANDROID_KHR
     - Flag to enable android imports in kompute (enabled with -DKOMPUTE_OPT_ANDROID_BUILD)
   * - -DRELEASE=1
     - Enable release build (enabled by cmake release build)
   * - -DDEBUG=1
     - Enable debug build including debug flags (enabled by cmake debug build)
   * - -DKOMPUTE_DISABLE_VK_DEBUG_LAYERS
     - Disable the debug Vulkan layers, mainly used for android builds
   * - -DKOMPUTE_DISABLE_SHADER_UTILS
     - Disable the shader utils and skip adding glslang as dependency


Dependencies
^^^^^^^^^^^^

Given Kompute is expected to be used across a broad range of architectures and hardware, it will be important to make sure we are able to minimise dependencies. 

Required dependencies
~~~~~~~~~~~~~~~~~~~~~

The only required dependency in the build is Vulkan. More specifically, the header files vulkan.h and vulkan.hpp, which are both part of the Vulkan SDK. If you haven't installed the Vulkan SDK yet, you can `download it here <https://vulkan.lunarg.com/>`_.

Optional dependencies
~~~~~~~~~~~~~~~~~~~~~

SPDLOG is the preferred logging library, however by default Vulkan Kompute runs without SPDLOG by overriding the macros. It also provides an easy way to override the macros if you prefer to bring your own logging framework. The macro override is the following:

.. code-block:: c++

   #ifndef KOMPUTE_LOG_OVERRIDE // Use this if you want to define custom macro overrides
   #if KOMPUTE_SPDLOG_ENABLED // Use this if you want to enable SPDLOG
   #include <spdlog/spdlog.h>
   #endif //KOMPUTE_SPDLOG_ENABLED
   // ... Otherwise it adds macros that use std::cout (and only print first element)
   #endif // KOMPUTE_LOG_OVERRIDE

You can choose to build with or without SPDLOG by using the cmake flag ``KOMPUTE_OPT_ENABLE_SPDLOG``.

Finally, remember that you will still need to set both the compile time log level with ``SPDLOG_ACTIVE_LEVEL``\ , and the runtime log level with ``spdlog::set_level(spdlog::level::debug);``.
