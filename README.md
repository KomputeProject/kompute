![GitHub](https://img.shields.io/badge/Release-ALPHA-yellow.svg)
![GitHub](https://img.shields.io/badge/Version-0.1.0-green.svg)
![GitHub](https://img.shields.io/badge/C++-11—20-purple.svg)
![GitHub](https://img.shields.io/badge/Build-cmake-red.svg)
![GitHub](https://img.shields.io/badge/Python-3.5—3.8-blue.svg)
![GitHub](https://img.shields.io/badge/License-Apache-black.svg)

<table>
<tr>

<td width="20%">
<img src="https://raw.githubusercontent.com/axsaucedo/vulkan-kompute/master/docs/images/kompute.jpg">
</td>

<td>

<h1>Vulkan Kompute</h1>
<h3>The General Purpose Vulkan Compute Framework. </h3>

</td>

</tr>
</table>

<h4>Blazing fast, lightweight, easy to set up and optimized for advanced data processing usecases.</h4>

🔋 [Documentation](https://axsaucedo.github.io/vulkan-kompute/) 💻 [Import to your project](https://axsaucedo.github.io/vulkan-kompute/) ⌨ [Tutorials](https://axsaucedo.github.io/vulkan-kompute/) 💾


## Principles & Features

* [Single header](single_include/kompute/Kompute.hpp) library for simple import to your project
* [Documentation](https://axsaucedo.github.io/vulkan-kompute/) leveraging doxygen and sphinx 
* Non-Vulkan naming convention to disambiguate Vulkan vs Kompute components
* Extends the existing Vulkan API with a compute-specific interface
* BYOV: Play nice with existing Vulkan applications with a bring-your-own-Vulkan design
* Directed acyclic memory management and explicits relationships of ownership

## Getting Started

### Setup

Kompute is provided as a single header file [`Kompute.hpp`](single_include/kompute/Kompute.hpp) that can be simply included in your code and integrated with the shared library.

This project is built using cmake providing a simple way to integrate as static or shared library.


### Your first Kompute

Run your tensors against default operations via the Manager.

```c++
int main() {

    kp::Manager mgr; // Automatically selects Device 0

    // Create 3 tensors of default type float
    auto tensorLhs = std::make_shared<kp::Tensor>(kp::Tensor({ 0., 1., 2. }));
    auto tensorRhs = std::make_shared<kp::Tensor>(kp::Tensor({ 2., 4., 6. }));
    auto tensorOut = std::make_shared<kp::Tensor>(kp::Tensor({ 0., 0., 0. }));

    // Create tensor data in GPU
    mgr.evalOpDefault<kp::OpCreateTensor>({ tensorLhs, tensorRhs, tensorOut });

    // Run Kompute operation on the parameters provided with dispatch layout
    mgr.evalOpDefault<kp::OpMult<3, 1, 1>>(
        { tensorLhs, tensorRhs, tensorOut }, 
        true, // Whether to retrieve the output from GPU memory
        std::vector<char>(shader.begin(), shader.end()));

    // Prints the output which is { 0, 4, 12 }
    std::cout << fmt::format("Output: {}", tensorOutput.data()) << std::endl;
}
```

Pass compute shader data (in raw or compiled SPIR-V) format for faster dev cycles.

```c++
int main() {

    kp::Manager mgr(1); // Explicitly selecting device 1

    auto tensorA = std::make_shared<kp::Tensor>(kp::Tensor({ 0, 1, 2 }));
    auto tensorRhs = std::make_shared<kp::Tensor>(kp::Tensor({ 2, 4, 6 }));

    // Define your shader as a string, or directly pass the compiled bytes
    std::string shader(
        "#version 450\n"
        "layout (local_size_x = 1) in;\n"
        "layout(set = 0, binding = 0) buffer bufa { uint a[]; };\n"
        "layout(set = 0, binding = 1) buffer bufb { uint b[]; };\n"
        "void main() {\n"
        "    uint index = gl_GlobalInvocationID.x;\n"
        "    b[index] = a[index];\n"
        "    a[index] = index;\n"
        "}\n"
    );

    // Create tensor data in GPU
    mgr.evalOpDefault<kp::OpCreateTensor>({ tensorA, tensorB });

    // Run Kompute operation on the parameters provided with dispatch layout
    mgr.evalOpDefault<kp::OpMult<3, 1, 1>>(
        { tensorLhs, tensorRhs, tensorOut }, 
        true, // Whether to retrieve the output from GPU memory
        std::vector<char>(shader.begin(), shader.end()));

    // Prints the output which is A: { 0, 1, 2 } B: { 3, 4, 5 }
    std::cout << fmt::format("A: {}, B: {}", 
        tensorA.data(), tensorB.data()) << std::endl;
}
```

Pass file path for shader data (in raw or compiled SPIR-V) for faster dev cycles.

```c++
int main() {

    kp::Manager mgr; // Automatically selects Device 0

    auto tensorA = std::make_shared<kp::Tensor>(kp::Tensor({ 0, 1, 2 }));
    auto tensorRhs = std::make_shared<kp::Tensor>(kp::Tensor({ 2, 4, 6 }));

    // Create tensor data in GPU
    mgr.evalOpDefault<kp::OpCreateTensor>({ tensorA, tensorB });

    // Run Kompute operation on the parameters provided with dispatch layout
    mgr.evalOpDefault<kp::OpMult<3, 1, 1>>(
        { tensorLhs, tensorRhs, tensorOut }, 
        true, // Whether to retrieve the output from GPU memory
        "path/to/shader.comp");

    // Prints the output which is A: { 0, 1, 2 } B: { 3, 4, 5 }
    std::cout << fmt::format("A: {}, B: {}", 
        tensorA.data(), tensorB.data()) << std::endl;
}
```

Record commands in a single submit by using a Sequence to send in batch to GPU.

```c++
int main() {

    kp::Manager mgr;

    std::shared_ptr<kp::Tensor> tensorLHS{ new kp::Tensor({ 0.0, 1.0, 2.0 }) };
    std::shared_ptr<kp::Tensor> tensorRHS{ new kp::Tensor( { 2.0, 4.0, 6.0 }) };
    std::shared_ptr<kp::Tensor> tensorOutput{ new kp::Tensor({ 0.0, 0.0, 0.0 }) };

    kp::Sequence sq = mgr.constructSequence();
    // Begin recoding commands
    sq.begin();

    // Record sequence of operations to be sent to GPU in batch
    {
        sq.record<kp::OpCreateTensor>({ tensorLHS });
        sq.record<kp::OpCreateTensor>({ tensorRHS });
        sq.record<kp::OpCreateTensor>({ tensorOutput });

        sq.record<kp::OpMult<>>({ tensorLHS, tensorRHS, tensorOutput });
    }

    // Stop recording
    sq.end();

    // Submit operations to GPU
    sq.eval();

    std::cout << fmt::format("Output: {}", tensorOutput.data()) << std::endl;
}
```

## Advanced Examples

We cover more advanced examples and applications of Vulkan Kompute, such as machine learning algorithms built on top of Kompute.

You can find these in the advanced examples documentation section, such as the [logistic regression example](https://axsaucedo.github.io/vulkan-kompute/overview/advanced-examples.html).

## Motivations

Vulkan Kompute was created after identifying the challenge most GPU processing projects with Vulkan undergo - namely having to build extensive boilerplate for Vulkan and create abstractions and interfaces that expose the core compute capabilities. It is only after a few thousand lines of code that it's possible to start building the application-specific logic. 

We believe Vulkan has an excellent design in its way to interact with the GPU, so by no means we aim to abstract or hide any complexity, but instead we want to provide a baseline of tools and interfaces that allow Vulkan Compute developers to focus on the higher level computational complexities of the application.

It is because of this that we have adopted development principles for the project that ensure the Vulkan API is augmented specifically for computation, whilst speeding development iterations and opening the doors to further use-cases.

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


<img width="100%" src="https://raw.githubusercontent.com/axsaucedo/vulkan-kompute/master/docs/images/kompute-vulkan-architecture.jpg">

<br>
<br>
(very tiny, check the docs to for details)
<br>
<br>

<img width="100%" src="https://raw.githubusercontent.com/axsaucedo/vulkan-kompute/master/docs/images/suspicious.jfif">

</td>
<td>
<img width="100%" src="https://raw.githubusercontent.com/axsaucedo/vulkan-kompute/master/docs/images/kompute-architecture.jpg">
</td>
</tr>
</table>


## Kompute Development

We appreciate PRs and Issues. If you want to contribute try checking the "Good first issue" tag, but even using Vulkan Kompute and reporting issues is a great contribution!

### Contributing

#### Dev Dependencies

* Testing
    + Catch2
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


# The Komputer is waiting for instructions...

![](https://raw.githubusercontent.com/axsaucedo/vulkan-kompute/master/docs/images/komputer-2.gif)

