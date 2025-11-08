# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Claude Development Instructions

**DOCUMENT PROGRESS**: Ensure that before implementing a larger change / plan, this is documented in the PROGRESS.md. This file should be append-only; each entry should contain a title with the date and time. Even if auto-accept changes are enabled ensure that user approves the appending of content to this file. ALWAYS APPEND TO THIS FILE, AND REQUEST USER PERMISSION TO APPEND EVEN WHEN AUTO ACCEPT ENABLED. THIS IS IMPORTANT.

**KEEP IT SIMPLE**: When implementing changes or new features, default to simplicity over complexity. Avoid adding extra layers of abstraction unless they are actually required for further feature support. The existing Kompute architecture is already well-designed - respect the established patterns and don't over-engineer solutions. Direct, straightforward implementations that follow existing conventions are preferred over clever abstractions that may complicate future maintenance.

## Project Overview

Kompute is a general-purpose GPU compute framework built on Vulkan that provides cross-vendor graphics card support (AMD, Qualcomm, NVIDIA). It offers blazing fast, mobile-enabled, asynchronous GPU computing capabilities optimized for advanced GPU acceleration use cases, particularly machine learning and high-performance computing workloads.

The framework follows the BYOV principle (Bring Your Own Vulkan) and is designed to integrate with existing Vulkan applications while providing a high-level interface for GPU compute operations.

## Essential Development Commands

**Local Development Workflow:**
```bash
# Configure and run all tests (recommended for development)
make mk_cmake mk_run_tests

# Just verify compilation works
make mk_cmake mk_build_kompute

# Clear CMake cache when needed
make clean_cmake
```

**Individual Build Tasks:**
```bash
# Build specific components
make mk_build_all         # Build everything
make mk_build_kompute     # Core library only
make mk_build_tests       # Test suite
make mk_build_benchmark   # Performance benchmarks
make mk_build_docs        # Documentation

# Run tests with different configurations
make mk_run_tests         # Standard GPU tests
make mk_run_tests_cpu     # CPU-only tests (SwiftShader)
```

**Development Tools:**
```bash
make format              # Format code with clang-format
make static_scan         # Run cppcheck static analysis
make build_shaders       # Rebuild GLSL shaders to C++ headers
```

## Core Architecture & Vulkan Integration

Kompute's architecture maps directly onto Vulkan SDK concepts, with each component managing specific Vulkan resources:

### Manager (`src/Manager.cpp`, `src/include/kompute/Manager.hpp`)
**Primary Orchestrator and Vulkan Context Owner**

- **Vulkan Resources Managed**: `vk::Instance`, `vk::PhysicalDevice`, `vk::Device`, `vk::Queue`
- **Memory Management**: Top-level owner with hierarchical cleanup (acyclic ownership)
- **Initialization Patterns**:
  - Default: `Manager()` - Creates device 0, first queue
  - Custom: `Manager(physicalDeviceIndex, familyQueueIndices, extensions)`
  - BYOV: `Manager(instance, physicalDevice, device)` - External Vulkan integration
- **Key Vulkan Concepts**: Device selection, queue family management, extension loading
- **Debug Integration**: Validation layer callback setup when `KOMPUTE_DISABLE_VK_DEBUG_LAYERS` not set

### Memory Management Architecture
**Explicit Ownership with Vulkan Buffer/Memory Lifecycle**

Core principles from `docs/overview/memory-management.rst`:
- Memory management only for Kompute-created resources
- Acyclic ownership hierarchy with single top manager
- Operations do not manage GPU memory or resources
- Resources cannot be recreated once destroyed, only rebuilt if not destroyed

**Memory Types** (`src/include/kompute/Memory.hpp`, `src/include/kompute/Core.hpp`):
- `eDevice`: GPU-only memory (vk::MemoryPropertyFlagBits::eDeviceLocal)
- `eHost`: CPU-accessible memory (vk::MemoryPropertyFlagBits::eHostVisible)
- `eStorage`: Optimized for large data transfers
- `eDeviceAndHost`: Accessible from both CPU and GPU

### Tensor (`src/Tensor.cpp`, `src/include/kompute/Tensor.hpp`)
**Fundamental Data Structure with Vulkan Buffer Management**

- **Vulkan Resources**: `vk::Buffer`, `vk::DeviceMemory`, descriptor bindings
- **Data Types Supported**: `float`, `double`, `int32_t`, `uint32_t`, `bool` (defined in `DataTypes` enum)
- **Memory Patterns**:
  - Host vector (`std::vector<T>`) for CPU data
  - Vulkan buffer for GPU operations
  - Explicit sync operations between host/device
- **Key Operations**: `recordCopyFrom()`, `recordCopyBuffer()`, memory mapping
- **Vulkan Integration**: Buffer creation with usage flags, memory allocation from device heap

