

Processing Shaders with Kompute
=====================

Demo / testing function to compile shaders
----------------------------------

GLSLANG was initially integrated as part of the framework but it now has been removed due to the license of the glslang pre-processor being under a custom NVIDIA license which explicitly excludes grant of any licenses to NVIDIA's patents in the preprocessor.

For users that are looking to quickly test the processors it is possible to use the function that is provided in the examples which provides a (non-thread-safe / non-robust) implementation that compiles a shader string into spirv bytes. It is not recommended to use in production but it does enable for faster iteration cycles during development.

.. code-block:: cpp
    :linenos:
    static std::vector<uint32_t>
    compileSource(
      const std::string& source)
    {
        if (system(std::string("glslangValidator --stdin -S comp -V -o tmp_kp_shader.comp.spv << END
    " + source + "
    END").c_str()))
            throw std::runtime_error("Error running glslangValidator command");
        std::ifstream fileStream("tmp_kp_shader.comp.spv", std::ios::binary);
        std::vector<char> buffer;
        buffer.insert(buffer.begin(), std::istreambuf_iterator<char>(fileStream), {});
        return {(uint32_t*)buffer.data(), (uint32_t*)(buffer.data() + buffer.size())};
    }

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


