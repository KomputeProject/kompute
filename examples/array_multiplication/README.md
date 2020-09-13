# Kompute Array Multiplication Example

This folder contains an end to end Kompute Example that implements logistic regression.

This example is structured such that you will be able to extend it for your project.

It contains a cmake build configuration that can be used in your production applications.

## Pre-requisites

In order to run this example, you will need the following dependencies:

* REQUIRED
    + Vulkan Kompute library must be accessible
    + The Vulkan SDK must be installed
* OPTIONAL
    + SPDLOG - for logging
    + FMT - for text formatting

We will cover how you can install Vulkan Kompute in the next section.

For the Vulkan SDK, the simplest way to install it is through [their website](https://vulkan.lunarg.com/sdk/home). You just have to follow the instructions for the relevant platform.

For the other libraries, because they are optional you can just make sure you build and install Kompute with these disabled (this will be covered in more detail below).

Alternatively you can use package managers such as vcpkg to help you install them, although to simplify things you can start without the dependencies first.

## Set Up Vulkan Kompute Dependency

You have multiple options to set up Vulkan Kompute. The easiest is to perform a local installation.

For this, you will want to go to the main repo and run the following cmake command, which will configure it without SPDLOG by default.

```
cmake \
    -Bbuild
```

You can pass the following optional parameters based on your desired configuration:
* If you wish to install with spdlog support you just have to pass `-DKOMPUTE_ENABLE_SPDLOG=1`.
* If you wish to perform the installation on the local folder instead of in your system you can use `-DCMAKE_INSTALL_PREFIX="build/src/CMakeFiles/Export/"` which will basically ensure that the final files are created in the local directory.
* If you are using a package manager such as `vcpkg` make sure you pass the `-DCMAKE_TOOLCHAIN_FILE=` parameter 

Then you can proceed to run the installation:

* For Windows / Visual Studio you just have to build `INSTALL.vcxproj`
* For Linux you can just run the `install` target via `make -C build install`

You also have the option to build as `Release` or `Debug` - just make sure that you build your example with the same build/debug flags as required.

## Building the example

Now that you've set up the dependencies / installation of Vulkan Kompute you can build this example.

You will notice that it's a standalone project, so you can re-use it for your application.

To build you just need to run the cmake command in this folder as follows:

```
cmake \
    -Bbuild
```

Make sure to pass the required flags depending on the configuration above:
* If you built with Debug make sure you build your example with Debug as well
* If you installed in the local folder, make sure you pass the CMAKE_PREFIX_PATH pointing to the respective folder (e.g. `-DCMAKE_PREFIX_PATH=../../build/src/CMakeFiles/Export/lib/cmake/kompute/` if parent folder is main repo).
* If you built Vulkan Kompute with spdlog enabled, make sure to pass `-DKOMPUTE_OPT_ENABLE_SPDLOG=1`
* If you are using a package manager such as `vcpkg` make sure you pass the `-DCMAKE_TOOLCHAIN_FILE=` parameter 

Now you just have to build your application as above:

* For Windows / Visual Studio you just have to build and run `kompute_array_mult.vcxproj`
* For Linux you can just run the `kompute_array_mult` target via `make -C build kompute_array_mult`

