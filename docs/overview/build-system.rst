
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
   * - -DKOMPUTE_OPT_BUILD_TESTS=ON
     - Enable if you wish to build and run the tests (must have deps installed.
   * - -DKOMPUTE_OPT_CODE_COVERAGE=ON
     - Enable if you wish to build and run code coverage (must have deps installed which are limited to Windows platform)
   * - -DKOMPUTE_OPT_BUILD_DOCS=ON
     - Enable if you wish to build the docs (must have docs deps installed)
   * - -DKOMPUTE_OPT_BUILD_SHADERS=ON
     - Enable if you wish to build the shaders into header files (must have docs deps installed)
   * - -DKOMPUTE_OPT_BUILD_SINGLE_HEADER=ON
     - Option to build the single header file using "quom" utility
   * - -DKOMPUTE_OPT_INSTALL=OFF
     - Disables the install step in the cmake file (useful for android build)
   * - -DKOMPUTE_OPT_BUILD_PYTHON=ON
     - Enable to build python bindings (used internally for python package)
   * - -DKOMPUTE_OPT_ENABLE_SPDLOG=ON
     - Enable to compile with spdlog as the internal logging framework
   * - -DKOMPUTE_OPT_USE_BUILD_IN_SPDLOG=ON
     - Use the build in version of Spdlog
   * - -KOMPUTE_OPT_USE_BUILD_IN_FMT=ON
     - Use the build in version of fmt
   * - -KOMPUTE_OPT_USE_BUILD_IN_GOOGLE_TEST=ON
     - Use the build in version of GoogleTest
   * - -KOMPUTE_OPT_USE_BUILD_IN_PYBIND11=ON
     - Use the build in version of pybind11
   * - -KOMPUTE_OPT_USE_BUILD_IN_VULKAN_HEADER=ON
     - Use the build in version of Vulkan Headers. This could be helpful in case your system Vulkan Headers are to new for your driver. If you set this to false, please make sure your system Vulkan Header are supported by your driver.
   * - -KOMPUTE_OPT_BUILD_IN_VULKAN_HEADER_TAG="v1.2.203"
     - The git tag used for the build in Vulkan Headers when 'KOMPUTE_OPT_USE_BUILD_IN_VULKAN_HEADER' is enabled. A list of tags can be found here: https://github.com/KhronosGroup/Vulkan-Headers/tags
   * - -KOMPUTE_OPT_DISABLE_VULKAN_VERSION_CHECK=OFF
     - Whether to check if your driver supports the Vulkan Header version you are linking against. This might be useful in case you build shared on a different system than you run later.
   * - -DKOMPUTE_OPT_ANDROID_BUILD=ON
     - Enables android build which includes and excludes relevant libraries
   * - -DKOMPUTE_OPT_DISABLE_VK_DEBUG_LAYERS=ON
     - Explicitly disables debug layers even when on debug mode
   * - -DKOMPUTE_OPT_DEPENDENCIES_SHARED_LIBS=ON
     - Ensures dependencies are referenced as shared libraries for kompute install
   * - -DKOMPUTE_OPT_BUILD_AS_SHARED_LIB=ON
     - Whether to build Kompute as shared lib instead of static
   * - -DKOMPUTE_EXTRA_CXX_FLAGS="..."
     - Allows you to pass extra config flags to compiler

Compile Flags
~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1

   * - Flag
     - Description
   * - KOMPUTE_CREATE_PIPELINE_RESULT_VALUE
     - Ensure the return value of createPipeline is processed as ResultValue instead of Result
   * - -DKOMPUTE_VK_API_VERSION="..."
     - Sets the default api version to use for kompute api
   * - -DKOMPUTE_VK_API_MAJOR_VERSION=ON
     - Major version to use for the Vulkan SDK
   * - -DKOMPUTE_VK_API_MINOR_VERSION=ON
     - Minor version to use for the Vulkan SDK
   * - -DKOMPUTE_ENABLE_SPDLOG=ON
     - Enables the build with SPDLOG and FMT dependencies (must be installed)
   * - -DKOMPUTE_LOG_OVERRIDE=ON
     - Does not define the SPDLOG_\ :raw-html-m2r:`<LEVEL>` macros if these are to be overridden
   * - -DKOMPUTE_LOG_LEVEL
     - The level for the log level on compile level (also sets spdlog level if enabled)
   * - -DVVK_USE_PLATFORM_ANDROID_KHR
     - Flag to enable android imports in kompute (enabled with -DKOMPUTE_OPT_ANDROID_BUILD)
   * - -DRELEASE=ON
     - Enable release build (enabled by cmake release build)
   * - -DDEBUG=ON
     - Enable debug build including debug flags (enabled by cmake debug build)
   * - -DKOMPUTE_DISABLE_VK_DEBUG_LAYERS
     - Disable the debug Vulkan SDK Layers, mainly used for android builds

Other CMake Flags
~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1

   * - Flag
     - Description
   * - -DPYTHON_INCLUDE_DIR
     - Path to where Python.h is found, used for specifying installation of Python (see `this PR <https://github.com/KomputeProject/kompute/pull/222>`_)
   * - -DPYTHON_LIBRARY
     - Path to the Python library, used for specifying installation of Python (see `this PR <https://github.com/KomputeProject/kompute/pull/222>`_)
   * - -DCMAKE_OSX_ARCHITECTURES
     - Specifies the target architecture for Apple platforms (see `this issue <https://github.com/KomputeProject/kompute/issues/223>`_)

Dependencies
^^^^^^^^^^^^

Given Kompute is expected to be used across a broad range of architectures and hardware, it will be important to make sure we are able to minimise dependencies. 

Required dependencies
~~~~~~~~~~~~~~~~~~~~~

The only required dependency in the build is the Vulkan SDK. More specifically, the header files vulkan.h and vulkan.hpp, which are both part of the Vulkan SDK. If you haven't installed the Vulkan SDK yet, you can `download it here <https://vulkan.lunarg.com/>`_.

Optional dependencies
~~~~~~~~~~~~~~~~~~~~~

SPDLOG is the preferred logging library, however by default Kompute runs without SPDLOG by overriding the macros. It also provides an easy way to override the macros if you prefer to bring your own logging framework. The macro override is the following:

.. code-block:: c++

   #ifndef KOMPUTE_LOG_OVERRIDE // Use this if you want to define custom macro overrides
   #if KOMPUTE_SPDLOG_ENABLED // Use this if you want to enable SPDLOG
   #include <spdlog/spdlog.h>
   #endif //KOMPUTE_SPDLOG_ENABLED
   // ... Otherwise it adds macros that use std::cout (and only print first element)
   #endif // KOMPUTE_LOG_OVERRIDE

You can choose to build with or without SPDLOG by using the cmake flag ``KOMPUTE_OPT_ENABLE_SPDLOG``.

Finally, remember that you will still need to set both the compile time log level with ``SPDLOG_ACTIVE_LEVEL``\ , and the runtime log level with ``spdlog::set_level(spdlog::level::debug);``.
