# Kompute Logistic Regression Example

This folder contains an end to end Kompute Example that implements logistic regression.

This example is structured such that you will be able to extend it for your project.

It contains a cmake build configuration that can be used in your production applications.

## Building the example

You will notice that it's a standalone project, so you can re-use it for your application.

This project has the option to either import the Kompute dependency relative to the project or use your existing installation of Kompute.

To build you just need to run the cmake command in this folder as follows:

```
cmake -Bbuild/ \
          -DCMAKE_BUILD_TYPE=Debug                   \
          -DKOMPUTE_OPT_INSTALL=0                    \
          -DKOMPUTE_OPT_REPO_SUBMODULE_BUILD=1       \
          -DKOMPUTE_OPT_ENABLE_SPDLOG=1
```

You can pass the following optional parameters based on your desired configuration:
* If you wish to install with spdlog support you just have to pass `-DKOMPUTE_OPT_ENABLE_SPDLOG=1`.
* If you are using a package manager such as `vcpkg` make sure you pass the `-DCMAKE_TOOLCHAIN_FILE=` parameter 
* If you wish to load shader from raw glsl string instead of spirv bytes you can use `-DKOMPUTE_ANDROID_SHADER_FROM_STRING`

## Pre-requisites

In order to run this example, you will need the following dependencies:

* REQUIRED
    + The Vulkan SDK must be installed
* OPTIONAL
    + Vulkan Kompute library must be accessible (by default it uses the source directory)
    + SPDLOG - for logging
    + FMT - for text formatting

