# Vulkan Kompute Godot Example

## Set Up Dependencies

### Vulkan

You will need the Vulkan SDK, in this case we use version `1.2.148.1`, which you can get at the official site https://vulkan.lunarg.com/sdk/home#windows

This will have the following contents that will be required later on:

* The VulkanSDK static library `vulkan-1`

### Kompute

We will be using v0.3.1 of Kompute, and similar to above we will need the built static library, but in this case we will build it.

We can start by cloning the repository on the v0.3.1 branch:

```
git clone --branch v0.3.1 https://github.com/EthicalML/vulkan-kompute/
```

You will be able to use cmake to generate the build files for your platform.

```
cmake vulkan-kompute/. -Bvulkan-kompute/build
```

You need to make sure that the build is configured with the same flags required for godot, for example, in windows you will need:

* Release build
* Configuration type: static library
* Runtime lib: Multi-threaded / multi-threaded debug

Now you should see the library built under `build/src/Release`

## Building Godot

Now to build godot you will need to set up a couple of things for the Scons file to work - namely setting up the following:

* Copy the `vulkan-1` library from your vulkan sdk folder to `lib/vulkan-1.lib`
* Copy the `kompute.lib` library from the Kompute build to `lib/kompute.lib`
* Make sure the versions above match as we provide the headers in the `include` folder - if you used different versions make sure these match as well


