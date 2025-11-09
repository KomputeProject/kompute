# PLAN.md - Complete shared_ptr Removal from Manager

## Executive Summary
Complete the shared_ptr removal project by eliminating remaining shared_ptr usage in Manager class. This requires systematic transformation of Manager API methods, Sequence interface updates, and comprehensive test file migration.

## Current State Analysis

### Manager Shared_ptr Usage Remaining:
1. **Algorithm Methods (Lines 454-519)**: Return `std::shared_ptr<Algorithm>` with no-op deleters
2. **Image Methods (Lines 194-436)**: All image creation methods return `std::shared_ptr<ImageT<T>>` and `std::shared_ptr<Image>`
3. **Tensor Overloads (Lines 143-183)**: void* parameter tensor methods return `std::shared_ptr<Tensor>`
4. **Hybrid Storage (Lines 563-571)**: Dual storage with `mManagedMemObjects` (weak_ptr) and `mOwnedMemObjects` (unique_ptr)
5. **Sequence Interface (Line 147)**: `eval<T>(std::shared_ptr<Algorithm>)` template expects shared_ptr

### Impact Analysis:
- **Breaking Changes**: All Manager API methods will return raw pointers
- **Test Updates Required**: 18+ test files need systematic updates
- **Sequence API Changes**: Need raw pointer overloads for Algorithm parameters
- **Performance Critical**: Complete shared_ptr elimination in all hot paths

## Phase 7: Complete Manager API Transformation

### Phase 7A: Transform Remaining Manager Methods
**Target: Convert all shared_ptr returns to raw pointers**

1. **Algorithm Methods**:
   - `algorithm()` → return `Algorithm*` instead of `std::shared_ptr<Algorithm>`
   - Remove no-op deleter wrapper creation (lines 509-511)
   - Update documentation to reflect raw pointer ownership

2. **Image Methods** (8 method overloads):
   - `imageT<T>()` templates → return `ImageT<T>*`
   - `image()` overloads → return `Image*`
   - Convert from `std::shared_ptr<ImageT<T>>` construction to `std::unique_ptr` + raw pointer return
   - Apply same pattern as tensorT methods (lines 96-106)

3. **Tensor Overloads**:
   - void* parameter `tensor()` methods → return `Tensor*`
   - Convert shared_ptr construction to unique_ptr ownership pattern

### Phase 7B: Clean Up Dual Storage Systems
**Target: Remove hybrid storage completely**

1. **Remove Managed Collections**:
   - Eliminate `mManagedMemObjects`, `mManagedSequences`, `mManagedAlgorithms` (lines 563-565)
   - Remove `clear()` weak_ptr cleanup logic
   - Update `destroy()` method to only handle owned collections

2. **Consolidate to Direct Ownership**:
   - Keep only `mOwnedMemObjects`, `mOwnedSequences`, `mOwnedAlgorithms` (lines 569-571)
   - Simplify memory management to single ownership model

### Phase 7C: Update Sequence Interface
**Target: Support raw pointer Algorithm parameters**

1. **Add Raw Pointer Overloads**:
   - Add `eval<T>(Algorithm* algorithm, TArgs&&... params)` template
   - Add `record<T>(Algorithm* algorithm, TArgs&&... params)` template
   - Maintain shared_ptr versions for compatibility during transition

2. **Operation Constructor Updates**:
   - Verify operations can accept raw Algorithm pointers
   - Update if needed to accept both raw and shared_ptr

## Phase 8: Systematic Test Migration

### Phase 8A: Priority Test Files (High Usage)
**Start with most impacted test files**

1. **TestManager.cpp** - Uses `mgr.algorithm()` directly
2. **TestLogisticRegression.cpp** - Complex algorithm usage
3. **TestMultipleAlgoExecutions.cpp** - Multiple algorithm patterns
4. **TestSequence.cpp** - Sequence interface testing

**Pattern for each file**:
```cpp
// OLD:
std::shared_ptr<kp::Algorithm> algo = mgr.algorithm();
mgr.sequence()->eval<kp::OpMult>(params, algo);

// NEW:
kp::Algorithm* algo = mgr.algorithm();
mgr.sequence()->eval<kp::OpMult>(params, algo);
```

### Phase 8B: Systematic Remaining Tests (16 files)
**Apply established patterns**

1. **Update Headers**: Add `#include "TestUtils.hpp"`
2. **Convert Declarations**: `std::shared_ptr<kp::TensorT<float>>` → `kp::TensorT<float>*`
3. **Update Sequence Calls**: Use raw pointers directly or TestUtils conversion
4. **Image Method Updates**: Convert image creation calls to raw pointers

### Phase 8C: Benchmark and Examples
1. **benchmark/TestBenchmark.cpp** - Apply same patterns
2. **examples/** directory - Update C++ example applications

## Implementation Strategy

### Incremental Approach:
1. **Phase 7A**: Transform Manager API methods one category at a time
2. **Phase 7B**: Clean up storage after API changes complete
3. **Phase 7C**: Add Sequence overloads before full test migration
4. **Phase 8A**: Migrate priority test files with new APIs
5. **Phase 8B**: Systematic migration of remaining tests
6. **Phase 8C**: Complete with benchmarks and examples

### Risk Mitigation:
- **Compile-Test Each Phase**: Ensure core library builds after each transformation
- **Maintain TestUtils.hpp**: Keep compatibility utilities during transition
- **Document Breaking Changes**: Clear migration guide for each API change
- **Performance Validation**: Verify optimizations preserved throughout

### Success Criteria:
- ✅ Zero shared_ptr usage in Manager public APIs
- ✅ Single ownership model (unique_ptr only)
- ✅ All tests compile and pass with raw pointer APIs
- ✅ Performance improvements maintained (>65% in critical paths)
- ✅ Clean, maintainable codebase with clear ownership contracts

## Estimated Effort:
- **Phase 7**: 2-3 hours (API transformations)
- **Phase 8A**: 2-3 hours (priority test files)
- **Phase 8B**: 4-6 hours (systematic test migration)
- **Phase 8C**: 1-2 hours (benchmarks/examples)
- **Total**: 9-14 hours of systematic work

This plan completes the shared_ptr removal project with full API consistency and optimal performance while maintaining code maintainability.