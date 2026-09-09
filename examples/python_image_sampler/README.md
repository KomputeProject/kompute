# Python Image Sampler Example

This demonstrates using `Image.create_sampler()` from Python to bind an image
as a combined image sampler (`sampler2D` in GLSL) instead of a plain storage
image (`image2D`).

The script uploads a tiny 4x4 checkerboard "image", attaches a sampler to
it, and dispatches a compute shader that fills a larger 16x16 output image
by sampling the small input with `texture()`. Because the input image has a
sampler attached, the GPU bilinearly filters between texels, producing a
smoothly interpolated (rather than blocky/nearest) upsample -- something
that isn't possible with `imageLoad`/`imageStore` on a plain storage image.

To run the example:

```bash
python image_sampler.py
```

## Pre-requisites

* REQUIRED
    + The `kp` Python package (built with `KOMPUTE_OPT_BUILD_PYTHON=ON`, or
      `pip install .` from the repo root)
    + `numpy`
    + `glslangValidator` available on your `PATH` (ships with the
      [Vulkan SDK](https://vulkan.lunarg.com/sdk/home)), used here to
      compile the inline GLSL shader at runtime
