

Processing Shaders with Kompute
=====================

Kompute allows for two main ways of interacting with shaders - namely:

* Integration with [glslang](https://github.com/KhronosGroup/glslang) for online/runtime shader compilation
* A CLI that coverts shaders into C++ header files

Processing Shaders Online via Kompute Shader Utils
---------------

Kompute provides a set of helper functions that expose the C++ functionality of the glslang Khronos framework to process shader sources online during runtime.

It's worth emphasising that the suggested approach is to process shaders offline, so the section below is suggested to convert shaders to either their respective SPV format, or convert them into C++ sources that would be embedded as part of the resulting binary.

The Shader utility function can be skipped on build time through compiler flags - for more information on this you should read the `build section <build-system.rst>`_.

More details on the shader utils can be found in the :class:`kp::Shader` section of the `C++ reference page <reference.rst>`_.

Converting Shaders into C / C++ Header Files
----------------------------------

Kompute allows for shaders to be loaded directly through the :class:`kp::OpAlgoBase` as either raw strings (through shaderc) or compiled SPIRV bytes. For this latter, the traditional method of including the SPIRV bytes is by loading the SPIRV file directly and passing the contents.

The Kompute codebase has a utility that allows you to convert shader files into C++ header files containing the SPIRV header data. This is useful as it enables developers to compile the SPIRV shaders into the final binary, which avoids the need for multiple files being required.

The utility can be found under `scripts/convert_shaders.py <https://github.com/EthicalML/vulkan-kompute/blob/master/scripts/convert_shaders.py>`_ and consists primarily of a Python CLI that can be called to pass arguments.

In order to use this Python utility, you will have to first install the dependencies outlined by the `scripts/requirements.txt` file. You will need to have python 3 and pip3 installed.

.. code-block:: bash
    :linenos:

    python3 -m pip install -r scripts/requirements.txt

Once the dependencies can be installed, you can run the Python script directly through the file as `python3 scripts/convert_shaders.py`.

You can run `python3 scripts/convert_shaders.py --help` to see all the options available - namely:

.. code-block:: bash
    :linenos:

    > python3 scripts/convert_shaders.py --help

    Usage: convert_shaders.py [OPTIONS]

      CLI function for shader generation

    Options:
      -p, --shader-path TEXT    The path for the directory to build and convert
                                shaders  [required]

      -s, --shader-binary TEXT  The path for the directory to build and convert
                                shaders  [required]

      -c, --header-path TEXT    The (optional) output file for the cpp header
                                files

      -v, --verbose             Enable versbosity if flag is provided
      --help                    Show this message and exit.

You can see the command that converts the shaders `in the makefile <https://github.com/EthicalML/vulkan-kompute/blob/45ddfe524b9ed63c5fe1fc33773c8f93a18e2fac/Makefile#L143>`_ to get an idea of how you would be able to use this utility.