### Image (`src/Image.cpp`, `src/include/kompute/Image.hpp`)
**2D/3D Data Structure with Vulkan Image Management**

- **Vulkan Resources**: `vk::Image`, `vk::DeviceMemory`, `vk::ImageView`
- **Image Formats**: Extensive format support through `vk::Format`
- **Usage Patterns**: Texture operations, image processing, ML convolutions
- **Memory Layout**: Handles Vulkan image memory requirements and alignment

### Sequence (`src/Sequence.cpp`, `src/include/kompute/Sequence.hpp`)
**Command Buffer Orchestration with Vulkan Command Recording**

- **Vulkan Resources**: `vk::CommandBuffer`, `vk::CommandPool`, `vk::Fence`
- **Execution Modes**:
  - `record<OpType>()` - Record commands without execution
  - `eval<OpType>()` - Immediate record and execute
  - `evalAsync()` - Asynchronous execution with fence synchronization
- **Vulkan Integration**: Command buffer lifecycle, queue submission, fence waiting
- **Queue Management**: Associates with specific GPU queue families for parallel execution

### Operations (`src/operations/`)
**Vulkan Command Recording Abstractions**

**Base Operation** (`OpBase.hpp`):
- Pure virtual `record()` method for Vulkan command buffer recording
- No GPU memory management (handled by Tensors/Images)

**Core Operations**:
- **`OpSyncDevice`**: Host→Device memory transfer (`vkCmdCopyBuffer` from staging)
- **`OpSyncLocal`**: Device→Host memory transfer (memory mapping/staging buffer)
- **`OpCopy`**: Device-to-device memory copy (`vkCmdCopyBuffer`)
- **`OpMemoryBarrier`**: Vulkan memory barriers (`vkCmdPipelineBarrier`)
- **`OpAlgoDispatch`**: Compute shader dispatch (`vkCmdDispatch`)

### Algorithm (`src/Algorithm.cpp`, `src/include/kompute/Algorithm.hpp`)
**Compute Shader Pipeline with Vulkan Compute Resources**

**Vulkan Dependencies and Design Constraints**:
- **`vk::ShaderModule`**: Created from SPIR-V bytecode
- **`vk::DescriptorSetLayout`**: Must match shader binding declarations
- **`vk::PipelineLayout`**: Defines push constants and descriptor set layouts
- **`vk::Pipeline`**: Compute pipeline linking shader, layout, and specialization

**Critical Vulkan Constraints**:
- **Descriptor Set Binding**: Once created, binding indices are immutable
- **Push Constants**: Size fixed at pipeline creation, values updatable per dispatch
- **Specialization Constants**: Set at pipeline creation, cannot be modified
- **Shader Compilation**: SPIR-V must be compiled with matching interface

**Workgroup Management**:
- Local workgroup size defined in shader (`local_size_x/y/z`)
- Global dispatch size calculated: `(elementCount + localSize - 1) / localSize`

### Logger (`src/logger/`, `src/include/kompute/logger/Logger.hpp`)
**Configurable Logging with Compile-time Optimization**

- **Build-time Configuration**: `KOMPUTE_OPT_LOG_LEVEL` (Trace, Debug, Info, Warn, Error, Critical, Off)
- **Backend Options**: spdlog (async/sync) or fmt-only
- **Macro System**: `KP_LOG_DEBUG()`, `KP_LOG_INFO()`, etc.
- **Performance**: Compiled out entirely when `KOMPUTE_OPT_LOG_LEVEL=Off`

## CMake Utilities & Build System

### Shader Compilation Pipeline (`cmake/vulkan_shader_compiler.cmake`)
**GLSL to C++ Header Conversion**

```cmake
vulkan_compile_shader(
    INFILE shader/compute.comp
    OUTFILE include/ComputeShader.hpp
    NAMESPACE kp
)
```

**Process**: `.comp` → glslangValidator → `.spv` → cmake script → `.hpp`
**Usage**: Embeds SPIR-V as C++ byte arrays, enables compile-time shader inclusion

### Additional CMake Utilities (`cmake/`)
- **`bin_file_to_header.cmake`**: Binary to C++ header conversion
- **`check_vulkan_version.cmake`**: Runtime Vulkan version validation
- **`code_coverage.cmake`**: gcov/lcov integration for test coverage
- **`deprecation_warnings.cmake`**: Handles deprecated CMake options

### Build Configuration Options
**Core Development Flags**:
- `KOMPUTE_OPT_BUILD_TESTS=ON`: Enable GoogleTest suite
- `KOMPUTE_OPT_USE_SPDLOG=ON`: Use spdlog vs fmt for logging
- `KOMPUTE_OPT_LOG_LEVEL=Debug`: Set compile-time log level
- `KOMPUTE_OPT_DISABLE_VK_DEBUG_LAYERS=ON`: Disable Vulkan validation
- `KOMPUTE_OPT_BUILD_SHADERS=ON`: Rebuild shaders from source

