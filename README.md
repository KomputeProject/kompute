
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

* [Single header](single_include/kompute/Kompute.hpp) library for simple import to your project
* [Documentation](https://kompute.cc) leveraging doxygen and sphinx 
* BYOV: Bring-your-own-Vulkan design to play nice with existing Vulkan applications
* Non-Vulkan core naming conventions to disambiguate Vulkan vs Kompute components
* Asynchronous processing capabilities with granular mult-queue workload processing
* Fast development cycles with shader tooling, but robust static shader binary bundles for prod
* Explicit relationships for GPU and host memory ownership and memory management
* End-to-end examples for [machine learning 🤖](https://towardsdatascience.com/machine-learning-and-data-processing-in-the-gpu-with-vulkan-kompute-c9350e5e5d3a), [mobile development 📱](https://towardsdatascience.com/gpu-accelerated-machine-learning-in-your-mobile-applications-using-the-android-ndk-vulkan-kompute-1e9da37b7617), [game development 🎮](https://towardsdatascience.com/supercharging-game-development-with-gpu-accelerated-ml-using-vulkan-kompute-the-godot-game-engine-4e75a84ea9f0).

![](https://raw.githubusercontent.com/ethicalml/vulkan-kompute/master/docs/images/komputer-2.gif)

## Getting Started

### Setup

Kompute is provided as a single header file [`Kompute.hpp`](#setup) that can be simply included in your code and integrated with the shared library.

This project is built using cmake, providing a simple way to integrate as static or shared library across multiple platforms including mobile, game engines and various other platforms - see [build-system section](#build-overview) for more details.


#### Your First Kompute

Pass compute shader data in glsl/hlsl text or compiled SPIR-V format (or as path to the file). View [more examples](#simple-examples).

```c++
int main() {

    // You can allow Kompute to create the Vulkan components, or pass your existing ones
    kp::Manager mgr; // Selects device 0 unless explicitly requested

    // Creates tensor an initializes GPU memory (below we show more granularity)
    auto tensorA = std::make_shared<kp::Tensor>(kp::Tensor({ 3., 4., 5. }));
    auto tensorB = std::make_shared<kp::Tensor>(kp::Tensor({ 0., 0., 0. }));

    // Create tensors data explicitly in GPU with an operation
    mgr.evalOpDefault<kp::OpTensorCreate>({ tensorA, tensorB });

    // Define your shader as a string (using string literals for simplicity)
    // (You can also pass the raw compiled bytes, or even path to file)
    std::string shader(R"(
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

    // Run Kompute operation on the parameters provided with dispatch layout
    mgr.evalOpDefault<kp::OpAlgoBase<3, 1, 1>>(
        { tensorA, tensorB }, 
        std::vector<char>(shader.begin(), shader.end()));

    // Sync the GPU memory back to the local tensor
    mgr.evalOpDefault<kp::OpTensorSyncLocal>({ tensorA, tensorB });

    // Prints the output which is A: { 0, 1, 2 } B: { 3, 4, 5 }
    std::cout << fmt::format("A: {}, B: {}", 
        tensorA.data(), tensorB.data()) << std::endl;
}
```

## Motivations

This project started after seeing that a lot of new and renowned ML & DL projects like Pytorch, Tensorflow, Alibaba DNN, between others, have either integrated or are looking to integrate the Vulkan GPU SDK to add mobile GPU (and cross-vendor GPU) support.

The Vulkan SDK offers a great low level interface that enables for highly specialized optimizations - however it comes at a cost of highly verbose code which requires 500-2000 lines of code to even begin writing application code. This has resulted in each of these projects having to implement the same baseline to abstract the non-compute related features of Vulkan, although it's not always the case, this can result in slower development cycles, and opportunities for bugs to be introduced.

We are currently developing Vulkan Kompute not to hide the Vulkan SDK interface (as it's incredibly well designed) but to augment it with a direct focus on Vulkan's GPU computing capabilities. [This article](https://towardsdatascience.com/machine-learning-and-data-processing-in-the-gpu-with-vulkan-kompute-c9350e5e5d3a) provides a high level overview of the motivations of Kompute, together with a set of hands on examples that introduce both GPU computing as well as the core Vulkan Kompute architecture.

## More examples

### Simple examples

* [Pass shader as raw string](#your-first-kompute)
* [Record batch commands with a Kompute Sequence](#record-batch-commands)
* [Run Asynchronous Operations](#asynchronous-operations)
* [Run Parallel Operations Across Multiple GPU Queues](#parallel-operations)
* [Create your custom Kompute Operations](#your-custom-kompute-operation)

### End-to-end examples

* [Machine Learning Logistic Regression Implementation](https://towardsdatascience.com/machine-learning-and-data-processing-in-the-gpu-with-vulkan-kompute-c9350e5e5d3a)
* [Android NDK Mobile Kompute ML Application](https://towardsdatascience.com/gpu-accelerated-machine-learning-in-your-mobile-applications-using-the-android-ndk-vulkan-kompute-1e9da37b7617)
* [Game Development Kompute ML in Godot Engine](https://towardsdatascience.com/supercharging-game-development-with-gpu-accelerated-ml-using-vulkan-kompute-the-godot-game-engine-4e75a84ea9f0)

#### Record batch commands

Record commands in a single submit by using a Sequence to send in batch to GPU. Back to [more examples](#simple-examples)

```c++
int main() {

    kp::Manager mgr;

    std::shared_ptr<kp::Tensor> tensorLHS{ new kp::Tensor({ 1., 1., 1. }) };
    std::shared_ptr<kp::Tensor> tensorRHS{ new kp::Tensor({ 2., 2., 2. }) };
    std::shared_ptr<kp::Tensor> tensorOutput{ new kp::Tensor({ 0., 0., 0. }) };

    // Create all the tensors in memory
    mgr.evalOpDefault<kp::OpCreateTensor>({tensorLHS, tensorRHS, tensorOutput});

    // Create a new sequence
    std::weak_ptr<kp::Sequence> sqWeakPtr = mgr.getOrCreateManagedSequence();

    if (std::shared_ptr<kp::Sequence> sq = sqWeakPtr.lock())
    {
        // Begin recording commands
        sq->begin();

        // Record batch commands to send to GPU
        sq->record<kp::OpMult<>>({ tensorLHS, tensorRHS, tensorOutput });
        sq->record<kp::OpTensorCopy>({tensorOutput, tensorLHS, tensorRHS});

        // Stop recording
        sq->end();

        // Submit multiple batch operations to GPU
        size_t ITERATIONS = 5;
        for (size_t i = 0; i < ITERATIONS; i++) {
            sq->eval();
        }

        // Sync GPU memory back to local tensor
        sq->begin();
        sq->record<kp::OpTensorSyncLocal>({tensorOutput});
        sq->end();
        sq->eval();
    }

    // Print the output which iterates through OpMult 5 times
    // in this case the output is {32, 32 , 32}
    std::cout << fmt::format("Output: {}", tensorOutput.data()) << std::endl;
}
```

#### Asynchronous Operations

You can submit operations asynchronously with the async/await commands in the kp::Manager and kp::Sequence, which provides granularity on waiting on the vk::Fence. Back to [more examples](#simple-examples)

```c++
int main() {

    // You can allow Kompute to create the Vulkan components, or pass your existing ones
    kp::Manager mgr; // Selects device 0 unless explicitly requested

    // For synchronous steps we must already have a sequence created
    mgr.createManagedSequence("async");

    // Creates tensor an initializes GPU memory (below we show more granularity)
    auto tensor = std::make_shared<kp::Tensor>(kp::Tensor(std::vector<float>(10, 0.0)));

    // Create tensors data explicitly in GPU with an operation
    mgr.evalOpAsync<kp::OpTensorCreate>({ tensor }, "async");

    // Define your shader as a string (using string literals for simplicity)
    // (You can also pass the raw compiled bytes, or even path to file)
    std::string shader(R"(
        #version 450

        layout (local_size_x = 1) in;

        layout(set = 0, binding = 0) buffer b { float pb[]; };

        shared uint sharedTotal[1];

        void main() {
            uint index = gl_GlobalInvocationID.x;

            sharedTotal[0] = 0;

            // Iterating to simulate longer process
            for (int i = 0; i < 100000000; i++)
            {
                atomicAdd(sharedTotal[0], 1);
            }

            pb[index] = sharedTotal[0];
        }
    )");

    // We can now await for the previous submitted command
    // The second parameter can be the amount of time to wait
    // The time provided is in nanoseconds
    mgr.evalOpAwait("async", 10000);

    // Run Async Kompute operation on the parameters provided
    mgr.evalOpAsync<kp::OpAlgoBase<>>(
        { tensor }, 
        "async",
        std::vector<char>(shader.begin(), shader.end()));

    // Here we can do other work

    // When we're ready we can wait 
    // The default wait time is UINT64_MAX
    mgr.evalOpAwait("async")

    // Sync the GPU memory back to the local tensor
    // We can still run synchronous jobs in our created sequence
    mgr.evalOp<kp::OpTensorSyncLocal>({ tensor }, "async");

    // Prints the output: B: { 100000000, ... }
    std::cout << fmt::format("B: {}", 
        tensor.data()) << std::endl;
}
```

#### Parallel Operations

Besides being able to submit asynchronous operations, you can also leverage the underlying GPU compute queues to process operations in parallel.

This will depend on your underlying graphics card, but for example in NVIDIA graphics cards the operations submitted across queues in one family are not parallelizable, but operations submitted across queueFamilies can be parallelizable.

Below we show how you can parallelize operations in an [NVIDIA 1650](http://vulkan.gpuinfo.org/displayreport.php?id=9700#queuefamilies), which has a `GRAPHICS+COMPUTE` family on `index 0`, and `COMPUTE` family on `index 2`.

Back to [more examples](#simple-examples)

```c++
int main() {

    // In this case we select device 0, and for queues, one queue from familyIndex 0
    // and one queue from familyIndex 2
    uint32_t deviceIndex(0);
    std::vector<uint32_t> familyIndeces = {0, 2};

    // We create a manager with device index, and queues by queue family index
    kp::Manager mgr(deviceIndex, familyIndeces);

    // We need to create explicit sequences with their respective queues
    // The second parameter is the index in the familyIndex array which is relative
    //      to the vector we created the manager with.
    mgr.createManagedSequence("queueOne", 0);
    mgr.createManagedSequence("queueTwo", 1);

    // Creates tensor an initializes GPU memory (below we show more granularity)
    auto tensorA = std::make_shared<kp::Tensor>(kp::Tensor(std::vector<float>(10, 0.0)));
    auto tensorB = std::make_shared<kp::Tensor>(kp::Tensor(std::vector<float>(10, 0.0)));

    // We run the first step synchronously on the default sequence
    mgr.evalOpDefault<kp::OpTensorCreate>({ tensorA, tensorB });

    // Define your shader as a string (using string literals for simplicity)
    // (You can also pass the raw compiled bytes, or even path to file)
    std::string shader(R"(
        #version 450

        layout (local_size_x = 1) in;

        layout(set = 0, binding = 0) buffer b { float pb[]; };

        shared uint sharedTotal[1];

        void main() {
            uint index = gl_GlobalInvocationID.x;

            sharedTotal[0] = 0;

            // Iterating to simulate longer process
            for (int i = 0; i < 100000000; i++)
            {
                atomicAdd(sharedTotal[0], 1);
            }

            pb[index] = sharedTotal[0];
        }
    )");

    // Run the first parallel operation in the `queueOne` sequence
    mgr.evalOpAsync<kp::OpAlgoBase<>>(
        { tensorA }, 
        "queueOne",
        std::vector<char>(shader.begin(), shader.end()));

    // Run the second parallel operation in the `queueTwo` sequence
    mgr.evalOpAsync<kp::OpAlgoBase<>>(
        { tensorB }, 
        "queueTwo",
        std::vector<char>(shader.begin(), shader.end()));

    // Here we can do other work

    // We can now wait for thw two parallel tasks to finish
    mgr.evalOpAwait("queueOne")
    mgr.evalOpAwait("queueTwo")

    // Sync the GPU memory back to the local tensor
    mgr.evalOp<kp::OpTensorSyncLocal>({ tensorA, tensorB });

    // Prints the output: A: 100000000 B: 100000000
    std::cout << fmt::format("A: {}, B: {}", 
        tensorA.data()[0], tensorB.data()[0]) << std::endl;
}
```

### Your Custom Kompute Operation

Build your own pre-compiled operations for domain specific workflows. Back to [more examples](#simple-examples)

We also provide tools that allow you to [convert shaders into C++ headers](https://github.com/EthicalML/vulkan-kompute/blob/master/scripts/convert_shaders.py#L40).

```c++

template<uint32_t tX = 0, uint32_t tY = 0, uint32_t tZ = 0>
class OpMyCustom : public OpAlgoBase<tX, tY, tZ>
{
  public:
    OpMyCustom(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
           std::shared_ptr<vk::Device> device,
           std::shared_ptr<vk::CommandBuffer> commandBuffer,
           std::vector<std::shared_ptr<Tensor>> tensors)
      : OpAlgoBase<tX, tY, tZ>(physicalDevice, device, commandBuffer, tensors, "")
    {
        // Perform your custom steps such as reading from a shader file
        this->mShaderFilePath = "shaders/glsl/opmult.comp";
    }
}


int main() {

    kp::Manager mgr; // Automatically selects Device 0

    // Create 3 tensors of default type float
    auto tensorLhs = std::make_shared<kp::Tensor>(kp::Tensor({ 0., 1., 2. }));
    auto tensorRhs = std::make_shared<kp::Tensor>(kp::Tensor({ 2., 4., 6. }));
    auto tensorOut = std::make_shared<kp::Tensor>(kp::Tensor({ 0., 0., 0. }));

    // Create tensors data explicitly in GPU with an operation
    mgr.evalOpDefault<kp::OpTensorCreate>({ tensorLhs, tensorRhs, tensorOut });

    // Run Kompute operation on the parameters provided with dispatch layout
    mgr.evalOpDefault<kp::OpMyCustom<3, 1, 1>>(
        { tensorLhs, tensorRhs, tensorOut });

    // Prints the output which is { 0, 4, 12 }
    std::cout << fmt::format("Output: {}", tensorOutput.data()) << std::endl;
}
```

## Components & Architecture

The core architecture of Kompute include the following:
* Kompute Manager - Base orchestrator which creates and manages device and child components
* Kompute Sequence - Container of operations that can be sent to GPU as batch
* Kompute Operation - Individual operation which performs actions on top of tensors and (opt) algorithms
* Kompute Tensor - Tensor structured data used in GPU operations
* Kompute Algorithm - Abstraction for (shader) code executed in the GPU
* Kompute ParameterGroup - Container that can group tensors to be fed into an algorithm

To see a full breakdown you can read further in the documentation.

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

## Build Overview

The build system provided is `cmake` which allows for cross platform builds. Below is a brief overview of the build system.

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

