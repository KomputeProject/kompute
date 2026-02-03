# Kompute Debugging GLSL Shader using DebugPrint

This folder contains an end to end Kompute Example that implements a GLSL with a debug print statement.
This example is structured such that you will be able to extend it for your project.
It contains a CMake build configuration that can be used in your production applications.

## Further information
Debugging Vulkan shaders, especially compute shaders, can be very difficult to do even with the aid
of a powerful debugging tool like RenderDoc. Debug Printf is a recent Vulkan feature that allows
developers to debug their shaders by inserting Debug Print statements. This debug print statement operates 
quite like the C printf statement, only that it is executed in multiple compute cores at the same time, so it needs some logic to allow semantic usage of the debug statement otherwise you can easily be overwelmed by debug messages. 
<br>

This feature is now supported within RenderDoc in a way that allows for per-invocation inspection of values in a shader.
This article describes how to instrument your GLSL or HLSL shaders with Debug Printf and how to
inspect and debug with them in the terminal, using vkconfig, or with environment variables.

For a full walkthrough of the process please see my article at https://medium.com/@evanokeeffe/using-debug-printf-in-vulkan-kompute-shaders-2aaf30bdb96c

## Building the example

You will notice that it's a standalone project, so you can re-use it for your application.
It uses CMake's [`fetch_content`](https://cmake.org/cmake/help/latest/module/FetchContent.html) to consume Kompute as a dependency.
To build you just need to run the CMake command in this folder as follows:

```bash
git clone https://github.com/KomputeProject/kompute.git
cd kompute/examples/vulkan_ext_printf
mkdir build
cd build
cmake ..
cmake --build .
```

## Executing

From inside the `build/` directory run:

### Linux

```bash
./example_shader_printf
```

## Pre-requisites

In order to run this example, you will need the following dependencies:

* REQUIRED
    + The Vulkan SDK must be installed

For the Vulkan SDK, the simplest way to install it is through [their website](https://vulkan.lunarg.com/sdk/home). You just have to follow the instructions for the relevant platform.