## Test Architecture & Coverage

### Test Structure (`test/CMakeLists.txt`)
**Comprehensive GoogleTest Suite**:
- **Total Test Files**: 18 test files covering all major components
- **Test Executable**: `kompute_tests` with GTest integration
- **Dependencies**: Links against `kompute::kompute`, `kp_logger`, test shaders

### Key Test Categories
**Core Component Tests**:
- `TestManager.cpp`: Manager initialization, device selection, queue management
- `TestTensor.cpp`: Tensor creation, memory management, data types
- `TestImage.cpp`: Image operations, format handling
- `TestSequence.cpp`: Command recording, execution patterns, async operations
- `TestAlgorithm.cpp` (via multiple files): Shader dispatch, pipeline management

**Operations Testing**:
- `TestOpSync.cpp`: Host/device synchronization
- `TestOpCopy*.cpp`: Memory copy operations between tensors/images
- `TestOpShadersFromStringAndFile.cpp`: Shader loading mechanisms

**Advanced Features**:
- `TestAsyncOperations.cpp`: Parallel execution, fence synchronization
- `TestPushConstant.cpp`: Push constant parameter passing
- `TestSpecializationConstant.cpp`: Compile-time shader parameters
- `TestWorkgroup.cpp`: Dispatch size calculation

### CI & Testing Infrastructure
**SwiftShader Integration**: CPU-based Vulkan implementation for CI
- **Limitations**: Some Vulkan extensions not supported in SwiftShader
- **Filtered Tests**: `FILTER_TESTS` excludes SwiftShader-incompatible tests
- **Coverage**: Achieves ~90% test coverage as documented

**Test Execution**:
```bash
# GPU tests (requires Vulkan driver)
./build/bin/kompute_tests

# CPU tests (SwiftShader)
VK_ICD_FILENAMES=./swiftshader/build/vk_swiftshader_icd.json ./build/bin/kompute_tests
```

## Directory Structure & Build Artifacts

### Source Organization
```
src/
├── include/kompute/          # Public C++ API headers
│   ├── operations/           # Operation class definitions
│   └── logger/              # Logging infrastructure
├── shaders/glsl/            # GLSL compute shaders
├── [Component].cpp          # Implementation files
└── CMakeLists.txt          # Core library build

test/
├── shaders/glsl/           # Test-specific shaders
├── Test[Component].cpp     # GoogleTest test files
└── CMakeLists.txt         # Test executable build

cmake/                      # Build system utilities
examples/                   # Example applications
docs/                      # Sphinx documentation
benchmark/                 # Performance benchmarks
```

### Build Outputs
- **`build/bin/`**: Executables (tests, benchmarks, examples)
- **`build/lib/`**: Libraries (`libkompute`, `libkp_logger`)
- **`build/docs/`**: Generated documentation (Doxygen + Sphinx)

## Vulkan Compute Shader Context

When developing with Kompute, understanding Vulkan compute concepts is essential:

### Descriptor Sets & Pipeline Dependencies
- **Descriptor Bindings**: Must match between GLSL `layout(set=0, binding=N)` and C++ descriptor set allocation
- **Pipeline Creation**: Expensive operation, reuse algorithms where possible
- **Specialization Constants**: `layout(constant_id = N)` for compile-time parameters
- **Push Constants**: `layout(push_constant)` for per-dispatch parameters

### Memory Barriers & Synchronization
- **Pipeline Barriers**: Required between compute dispatch and memory operations
- **Access Masks**: `VK_ACCESS_SHADER_WRITE_BIT` → `VK_ACCESS_TRANSFER_READ_BIT`
- **Stage Masks**: `VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT` dependencies

### Performance Considerations
- **Local Workgroup Size**: Balance between occupancy and resource usage
- **Memory Coalescing**: Structure data access patterns for GPU efficiency
- **Buffer Usage Flags**: Match Vulkan usage flags to actual operations

## Development Context & Resources

For comprehensive Kompute development, consider:

**Vulkan Resources**:
- [Vulkan Compute Shader Tutorial](https://vulkan-tutorial.com/Compute_Shader)
- Vulkan specification for compute pipeline details
- Vulkan memory model for synchronization primitives

**Project Resources**:
- [GitHub Issues](https://github.com/KomputeProject/kompute/issues) for active development discussions
- Documentation architecture diagrams in `docs/images/`
- Real-world usage examples in `examples/` directory

**Architecture Decision Context**:
- Vulkan SDK dependencies influence Algorithm design (descriptor sets, pipelines)
- Memory management principles from BYOV design philosophy
- Cross-platform support requirements (Windows, Linux, macOS, Android)

---

## Python Development

For Python development guidance, view `python/CLAUDE.md` which contains all details related to Python bindings, pybind11 integration, and Python-specific development workflows.
