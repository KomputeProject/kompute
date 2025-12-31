# Kompute Codebase Analysis Report

**Date**: December 31, 2025  
**Version Analyzed**: 0.9.0  
**Test Results**: 106/107 tests passing  

---

## Executive Summary

This report provides a comprehensive analysis of the Kompute GPU compute framework codebase, identifying potential bugs, suboptimal designs, and improvement opportunities. The analysis covers the C++ core library (`src/`), Python bindings (`python/`), CMake build system (`cmake/`), tests (`test/`), and documentation (`docs/`).

Kompute is a well-designed Vulkan-based GPU compute framework with solid architecture, but there are several areas for improvement related to memory safety, API design, code maintainability, and modern C++ practices.

---

## Table of Contents

1. [Critical Issues](#1-critical-issues)
2. [Memory Management Issues](#2-memory-management-issues)
3. [API Design Concerns](#3-api-design-concerns)
4. [Code Quality & Maintainability](#4-code-quality--maintainability)
5. [Build System Issues](#5-build-system-issues)
6. [Test Coverage Gaps](#6-test-coverage-gaps)
7. [Documentation Issues](#7-documentation-issues)
8. [Performance Optimization Opportunities](#8-performance-optimization-opportunities)
9. [Security Considerations](#9-security-considerations)
10. [Modernization Opportunities](#10-modernization-opportunities)
11. [Recommendations Summary](#11-recommendations-summary)

---

## 1. Critical Issues

### 1.1 Memory Leak in Algorithm Push Constants (HIGH PRIORITY)

**Location**: `src/include/kompute/Algorithm.hpp:243-250`

**Issue**: The `setPushConstants` method allocates memory with `malloc()` but if an exception occurs after allocation, memory is leaked.

```cpp
void setPushConstants(void* data, uint32_t size, uint32_t memorySize) {
    // ...
    if (this->mPushConstantsData) {
        free(this->mPushConstantsData);  // Freed before new allocation
    }
    this->mPushConstantsData = malloc(totalSize);
    memcpy(this->mPushConstantsData, data, totalSize);  // If this throws, mPushConstantsData leaks
}
```

**Recommendation**: Use RAII wrappers or smart pointers instead of raw `malloc/free`.

---

### 1.2 Potential Use-After-Free in Destructor Chain

**Location**: `src/Algorithm.cpp:41-51`

**Issue**: In `Algorithm::destroy()`, there's a redundant null check pattern that's error-prone:

```cpp
if (this->mFreePipeline && this->mPipeline) {
    KP_LOG_DEBUG("Kompute Algorithm Destroying pipeline");
    if (!this->mPipeline) {  // This is always false here!
        KP_LOG_WARN("...");
    }
    this->mDevice->destroy(*this->mPipeline, ...);
}
```

**Recommendation**: Remove the redundant inner null checks.

---

### 1.3 Uninitialized Member Variable

**Location**: `src/include/kompute/Sequence.hpp:284`

**Issue**: `mQueueIndex` is initialized to `-1` which as `uint32_t` becomes `UINT32_MAX`:

```cpp
uint32_t mQueueIndex = -1;
```

**Recommendation**: Use a valid sentinel value or make initialization explicit.

---

### 1.4 Race Condition in Async Operations

**Location**: `src/Sequence.cpp:116-120`

**Issue**: The test `TestAsyncOperations.TestManagerAsyncExecutionTimeout` is failing. The issue is in `evalAsync()` which doesn't properly handle timeout scenarios:

```cpp
std::shared_ptr<Sequence>
Sequence::evalAwait(uint64_t waitFor)
{
    // ...
    if (result == vk::Result::eTimeout) {
        KP_LOG_WARN("...");
        return shared_from_this();  // postEval is not called on timeout!
    }
    // ...
}
```

**Recommendation**: Decide on consistent behavior for timeout scenarios.

---

## 2. Memory Management Issues

### 2.1 Manual Memory Management with malloc/free

**Locations**:
- `src/include/kompute/Algorithm.hpp:108-126`
- `src/include/kompute/operations/OpAlgoDispatch.hpp:39-42`
- `src/Algorithm.cpp:27-33` (commented out but relevant)

**Issue**: Using `malloc/free` instead of C++ idioms is error-prone.

**Code Example**:
```cpp
this->mSpecializationConstantsData = malloc(totalSize);
memcpy(this->mSpecializationConstantsData, specializationConstants.data(), totalSize);
```

**Recommendation**: Replace with `std::vector<std::byte>` or `std::unique_ptr<std::byte[]>`.

---

### 2.2 Missing Virtual Destructor Protection

**Location**: `src/include/kompute/Memory.hpp:80`

**Issue**: The `Memory` base class destructor is empty but not pure virtual, which could lead to incomplete cleanup if derived class destructors aren't called properly.

```cpp
virtual ~Memory(){};
```

**Recommendation**: Consider `= default` syntax and ensure proper cleanup chain.

---

### 2.3 Weak Pointer Container Cleanup

**Location**: `src/Manager.cpp:306-326`

**Issue**: The `clear()` method uses `erase-remove_if` idiom correctly, but the pattern could be encapsulated in a utility function for DRY compliance.

**Recommendation**: Create a helper template function for weak_ptr cleanup.

---

## 3. API Design Concerns

### 3.1 Inconsistent Method Naming

**Locations**: Various

**Issues**:
- `recordCopyFromStagingToDevice` vs `recordPrimaryMemoryBarrier` (inconsistent naming patterns)
- `memoryType()` vs `dataType()` (one with parentheses, consistency is good though)
- `isInit()` vs `isRecording()` vs `isRunning()` (good consistency here)

**Recommendation**: Document naming conventions in CONTRIBUTING.md.

---

### 3.2 Template-Heavy Python Bindings

**Location**: `python/src/main.cpp:534-706`

**Issue**: The algorithm binding code has massive code duplication for different dtype combinations (16+ nearly identical code blocks).

```cpp
if (spec_consts.dtype().is(py::dtype::of<std::float_t>())) {
    // ... 50 lines of code
} else if (spec_consts.dtype().is(py::dtype::of<std::int32_t>())) {
    // ... same 50 lines again
}
// ... repeated 16 times!
```

**Recommendation**: Refactor using a type visitor pattern or template helper.

---

### 3.3 Error Handling Inconsistency

**Locations**: Throughout codebase

**Issue**: Mix of exception throwing and silent failures:
- Some methods throw `std::runtime_error`
- Some log warnings and return
- Some log errors and continue

**Recommendation**: Document error handling strategy and apply consistently.

---

### 3.4 Missing noexcept Specifications

**Locations**: Many member functions

**Issue**: Move constructors and destructors should be `noexcept` for proper STL container behavior. Currently inconsistent.

**Examples**:
- `Algorithm::~Algorithm() noexcept` ✓
- `Tensor::~Tensor()` ✗ (missing noexcept)
- `Image::~Image()` ✗ (missing noexcept)

**Recommendation**: Add `noexcept` to destructors and move operations.

---

## 4. Code Quality & Maintainability

### 4.1 Typo in Log Message

**Location**: `src/Tensor.cpp:334`

**Issue**: "phyisical" should be "physical"

```cpp
throw std::runtime_error("Kompute Tensor phyisical device is null");
```

**Recommendation**: Fix typo.

---

### 4.2 Inconsistent Typo Pattern

**Locations**: 
- `src/Tensor.cpp:453,456,470` - "Kompose" instead of "Kompute"
- `src/Memory.cpp:305,319` - "Kompose" instead of "Kompute"
- `src/Image.cpp:499,508,625-649` - "Kompose" instead of "Kompute"

**Recommendation**: Global search and replace "Kompose" → "Kompute".

---

### 4.3 Commented-Out Code

**Location**: `src/Algorithm.cpp:27-33, 89-101`

**Issue**: Large blocks of commented-out code reduce readability.

```cpp
// We don't call freeDescriptorSet as the descriptor pool is not created
// with VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT more at
// (https://www.khronos.org/registry/vulkan/specs/...)
// if (this->mFreeDescriptorSet && this->mDescriptorSet) {
//    ...
```

**Recommendation**: Remove commented code, add documentation explaining why descriptor sets aren't freed.

---

### 4.4 Duplicate cmake_minimum_required

**Location**: `src/CMakeLists.txt:3,9`

```cmake
cmake_minimum_required(VERSION 3.20)
# ...
cmake_minimum_required(VERSION 3.20)  # Duplicate!
```

**Recommendation**: Remove duplicate.

---

### 4.5 Magic Numbers

**Locations**: Various

**Examples**:
```cpp
// src/Image.cpp:30-35
if (numChannels > 4 || numChannels == 3) {
    throw std::runtime_error("Kompute Images can only have up to 1, 2 or 4 channels");
}
```

**Recommendation**: Define constants like `MAX_IMAGE_CHANNELS = 4`.

---

### 4.6 Unused Parameter in Function

**Location**: `src/Tensor.cpp:172-173,184-185`

**Issue**: `bufferSize` parameter is marked as unused but still present:

```cpp
void Tensor::recordCopyBuffer(..., vk::DeviceSize /*bufferSize*/, ...)
```

**Recommendation**: Either use the parameter or remove it from the interface.

---

## 5. Build System Issues

### 5.1 Outdated GoogleTest Deprecation Warnings

**Location**: Build output, CMakeLists.txt:203-209

**Issue**: GoogleTest 1.11.0 uses deprecated CMake practices:

```
CMake Deprecation Warning at build/_deps/googletest-src/CMakeLists.txt:4
```

**Recommendation**: Update to GoogleTest 1.14.0 or newer.

---

### 5.2 Hardcoded Paths in Makefile

**Location**: `Makefile:11-12, 19-32`

**Issue**: Windows-specific hardcoded paths:

```makefile
VCPKG_WIN_PATH ?= "C:\\Users\\axsau\\Programming\\lib\\vcpkg\\..."
```

**Recommendation**: Use environment variables or CMake presets.

---

### 5.3 Spdlog Build Failure with Some Toolchains

**Location**: Observed during build

**Issue**: When `KOMPUTE_OPT_USE_SPDLOG=1` is set, builds can fail with certain toolchain configurations due to missing STL headers.

**Recommendation**: Add better toolchain detection and fallback.

---

### 5.4 Missing CMake Install Namespace Consistency

**Location**: `src/CMakeLists.txt`

**Issue**: The library alias is `kompute::kompute` but install target is just `kompute`.

**Recommendation**: Ensure consistency between build and install interface.

---

## 6. Test Coverage Gaps

### 6.1 Failing Async Test

**Test**: `TestAsyncOperations.TestManagerAsyncExecutionTimeout`

**Status**: FAILING

**Issue**: The test expects operations to timeout and return quickly, but the implementation doesn't handle this case correctly on all hardware.

**Recommendation**: Review timeout behavior and update test expectations.

---

### 6.2 Missing Edge Case Tests

**Areas lacking coverage**:
- Zero-sized tensor/image creation (error paths)
- Memory exhaustion scenarios
- Invalid SPIRV handling
- Concurrent access patterns
- Resource cleanup after errors

---

### 6.3 Platform-Specific Test Exclusions

**Location**: `Makefile:15`

```makefile
FILTER_TESTS ?= "-TestAsyncOperations.TestManagerParallelExecution:..."
```

**Issue**: Several tests are excluded by default without documentation of why.

**Recommendation**: Add comments explaining platform-specific test limitations.

---

## 7. Documentation Issues

### 7.1 Outdated README Version Badge

**Location**: `README.md:2`

```markdown
![GitHub](https://img.shields.io/badge/Version-0.7.0-green.svg)
```

**Issue**: Shows version 0.7.0 but codebase is 0.9.0.

**Recommendation**: Update or use dynamic badge.

---

### 7.2 Missing API Migration Guide

**Issue**: No documentation for API changes between versions.

**Recommendation**: Add MIGRATION.md for version upgrades.

---

### 7.3 Incomplete Doxygen Comments

**Locations**: Some public methods lack documentation

**Examples**:
- `Memory::getX()`, `Memory::getY()` have minimal docs
- Template specializations lack documentation

**Recommendation**: Complete documentation for public API.

---

### 7.4 Broken Links in Docs

**Location**: `README.md`, various external links

**Recommendation**: Audit and update all external links.

---

## 8. Performance Optimization Opportunities

### 8.1 Unnecessary String Copies

**Location**: `src/Manager.cpp:223-227`

```cpp
std::vector<std::string> envLayerNames;
// ...
for (const std::string& layerName : envLayerNames) {
    desiredLayerNames.push_back(layerName.c_str());  // Dangling pointer risk!
}
```

**Issue**: This code stores `c_str()` pointers that may become invalid after the vector is modified.

**Recommendation**: Ensure `envLayerNames` lifetime covers all uses of pointers.

---

### 8.2 Repeated Memory Property Queries

**Location**: `src/Tensor.cpp:403-404, src/Image.cpp:576-577`

**Issue**: `getMemoryProperties()` is called for each buffer/image allocation.

**Recommendation**: Cache memory properties at Manager level.

---

### 8.3 Vector Resizing in Loops

**Location**: `src/Algorithm.cpp:206-216`

```cpp
for (size_t i = 0; i < this->mMemObjects.size(); i++) {
    std::vector<vk::WriteDescriptorSet> computeWriteDescriptorSets;
    // ...
    computeWriteDescriptorSets.push_back(descriptorSet);
    this->mDevice->updateDescriptorSets(computeWriteDescriptorSets, nullptr);
}
```

**Recommendation**: Batch descriptor set updates.

---

### 8.4 Suboptimal Barrier Usage

**Location**: `src/OpAlgoDispatch.cpp:23-46`

**Issue**: Individual barriers are recorded for each memory object in a loop.

**Recommendation**: Batch barriers where possible for better GPU efficiency.

---

## 9. Security Considerations

### 9.1 Environment Variable Injection

**Location**: `src/Manager.cpp:215-227`

**Issue**: `KOMPUTE_ENV_DEBUG_LAYERS` environment variable is parsed without validation.

```cpp
const char* envLayerNamesVal = std::getenv("KOMPUTE_ENV_DEBUG_LAYERS");
// ... directly used without sanitization
```

**Recommendation**: Add input validation for environment variables.

---

### 9.2 Unchecked Memory Allocation

**Location**: `src/include/kompute/Algorithm.hpp:108, 120`

**Issue**: `malloc()` return value is not checked for null.

```cpp
this->mSpecializationConstantsData = malloc(totalSize);
// No null check!
memcpy(this->mSpecializationConstantsData, ...);
```

**Recommendation**: Check malloc return value or use `new` with exceptions.

---

### 9.3 Integer Overflow Potential

**Location**: `src/include/kompute/Algorithm.hpp:104-107`

```cpp
uint32_t memorySize = sizeof(decltype(specializationConstants.back()));
uint32_t size = specializationConstants.size();
uint32_t totalSize = size * memorySize;  // Potential overflow!
```

**Recommendation**: Add overflow checks or use `size_t`.

---

## 10. Modernization Opportunities

### 10.1 C++14 to C++17 Migration

**Current**: C++14 (`CMAKE_CXX_STANDARD 14`)

**Benefits of C++17**:
- `std::optional` for nullable returns
- `std::string_view` for string parameters
- `if constexpr` for template metaprogramming
- Structured bindings
- `[[nodiscard]]` attribute

**Recommendation**: Consider C++17 upgrade path.

---

### 10.2 Smart Pointer Improvements

**Current**: Uses `std::shared_ptr` extensively

**Opportunities**:
- Some `shared_ptr` could be `unique_ptr` (Algorithm ownership)
- Consider `std::make_unique`/`std::make_shared` consistently

---

### 10.3 Span for Array Parameters

**Current**: `std::vector<T>&` parameters

**Opportunity**: `std::span<T>` (C++20) or custom span for non-owning views

---

### 10.4 Constexpr Improvements

**Current**: Limited `constexpr` usage

**Opportunity**: Make more compile-time computation possible:
```cpp
static constexpr uint32_t dataTypeMemorySize(DataTypes dt);  // Could be constexpr
```

---

## 11. Recommendations Summary

### Priority 1: Critical (Fix Immediately)

| Issue | Location | Effort |
|-------|----------|--------|
| Memory leak in setPushConstants | Algorithm.hpp | Low |
| Unchecked malloc | Algorithm.hpp | Low |
| Fix failing async test | Sequence.cpp | Medium |
| Remove redundant null checks | Algorithm.cpp | Low |

### Priority 2: High (Fix Soon)

| Issue | Location | Effort |
|-------|----------|--------|
| Replace malloc/free with RAII | Algorithm.hpp, OpAlgoDispatch.hpp | Medium |
| Fix all "Kompose" typos | Multiple files | Low |
| Add noexcept to destructors | Tensor.hpp, Image.hpp | Low |
| Fix dangling pointer in env parsing | Manager.cpp | Medium |

### Priority 3: Medium (Plan for Next Release)

| Issue | Location | Effort |
|-------|----------|--------|
| Refactor Python bindings | main.cpp | High |
| Update GoogleTest version | CMakeLists.txt | Low |
| Add integer overflow checks | Algorithm.hpp | Medium |
| Batch descriptor set updates | Algorithm.cpp | Medium |
| Cache memory properties | Manager/Tensor | Medium |

### Priority 4: Low (Technical Debt)

| Issue | Location | Effort |
|-------|----------|--------|
| Remove commented code | Algorithm.cpp | Low |
| Document naming conventions | CONTRIBUTING.md | Low |
| Update README version badge | README.md | Low |
| Add edge case tests | test/ | High |
| Consider C++17 migration | CMakeLists.txt | High |

---

## Appendix A: Files Analyzed

### Core C++ Source
- `src/Algorithm.cpp`
- `src/Core.cpp`
- `src/Image.cpp`
- `src/Manager.cpp`
- `src/Memory.cpp`
- `src/OpAlgoDispatch.cpp`
- `src/OpCopy.cpp`
- `src/OpMemoryBarrier.cpp`
- `src/OpSyncDevice.cpp`
- `src/OpSyncLocal.cpp`
- `src/Sequence.cpp`
- `src/Tensor.cpp`

### Headers
- `src/include/kompute/*.hpp`
- `src/include/kompute/operations/*.hpp`
- `src/include/kompute/logger/*.hpp`

### Python Bindings
- `python/src/main.cpp`
- `python/src/utils.hpp`
- `python/test/*.py`

### Build System
- `CMakeLists.txt`
- `src/CMakeLists.txt`
- `cmake/*.cmake`
- `Makefile`

### Tests
- `test/*.cpp`

### Documentation
- `README.md`
- `CONTRIBUTING.md`
- `docs/overview/*.rst`

---

## Appendix B: Test Execution Results

```
[==========] 107 tests from 25 test suites ran. (16743 ms total)
[  PASSED  ] 106 tests.
[  FAILED  ] 1 test, listed below:
[  FAILED  ] TestAsyncOperations.TestManagerAsyncExecutionTimeout
```

---

## Appendix C: Build Configuration Used

```bash
cmake -Bbuild \
  -DCMAKE_BUILD_TYPE=Release \
  -DKOMPUTE_OPT_BUILD_TESTS=ON \
  -DKOMPUTE_OPT_ENABLE_BENCHMARK=OFF \
  -DKOMPUTE_OPT_BUILD_DOCS=OFF \
  -DKOMPUTE_OPT_CODE_COVERAGE=OFF \
  -DKOMPUTE_OPT_USE_SPDLOG=OFF \
  -DKOMPUTE_OPT_DISABLE_VK_DEBUG_LAYERS=ON \
  -DKOMPUTE_OPT_DISABLE_VULKAN_VERSION_CHECK=ON
```

---

## Implementation Plan

The following issues have been selected for implementation:

### Phase 1: Critical & Memory Safety Fixes

#### Task 1.1: Fix Memory Leak in Algorithm Push Constants
**Priority**: Critical  
**Files**: `src/include/kompute/Algorithm.hpp`  
**Changes**:
- Replace `void* mPushConstantsData` with `std::vector<uint8_t>`
- Replace `void* mSpecializationConstantsData` with `std::vector<uint8_t>`
- Remove manual `malloc/free` calls in `rebuild()` and `setPushConstants()`
- Update destructor to remove `free()` calls (now automatic)

**Implementation Steps**:
1. Change member declarations from `void*` to `std::vector<uint8_t>`
2. Update `rebuild()` template to use vector resize/copy
3. Update `setPushConstants()` to use vector operations
4. Update `getSpecializationConstants()` and `getPushConstants()` to work with vector
5. Verify specialization info and push constant binding still work

---

#### Task 1.2: Remove Redundant Null Checks in Algorithm::destroy()
**Priority**: Critical  
**Files**: `src/Algorithm.cpp`  
**Changes**:
- Remove inner `if (!this->mPipeline)` checks that are always false
- Apply same fix for mPipelineCache, mPipelineLayout, mShaderModule, mDescriptorSetLayout, mDescriptorPool

**Implementation Steps**:
1. Remove 6 redundant inner null checks in `destroy()` method
2. Keep outer conditional checks intact
3. Verify tests still pass

---

#### Task 2.2: Improve Virtual Destructor in Memory Base Class
**Priority**: High  
**Files**: `src/include/kompute/Memory.hpp`  
**Changes**:
- Change `virtual ~Memory(){};` to `virtual ~Memory() = default;`

**Implementation Steps**:
1. Update destructor declaration
2. Verify derived class destructors are called correctly

---

### Phase 2: API & Code Quality Improvements

#### Task 3.2: Refactor Python Bindings Code Duplication
**Priority**: Medium (larger change - Phase 2)  
**Files**: `python/src/main.cpp`  
**Changes**:
- Create helper template function for dtype dispatch
- Reduce 16 code blocks to single template-based implementation

**Implementation Steps**:
1. Create `dispatchByDtype()` helper function template
2. Refactor algorithm binding to use helper
3. Reduce code from ~200 lines to ~50 lines
4. Test all dtype combinations

---

#### Task 3.4: Add noexcept to Destructors
**Priority**: High  
**Files**: 
- `src/include/kompute/Tensor.hpp`
- `src/include/kompute/Image.hpp`  
**Changes**:
- Add `noexcept` specifier to destructors

**Implementation Steps**:
1. Update `Tensor::~Tensor()` to `virtual ~Tensor() noexcept;`
2. Update `Image::~Image()` to `virtual ~Image() noexcept;`
3. Update implementation files if needed

---

### Phase 3: Code Quality & Typo Fixes

#### Task 4.1: Fix "phyisical" Typo
**Priority**: Low  
**Files**: `src/Tensor.cpp`  
**Changes**:
- Fix typo "phyisical" → "physical"

---

#### Task 4.2: Fix "Kompose" Typos Throughout Codebase
**Priority**: Low  
**Files**: 
- `src/Tensor.cpp`
- `src/Memory.cpp`
- `src/Image.cpp`  
**Changes**:
- Replace all occurrences of "Kompose" with "Kompute"

**Implementation Steps**:
1. Search for all "Kompose" occurrences
2. Replace with "Kompute"
3. Verify no functional changes

---

#### Task 4.3: Remove Commented-Out Code in Algorithm.cpp
**Priority**: Low  
**Files**: `src/Algorithm.cpp`  
**Changes**:
- Remove large commented-out code blocks
- Add concise documentation comment explaining why descriptor sets aren't freed

---

#### Task 4.4: Remove Duplicate cmake_minimum_required
**Priority**: Low  
**Files**: `src/CMakeLists.txt`  
**Changes**:
- Remove duplicate `cmake_minimum_required(VERSION 3.20)` on line 9

---

#### Task 4.5: Define Constants for Magic Numbers
**Priority**: Low  
**Files**: `src/Image.cpp`, `src/include/kompute/Image.hpp`  
**Changes**:
- Define `constexpr` for max image channels
- Replace magic number checks with named constant

---

### Phase 4: Build System & Test Improvements

#### Task 5.1: Update GoogleTest Version
**Priority**: Medium  
**Files**: `CMakeLists.txt`  
**Changes**:
- Update GoogleTest from release-1.11.0 to release-1.14.0

**Implementation Steps**:
1. Change GIT_TAG from `release-1.11.0` to `release-1.14.0`
2. Rebuild and verify tests pass

---

#### Task 6.2: Add Missing Edge Case Tests
**Priority**: Medium  
**Files**: `test/` (new test cases)  
**Changes**:
- Add test for zero-sized tensor creation (should throw)
- Add test for invalid SPIRV handling
- Add test for resource cleanup after errors

**Implementation Steps**:
1. Add `TestTensorEdgeCases` test suite
2. Add `TestAlgorithmEdgeCases` test suite
3. Verify error paths work correctly

---

### Phase 5: Performance & Safety

#### Task 8.1: Fix Dangling Pointer Risk in Environment Parsing
**Priority**: High  
**Files**: `src/Manager.cpp`  
**Changes**:
- Ensure `envLayerNames` vector lifetime covers all pointer uses
- Or copy strings to persistent storage before pushing `c_str()` pointers

**Implementation Steps**:
1. Move `desiredLayerNames` population after all modifications to `envLayerNames`
2. Or store strings directly and extract pointers only when needed
3. Add comment explaining lifetime requirements

---

### Phase 6: Error Handling Consistency (Follow-up)

#### Task 3.3: Document and Apply Consistent Error Handling
**Priority**: Medium (follow-up task)  
**Files**: Multiple  
**Changes**:
- Document error handling strategy in CONTRIBUTING.md
- Audit all error paths for consistency
- Apply consistent pattern (throw vs log+return)

---

## Implementation Order

```
Week 1: Critical Fixes
├── Task 1.1: Memory leak fix (Algorithm.hpp) - 2 hours
├── Task 1.2: Redundant null checks (Algorithm.cpp) - 30 min
├── Task 2.2: Virtual destructor (Memory.hpp) - 15 min
└── Task 8.1: Dangling pointer fix (Manager.cpp) - 1 hour

Week 2: Code Quality
├── Task 3.4: Add noexcept (Tensor.hpp, Image.hpp) - 30 min
├── Task 4.1: Fix "phyisical" typo - 5 min
├── Task 4.2: Fix "Kompose" typos - 15 min
├── Task 4.3: Remove commented code - 30 min
├── Task 4.4: Remove duplicate cmake - 5 min
└── Task 4.5: Define magic number constants - 30 min

Week 3: Build & Tests
├── Task 5.1: Update GoogleTest - 30 min
└── Task 6.2: Add edge case tests - 2 hours

Week 4: Larger Refactoring
├── Task 3.2: Python bindings refactor - 4 hours
└── Task 3.3: Error handling documentation - 2 hours
```

## Verification Checklist

After each task:
- [ ] Run `make mk_cmake && make mk_build_all`
- [ ] Run `make mk_run_tests`
- [ ] Verify 106+ tests passing
- [ ] No new compiler warnings

---

*End of Report*
