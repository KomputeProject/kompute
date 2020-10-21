
![GitHub](https://img.shields.io/badge/Release-ALPHA-yellow.svg)
![GitHub](https://img.shields.io/badge/Version-0.1.0-green.svg)
![GitHub](https://img.shields.io/badge/C++-11—20-purple.svg)
![GitHub](https://img.shields.io/badge/Build-cmake-red.svg)
![GitHub](https://img.shields.io/badge/Python-3.5—3.8-blue.svg)
![GitHub](https://img.shields.io/badge/License-Apache-black.svg)

<table>
<tr>

<td width="20%">
<img src="https://raw.githubusercontent.com/EthicalML/vulkan-kompute/master/docs/images/kompute.jpg">
</td>

<td>

<h1>Vulkan Kompute</h1>
<h3>The General Purpose Vulkan Compute Framework.</h3>

</td>

</tr>
</table>

<h4>Blazing fast, mobile-enabled, asynchronous, and optimized for advanced GPU processing usecases.</h4>

🔋 [Documentation](https://kompute.cc) 💻 [Blog Post](https://medium.com/@AxSaucedo/machine-learning-and-data-processing-in-the-gpu-with-vulkan-kompute-c9350e5e5d3a) ⌨ [Examples](#more-examples) 💾


## Principles & Features

* [Single header](#setup) library for simple import to your project
* [Documentation](https://kompute.cc) leveraging doxygen and sphinx 
* [Asynchronous & parallel processing](#asynchronous-and-parallel-operations) capabilities with multi-queue command submission
* [Non-Vulkan naming conventions](#architectural-overview) to disambiguate Vulkan vs Kompute components
* BYOV: [Bring-your-own-Vulkan design](#motivations) to play nice with existing Vulkan applications
* Explicit relationships for GPU and host [memory ownership and memory management](https://kompute.cc/overview/memory-management.html)
* [Short code examples](#simple-examples) showing the core features 
* Longer tutorials for [machine learning 🤖](https://towardsdatascience.com/machine-learning-and-data-processing-in-the-gpu-with-vulkan-kompute-c9350e5e5d3a), [mobile development 📱](https://towardsdatascience.com/gpu-accelerated-machine-learning-in-your-mobile-applications-using-the-android-ndk-vulkan-kompute-1e9da37b7617) and [game development 🎮](https://towardsdatascience.com/supercharging-game-development-with-gpu-accelerated-ml-using-vulkan-kompute-the-godot-game-engine-4e75a84ea9f0).

![](https://raw.githubusercontent.com/ethicalml/vulkan-kompute/master/docs/images/komputer-2.gif)

## Getting Started

### Setup

Kompute is provided as a single header file [`Kompute.hpp`](#setup). See [build-system section](#build-overview) for configurations available.


### Your First Kompute (SIMPLE)

This simple example will show the basics of Kompute through the high level API.

1. Create and initialise a set of data tensors for processing
2. Run compute shader synchronously
3. Map results back from GPU memory to print the results

View the [extended version](#your-first-kompute-extended) or [more examples](https://kompute.cc/overview/advanced-examples.html#simple-examples).

```c++
int main() {

    // Default manager selects device 0 and first available compute compatible GPU queue
    kp::Manager mgr; 

    // 1. Create and initialise a set of data tensors for processing
    auto tensorA = mgr.buildTensor({ 3., 4., 5. });
    auto tensorB = mgr.buildTensor({ 0., 0., 0. });

    // 2. Run compute shader synchronously
    mgr.evalOpDefault<kp::OpAlgoBase<>>(
        { tensorA, tensorB }, 
        shader); // "shader" explained below, and can be glsl/spirv string or path to file

    // 3. Map results back from GPU memory to print the results
    mgr.evalOpDefault<kp::OpTensorSyncLocal>({ tensorA, tensorB })

    // Prints the output which is A: { 0, 1, 2 } B: { 3, 4, 5 }
    std::cout << fmt::format("A: {}, B: {}", 
        tensorA.data(), tensorB.data()) << std::endl;
}
```

Your shader can be provided as raw glsl/hlsl string, SPIR-V bytes array (using our CLI), or string path to file containing either. Below are the examples of the valid ways of providing shader.

#### Passing raw GLSL/HLSL string

```c++
static std::string shaderString = (R"(
    #version 450

    layout (local_size_x = 1) in;

    layout(set = 0, binding = 0) buffer a { float pa[]; };
    layout(set = 0, binding = 1) buffer b { float pb[]; };

    void main() {
        uint index = gl_GlobalInvocationID.x;
        pb[index] = pa[index];
        pa[index] = index;
    }
)");
static std::vector<char> shader(shaderString.begin(), shaderString.end());
```

#### Passing SPIR-V Bytes array 

You can use the Kompute [shader-to-cpp-header CLI](https://kompute.cc/overview/shaders-to-headers.html) to convert your GLSL/HLSL or SPIRV shader into C++ header file (see documentation link for more info). This is useful if you want your binary to be compiled with all relevant artifacts.

```c++
static std::vector<char> shader = { 0x03, //... spirv bytes go here)
```

#### Path to file containing raw glsl/hlsl or SPIRV bytes

```c++
static std::string shader = "path/to/shader.glsl";
// Or SPIR-V
static std::string shader = "path/to/shader.glsl.spv";
```

## Architectural Overview

The core architecture of Kompute include the following:
* [Kompute Manager](https://kompute.cc/overview/reference.html#manager) - Base orchestrator which creates and manages device and child components
* [Kompute Sequence](https://kompute.cc/overview/reference.html#sequence) - Container of operations that can be sent to GPU as batch
* [Kompute Operation (Base)](https://kompute.cc/overview/reference.html#algorithm) - Base class from which all operations inherit
* [Kompute Tensor](https://kompute.cc/overview/reference.html#tensor) - Tensor structured data used in GPU operations
* [Kompute Algorithm](https://kompute.cc/overview/reference.html#algorithm) - Abstraction for (shader) code executed in the GPU

To see a full breakdown you can read further in the [C++ Class Reference](https://kompute.cc/overview/reference.html).

<table>
<th>
Full Vulkan Components
</th>
<th>
Simplified Kompute Components
</th>
<tr>
<td width=30%>


<img width="100%" src="https://raw.githubusercontent.com/ethicalml/vulkan-kompute/master/docs/images/kompute-vulkan-architecture.jpg">

<br>
<br>
(very tiny, check the <a href="https://ethicalml.github.io/vulkan-kompute/overview/reference.html">full reference diagram in docs for details</a>)
<br>
<br>

<img width="100%" src="https://raw.githubusercontent.com/ethicalml/vulkan-kompute/master/docs/images/suspicious.jfif">

</td>
<td>
<img width="100%" src="https://raw.githubusercontent.com/ethicalml/vulkan-kompute/master/docs/images/kompute-architecture.jpg">
</td>
</tr>
</table>


## Asynchronous and Parallel Operations

Kompute provides flexibility to run operations in an asynrchonous way through Vulkan Fences. Furthermore, Kompute enables for explicit allocation of queues, which allow for parallel execution of operations across queue families.

The image below provides an intuition how Kompute Sequences can be allocated to different queues to enable parallel execution based on hardware. You can see the [hands on example](https://kompute.cc/overview/advanced-examples.html#parallel-operations), as well as the [detailed documentation page](https://kompute.cc/overview/async-parallel.html) describing how it would work using an NVIDIA 1650 as example. 

![](https://raw.githubusercontent.com/ethicalml/vulkan-kompute/master/docs/images/queue-allocation.jpg)

## Motivations

This project started after seeing that a lot of new and renowned ML & DL projects like Pytorch, Tensorflow, Alibaba DNN, between others, have either integrated or are looking to integrate the Vulkan SDK to add mobile (and cross-vendor) GPU support.

The Vulkan SDK offers a great low level interface that enables for highly specialized optimizations - however it comes at a cost of highly verbose code which requires 500-2000 lines of code to even begin writing application code. This has resulted in each of these projects having to implement the same baseline to abstract the non-compute related features of Vulkan. This large amount of non-standardised boiler-plate can result in limited knowledge transfer, higher chance of unique framework implementation bugs being introduced, between others.

We are currently developing Vulkan Kompute not to hide the Vulkan SDK interface (as it's incredibly well designed) but to augment it with a direct focus on Vulkan's GPU computing capabilities. [This article](https://towardsdatascience.com/machine-learning-and-data-processing-in-the-gpu-with-vulkan-kompute-c9350e5e5d3a) provides a high level overview of the motivations of Kompute, together with a set of hands on examples that introduce both GPU computing as well as the core Vulkan Kompute architecture.

### Your First Kompute (EXTENDED)

We will now show the [same example as above](#your-first-kompute-simple) but leveraging more advanced Kompute features:

1. Create a set of data tensors in host memory for processing
2. Map the tensor host data into GPU memory with Kompute Operation
3. Run compute shader asynchronously with Async function
4. Create managed sequence to submit batch operations to the CPU
5. Map data back to host by running the sequence of batch operations

View [more examples](https://kompute.cc/overview/advanced-examples.html#simple-examples).

```c++
int main() {

    // Creating manager with Device 0, and a single queue of familyIndex 2
    kp::Manager mgr(0, { 2 });

    // 1. Create a set of data tensors in host memory for processing
    auto tensorA = std::make_shared<kp::Tensor>(kp::Tensor({ 3., 4., 5. }));
    auto tensorB = std::make_shared<kp::Tensor>(kp::Tensor({ 0., 0., 0. }));

    // 2. Map the tensor host data into GPU memory with Kompute Operation
    mgr.evalOpDefault<kp::OpTensorCreate>({ tensorA, tensorB });

    // 3. Run compute shader Asynchronously with explicit dispatch layout
    mgr.evalOpAsyncDefault<kp::OpAlgoBase<3, 1, 1>>(
        { tensorA, tensorB }, 
        shader); // Using the same shader as above

    // 3.1. Before submitting sequence batch we wait for the async operation
    mgr.evalOpAwaitDefault();

    // 4. Create managed sequence to submit batch operations to the CPU
    std::shared_ptr<kp::Sequence> sq = mgr.getOrCreateManagedSequence("seq").lock();

    // 4.1. Explicitly begin recording batch commands
    sq->begin();

    // 4.2. Record batch commands
    sq->record<kp::OpTensorSyncLocal({ tensorA });
    sq->record<kp::OpTensorSyncLocal({ tensorB });

    // 4.3. Explicitly stop recording batch commands
    sq->end();

    // 5. Map data back to host by running the sequence of batch operations
    sq->eval();

    // Prints the output which is A: { 0, 1, 2 } B: { 3, 4, 5 }
    std::cout << fmt::format("A: {}, B: {}", 
        tensorA.data(), tensorB.data()) << std::endl;
}
```

## More examples

### Simple examples

* [Pass shader as raw string](https://kompute.cc/overview/advanced-examples.html#simple-shader-example)
* [Record batch commands with a Kompute Sequence](https://kompute.cc/overview/advanced-examples.html#record-batch-commands)
* [Run Asynchronous Operations](https://kompute.cc/overview/advanced-examples.html#asynchronous-operations)
* [Run Parallel Operations Across Multiple GPU Queues](https://kompute.cc/overview/advanced-examples.html#parallel-operations)
* [Create your custom Kompute Operations](https://kompute.cc/overview/advanced-examples.html#your-custom-kompute-operation)
* [Implementing logistic regression from scratch](https://kompute.cc/overview/advanced-examples.html#logistic-regression-example)

### End-to-end examples

* [Machine Learning Logistic Regression Implementation](https://towardsdatascience.com/machine-learning-and-data-processing-in-the-gpu-with-vulkan-kompute-c9350e5e5d3a)
* [Parallelizing GPU-intensive Workloads via Multi-Queue Operations](https://towardsdatascience.com/parallelizing-heavy-gpu-workloads-via-multi-queue-operations-50a38b15a1dc)
* [Android NDK Mobile Kompute ML Application](https://towardsdatascience.com/gpu-accelerated-machine-learning-in-your-mobile-applications-using-the-android-ndk-vulkan-kompute-1e9da37b7617)
* [Game Development Kompute ML in Godot Engine](https://towardsdatascience.com/supercharging-game-development-with-gpu-accelerated-ml-using-vulkan-kompute-the-godot-game-engine-4e75a84ea9f0)

## Build Overview

The build system provided uses `cmake`, which allows for cross platform builds.

### Build parameters (cmake)

The recommended approach to build the project is as out-of-source build in the `build` folder. This project comes with a `Makefile` that provides a set of commands that help with developer workflows. You can see some of the commands if you want to add some of the more advanced commands.

For a base build you just have to run:
```
cmake -Bbuild
```

This by default configures without any of the extra build tasks (such as building shaders) and compiles without the optional dependencies. The table below provides more detail.

| Flag                                                  | Description                                                              |
|-------------------------------------------------------|--------------------------------------------------------------------------|
| -DCMAKE_INSTALL_PREFIX="build/src/CMakefiles/Export/" | Enables local installation (which won't require admin privileges)        |
| -DCMAKE_TOOLCHAIN_FILE="..."                          | This is the path for your package manager if you use it such as vcpkg    |
| -DKOMPUTE_OPT_BUILD_TESTS=1                           | Enable if you wish to build and run the tests (must have deps installed. |
| -DKOMPUTE_OPT_BUILD_DOCS=1                            | Enable if you wish to build the docs (must have docs deps installed)     |
| -DKOMPUTE_OPT_BUILD_SINGLE_HEADER=1                   | Option to build the single header file using "quom" utility              |
| -DKOMPUTE_EXTRA_CXX_FLAGS="..."                       | Allows you to pass extra config flags to compiler                        |
| -DKOMPUTE_OPT_INSTALL=0                               | Disables the install step in the cmake file (useful for android build)   |
| -DKOMPUTE_OPT_ANDROID_BUILD=1                         | Enables android build which includes and excludes relevant libraries     |

#### Compile Flags


| Flag                                 | Description                                                                             |
|--------------------------------------|-----------------------------------------------------------------------------------------|
| KOMPUTE_CREATE_PIPELINE_RESULT_VALUE | Ensure the return value of createPipeline is processed as ResultValue instead of Result |
| -DKOMPUTE_VK_API_VERSION="..."       | Sets the default api version to use for vulkan kompute api                              |
| -DKOMPUTE_VK_API_MAJOR_VERSION=1     | Major version to use for the Vulkan API                                                 |
| -DKOMPUTE_VK_API_MINOR_VERSION=1     | Minor version to use for the Vulkan API                                                 |
| -DKOMPUTE_ENABLE_SPDLOG=1            | Enables the build with SPDLOG and FMT dependencies (must be installed)                  |
| -DKOMPUTE_LOG_VERRIDE=1              | Does not define the SPDLOG_<LEVEL> macros if these are to be overriden                  |
| -DSPDLOG_ACTIVE_LEVEL                | The level for the log level on compile level (whether spdlog is enabled)                |
| -DVVK_USE_PLATFORM_ANDROID_KHR       | Flag to enable android imports in kompute (enabled with -DKOMPUTE_OPT_ANDROID_BUILD)    |
| -DRELEASE=1                          | Enable release build (enabled by cmake release build)                                   |
| -DDEBUG=1                            | Enable debug build including debug flags (enabled by cmake debug build)                 |
| -DKOMPUTE_DISABLE_VK_DEBUG_LAYERS    | Disable the debug Vulkan layers, mainly used for android builds                         |

### Dependencies

Given Kompute is expected to be used across a broad range of architectures and hardware, it will be important to make sure we are able to minimise dependencies. 

#### Required dependencies

The only required dependency in the build is Vulkan (vulkan.h and vulkan.hpp which are both part of the Vulkan SDK).

#### Optional dependencies

SPDLOG is the preferred logging library, however by default Vulkan Kompute runs without SPDLOG by overriding the macros. It also provides an easy way to override the macros if you prefer to bring your own logging framework. The macro override is the following:

```c++
#ifndef KOMPUTE_LOG_OVERRIDE // Use this if you want to define custom macro overrides
#if KOMPUTE_SPDLOG_ENABLED // Use this if you want to enable SPDLOG
#include <spdlog/spdlog.h>
#endif //KOMPUTE_SPDLOG_ENABLED
// ... Otherwise it adds macros that use std::cout (and only print first element)
#endif // KOMPUTE_LOG_OVERRIDE
```

You can choose to build with or without SPDLOG by using the cmake flag `KOMPUTE_OPT_ENABLE_SPDLOG`.

Finally, remember that you will still need to set both the compile time log level with `SPDLOG_ACTIVE_LEVEL`, and the runtime log level with `spdlog::set_level(spdlog::level::debug);`.


## Kompute Development

We appreciate PRs and Issues. If you want to contribute try checking the "Good first issue" tag, but even using Vulkan Kompute and reporting issues is a great contribution!

### Contributing

#### Dev Dependencies

* Testing
    + GTest
* Documentation
    + Doxygen (with Dot)
    + Sphynx

#### Development

* Follows Mozilla C++ Style Guide https://www-archive.mozilla.org/hacking/mozilla-style-guide.html
    + Uses post-commit hook to run the linter, you can set it up so it runs the linter before commit
    + All dependencies are defined in vcpkg.json 
* Uses cmake as build system, and provides a top level makefile with recommended command
* Uses xxd (or xxd.exe windows 64bit port) to convert shader spirv to header files
* Uses doxygen and sphinx for documentation and autodocs
* Uses vcpkg for finding the dependencies, it's the recommanded set up to retrieve the libraries

##### Updating documentation

To update the documentation will need to:
* Run the gendoxygen target in the build system
* Run the gensphynx target in the buildsystem 
* Push to github pages with `make push_docs_to_ghpages`

##### Running tests

To run tests you can use the helper top level Makefile

For visual studio you can run

```
make vs_cmake
make vs_run_tests VS_BUILD_TYPE="Release"
```

For unix you can run

```
make mk_cmake MK_BUILD_TYPE="Release"
make mk_run_tests
```

