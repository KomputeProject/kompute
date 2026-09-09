# Kompute Image Sampler Example

This folder contains an end to end Kompute example that shows how to use
`kp::Image::createSampler()` to bind an image as a combined image sampler
(`sampler2D` in GLSL) instead of a plain storage image (`image2D`).

The example uploads a tiny 4x4 checkerboard "image", attaches a sampler to
it, and dispatches a compute shader that fills a larger 16x16 output image
by sampling the small input with `texture()`. Because the input image has a
sampler attached, the GPU bilinearly filters between texels, producing a
smoothly interpolated (rather than blocky/nearest) upsample -- something
that isn't possible with `imageLoad`/`imageStore` on a plain storage image.

This example is structured such that you will be able to extend it for your
project. It contains a CMake build configuration that can be used in your
production applications.

## Building the example

You will notice that it's a standalone project, so you can re-use it for
your application. It uses CMake's
[`fetch_content`](https://cmake.org/cmake/help/latest/module/FetchContent.html)
to consume Kompute as a dependency. To build you just need to run the CMake
command in this folder as follows:

```bash
git clone https://github.com/KomputeProject/kompute.git
cd kompute/examples/image_sampler
mkdir build
cd build
cmake ..
cmake --build .
```

## Executing

Form inside the `build/` directory run:

### Linux

```bash
./kompute_image_sampler
```

### Windows

```bash
.\Debug\kompute_image_sampler.exe
```

## Pre-requisites

In order to run this example, you will need the following dependencies:

* REQUIRED
    + The Vulkan SDK must be installed

For the Vulkan SDK, the simplest way to install it is through
[their website](https://vulkan.lunarg.com/sdk/home). You just have to follow
the instructions for the relevant platform.
