# Kompute Structure Array Multiplication Example

This folder contains an Kompute Example demonstrating how to use structs between c++ and shaders. 

## Building the example

You will notice that it's a standalone project, so you can re-use it for your application.
It uses CMake's [`fetch_content`](https://cmake.org/cmake/help/latest/module/FetchContent.html) to consume Kompute as a dependency.
To build you just need to run the CMake command in this folder as follows:

```bash
git clone https://github.com/KomputeProject/kompute.git
cd kompute/examples/structure_array_multiplication
mkdir build
cd build
cmake ..
cmake --build .
```

## Executing

Form inside the `build/` directory run:

### Linux

```bash
./kompute_array_mult
```

### Windows

```bash
.\Debug\kompute_array_mult.exe
```

## Pre-requisites

In order to run this example, you will need the following dependencies:

* REQUIRED
    + The Vulkan SDK must be installed

For the Vulkan SDK, the simplest way to install it is through [their website](https://vulkan.lunarg.com/sdk/home). You just have to follow the instructions for the relevant platform.
