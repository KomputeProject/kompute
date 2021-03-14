# Changelog

## [v0.7.0](https://github.com/EthicalML/vulkan-kompute/tree/v0.7.0)

[Full Changelog](https://github.com/EthicalML/vulkan-kompute/compare/v0.6.0...v0.7.0)

**Implemented enhancements:**

- Extend non-spdlog print functions to use std::format [\#158](https://github.com/EthicalML/vulkan-kompute/issues/158)
- Add code coverage reports with codecov [\#145](https://github.com/EthicalML/vulkan-kompute/issues/145)
- Explore removing `std::vector mData;` completely from Tensor in favour of always storing data in hostVisible buffer memory \(TBC\) [\#144](https://github.com/EthicalML/vulkan-kompute/issues/144)
- Update all examples to match breaking changes in 0.7.0 [\#141](https://github.com/EthicalML/vulkan-kompute/issues/141)
- Avoid copy when returning python numpy / array [\#139](https://github.com/EthicalML/vulkan-kompute/issues/139)
- Cover all Python & C++ tests in CI  [\#121](https://github.com/EthicalML/vulkan-kompute/issues/121)
- Add C++ Test for Simple Work Groups Example [\#117](https://github.com/EthicalML/vulkan-kompute/issues/117)
- Expose push constants in OpAlgo [\#54](https://github.com/EthicalML/vulkan-kompute/issues/54)
- Expose ability to create barriers in OpTensor operations [\#45](https://github.com/EthicalML/vulkan-kompute/issues/45)
- Create delete function in manager to free / destroy sequence [\#36](https://github.com/EthicalML/vulkan-kompute/issues/36)
- Make specialisation data extensible [\#12](https://github.com/EthicalML/vulkan-kompute/issues/12)
- Support multiple types for Kompute Tensors [\#2](https://github.com/EthicalML/vulkan-kompute/issues/2)
- Added re-record sequence functionality and updated docs [\#171](https://github.com/EthicalML/vulkan-kompute/pull/171) ([axsaucedo](https://github.com/axsaucedo))
- Extend non-spdlog print functions to use fmt::format / fmt::print [\#159](https://github.com/EthicalML/vulkan-kompute/pull/159) ([axsaucedo](https://github.com/axsaucedo))
- Added support for custom SpecializedConstants and removed KomputeWorkgroup class [\#151](https://github.com/EthicalML/vulkan-kompute/pull/151) ([axsaucedo](https://github.com/axsaucedo))
- Added destroy functions for tensors and sequences \(named and object\) [\#146](https://github.com/EthicalML/vulkan-kompute/pull/146) ([axsaucedo](https://github.com/axsaucedo))

**Fixed bugs:**

- push\_constant not working in my case? [\#168](https://github.com/EthicalML/vulkan-kompute/issues/168)
- DescriptorPool set is not being freed [\#155](https://github.com/EthicalML/vulkan-kompute/issues/155)
- Updated memory barriers to include staging buffers [\#182](https://github.com/EthicalML/vulkan-kompute/pull/182) ([axsaucedo](https://github.com/axsaucedo))
- Adds push const ranges in pipelinelayout to fix \#168 [\#174](https://github.com/EthicalML/vulkan-kompute/pull/174) ([axsaucedo](https://github.com/axsaucedo))
- Added destructor for staging tensors [\#134](https://github.com/EthicalML/vulkan-kompute/pull/134) ([axsaucedo](https://github.com/axsaucedo))

**Closed issues:**

- Update memory barriers to align with tensor staging/primary memory revamp [\#181](https://github.com/EthicalML/vulkan-kompute/issues/181)
- Move shader defaultResource inside kp::Shader class [\#175](https://github.com/EthicalML/vulkan-kompute/issues/175)
- Reach at least 90% code coverage on tests [\#170](https://github.com/EthicalML/vulkan-kompute/issues/170)
- Add functionality to re-record sequence as now it's possible to update the underlying algorithm [\#169](https://github.com/EthicalML/vulkan-kompute/issues/169)
- Use numpy arrays as default return value [\#166](https://github.com/EthicalML/vulkan-kompute/issues/166)
- Update all shared\_ptr value passes to be by ref or const ref [\#161](https://github.com/EthicalML/vulkan-kompute/issues/161)
- Amend memory hierarchy for kp::Operations so they can be created separately [\#160](https://github.com/EthicalML/vulkan-kompute/issues/160)
- Customise theme of documentation [\#156](https://github.com/EthicalML/vulkan-kompute/issues/156)
- Remove KomputeWorkgroup class in favour of std::array\<uint32\_t, 3\> [\#152](https://github.com/EthicalML/vulkan-kompute/issues/152)
- Passing raw GLSL string to Shader Module depricated so remove this method from supported approach [\#150](https://github.com/EthicalML/vulkan-kompute/issues/150)
- Add python backwards compatibility for eval\_tensor\_create\_def [\#147](https://github.com/EthicalML/vulkan-kompute/issues/147)
- Document breaking changes for 0.7.0 [\#140](https://github.com/EthicalML/vulkan-kompute/issues/140)
- Tensor memory management and memory hierarchy redesign [\#136](https://github.com/EthicalML/vulkan-kompute/issues/136)
- Staging tensor GPU memory is not freed as part of OpCreateTensor removal [\#133](https://github.com/EthicalML/vulkan-kompute/issues/133)
- eStorage Tensors are currently unusable as OpTensorCreate calls mapDataIntoHostMemory [\#132](https://github.com/EthicalML/vulkan-kompute/issues/132)
- 0.6.0 Release [\#126](https://github.com/EthicalML/vulkan-kompute/issues/126)
- java.lang.UnsatisfiedLinkError: dlopen failed: library "libkompute-jni.so" not found [\#125](https://github.com/EthicalML/vulkan-kompute/issues/125)
- Initial exploration: Include explicit GLSL to SPIRV compilation [\#107](https://github.com/EthicalML/vulkan-kompute/issues/107)
- Add support for push constants [\#106](https://github.com/EthicalML/vulkan-kompute/issues/106)

**Merged pull requests:**

- Resolve moving all functions from tensor HPP to CPP [\#186](https://github.com/EthicalML/vulkan-kompute/pull/186) ([axsaucedo](https://github.com/axsaucedo))
- Device Properties [\#184](https://github.com/EthicalML/vulkan-kompute/pull/184) ([alexander-g](https://github.com/alexander-g))
- Too many warnings [\#183](https://github.com/EthicalML/vulkan-kompute/pull/183) ([alexander-g](https://github.com/alexander-g))
- Add support for bool, double, int32, uint32 and float32 on Tensors via TensorT [\#177](https://github.com/EthicalML/vulkan-kompute/pull/177) ([axsaucedo](https://github.com/axsaucedo))
- Support for Timestamping [\#176](https://github.com/EthicalML/vulkan-kompute/pull/176) ([alexander-g](https://github.com/alexander-g))
- Test for ShaderResources [\#165](https://github.com/EthicalML/vulkan-kompute/pull/165) ([aliPMPAINT](https://github.com/aliPMPAINT))
- Amend memory hierarchy to enable for push constants and functional interface for more flexible operations [\#164](https://github.com/EthicalML/vulkan-kompute/pull/164) ([axsaucedo](https://github.com/axsaucedo))
- made changes for include paths for complete installation [\#163](https://github.com/EthicalML/vulkan-kompute/pull/163) ([aliPMPAINT](https://github.com/aliPMPAINT))
- Added dark mode on docs [\#157](https://github.com/EthicalML/vulkan-kompute/pull/157) ([axsaucedo](https://github.com/axsaucedo))
- Glslang implementation for online shader compilation [\#154](https://github.com/EthicalML/vulkan-kompute/pull/154) ([axsaucedo](https://github.com/axsaucedo))
- Adding test code coverage using gcov and lcov [\#149](https://github.com/EthicalML/vulkan-kompute/pull/149) ([axsaucedo](https://github.com/axsaucedo))
- Added temporary backwards compatibility for eval\_tensor\_create\_def function [\#148](https://github.com/EthicalML/vulkan-kompute/pull/148) ([axsaucedo](https://github.com/axsaucedo))
- Amend memory ownership hierarchy to have Tensor owned by Manager instead of OpCreateTensor / OpBase [\#138](https://github.com/EthicalML/vulkan-kompute/pull/138) ([axsaucedo](https://github.com/axsaucedo))
- Removed Staging Tensors in favour of having two buffer & memory in a Tensor to minimise data transfer [\#137](https://github.com/EthicalML/vulkan-kompute/pull/137) ([axsaucedo](https://github.com/axsaucedo))

## [v0.6.0](https://github.com/EthicalML/vulkan-kompute/tree/v0.6.0) (2021-01-31)

[Full Changelog](https://github.com/EthicalML/vulkan-kompute/compare/v0.5.1...v0.6.0)

**Implemented enhancements:**

- Add simple test for Python `log\_level` function [\#120](https://github.com/EthicalML/vulkan-kompute/issues/120)
- Add further numpy support [\#104](https://github.com/EthicalML/vulkan-kompute/issues/104)
- SWIG syntax error - change order of keywords. [\#94](https://github.com/EthicalML/vulkan-kompute/issues/94)
- Create mocks to isolate unit tests for components [\#8](https://github.com/EthicalML/vulkan-kompute/issues/8)
- Disallowing zero sized tensors [\#129](https://github.com/EthicalML/vulkan-kompute/pull/129) ([alexander-g](https://github.com/alexander-g))
- Added further tests to CI and provide Dockerimage with builds to swiftshader [\#119](https://github.com/EthicalML/vulkan-kompute/pull/119) ([axsaucedo](https://github.com/axsaucedo))
- Workgroups for Python [\#116](https://github.com/EthicalML/vulkan-kompute/pull/116) ([alexander-g](https://github.com/alexander-g))
- Ubuntu CI [\#115](https://github.com/EthicalML/vulkan-kompute/pull/115) ([alexander-g](https://github.com/alexander-g))
- Faster set\_data\(\) [\#109](https://github.com/EthicalML/vulkan-kompute/pull/109) ([alexander-g](https://github.com/alexander-g))
- String parameter for eval\_algo\_str methods in Python [\#105](https://github.com/EthicalML/vulkan-kompute/pull/105) ([alexander-g](https://github.com/alexander-g))
- Added numpy\(\) method [\#103](https://github.com/EthicalML/vulkan-kompute/pull/103) ([alexander-g](https://github.com/alexander-g))

**Fixed bugs:**

- \[PYTHON\] Support string parameter instead of list for eval\_algo\_data when passing raw shader as string [\#93](https://github.com/EthicalML/vulkan-kompute/issues/93)
- \[PYTHON\] Fix log\_level on the python implementation \(using pybind's logging functions\) [\#92](https://github.com/EthicalML/vulkan-kompute/issues/92)

**Closed issues:**

- Add documentation for custom operations [\#128](https://github.com/EthicalML/vulkan-kompute/issues/128)
- Numpy Array Support and Work Group Configuration in Python Kompute [\#124](https://github.com/EthicalML/vulkan-kompute/issues/124)
- Remove references to spdlog in python module [\#122](https://github.com/EthicalML/vulkan-kompute/issues/122)
- Setup automated CI testing for PRs using GitHub actions [\#114](https://github.com/EthicalML/vulkan-kompute/issues/114)
- Python example type error \(pyshader\). [\#111](https://github.com/EthicalML/vulkan-kompute/issues/111)
- Update all references to operations to not use template [\#101](https://github.com/EthicalML/vulkan-kompute/issues/101)
- Getting a undefined reference error while creating a Kompute Manager [\#100](https://github.com/EthicalML/vulkan-kompute/issues/100)

**Merged pull requests:**

- 122 remove spdlog references in python [\#123](https://github.com/EthicalML/vulkan-kompute/pull/123) ([axsaucedo](https://github.com/axsaucedo))
- Native logging for Python [\#118](https://github.com/EthicalML/vulkan-kompute/pull/118) ([alexander-g](https://github.com/alexander-g))
- Fixes for the c++ Simple and Extended examples in readme [\#108](https://github.com/EthicalML/vulkan-kompute/pull/108) ([aliPMPAINT](https://github.com/aliPMPAINT))
- Fix building shaders on native linux [\#102](https://github.com/EthicalML/vulkan-kompute/pull/102) ([aliPMPAINT](https://github.com/aliPMPAINT))

## [v0.5.1](https://github.com/EthicalML/vulkan-kompute/tree/v0.5.1) (2020-11-12)

[Full Changelog](https://github.com/EthicalML/vulkan-kompute/compare/v0.5.0...v0.5.1)

**Implemented enhancements:**

- Remove the template params from OpAlgoBase for dispatch layout [\#57](https://github.com/EthicalML/vulkan-kompute/issues/57)
- Enable layout to be configured dynamically within shaders [\#26](https://github.com/EthicalML/vulkan-kompute/issues/26)
- replaced "static unsigned const" to "static const unsigned" to avoid SWIG parsing error. [\#95](https://github.com/EthicalML/vulkan-kompute/pull/95) ([0x0f0f0f](https://github.com/0x0f0f0f))

**Closed issues:**

- Support for MoltenVK? [\#96](https://github.com/EthicalML/vulkan-kompute/issues/96)
- Update all examples to use spir-v bytes by default [\#86](https://github.com/EthicalML/vulkan-kompute/issues/86)

**Merged pull requests:**

- Python extensions for end to end example [\#97](https://github.com/EthicalML/vulkan-kompute/pull/97) ([axsaucedo](https://github.com/axsaucedo))

## [v0.5.0](https://github.com/EthicalML/vulkan-kompute/tree/v0.5.0) (2020-11-08)

[Full Changelog](https://github.com/EthicalML/vulkan-kompute/compare/v0.4.1...v0.5.0)

**Implemented enhancements:**

- Adding Python package for Kompute [\#87](https://github.com/EthicalML/vulkan-kompute/issues/87)
- Python shader extension [\#91](https://github.com/EthicalML/vulkan-kompute/pull/91) ([axsaucedo](https://github.com/axsaucedo))
- Enhanced python build [\#89](https://github.com/EthicalML/vulkan-kompute/pull/89) ([axsaucedo](https://github.com/axsaucedo))
- Added python bindings with kp as python module  [\#88](https://github.com/EthicalML/vulkan-kompute/pull/88) ([axsaucedo](https://github.com/axsaucedo))

**Closed issues:**

- Examples segfault \(Linux / mesa / amdgpu\) [\#84](https://github.com/EthicalML/vulkan-kompute/issues/84)
- Kompute support for newer Vulkan HPP headers [\#81](https://github.com/EthicalML/vulkan-kompute/issues/81)

## [v0.4.1](https://github.com/EthicalML/vulkan-kompute/tree/v0.4.1) (2020-11-01)

[Full Changelog](https://github.com/EthicalML/vulkan-kompute/compare/v0.4.0...v0.4.1)

**Implemented enhancements:**

- Upgrade build to support VulkanHPP 1.2.154 \< 1.2.158 [\#82](https://github.com/EthicalML/vulkan-kompute/issues/82)
- Add Android example for Kompute [\#23](https://github.com/EthicalML/vulkan-kompute/issues/23)
- Fix compatibility for Vulkan HPP 1.2.155 and above [\#83](https://github.com/EthicalML/vulkan-kompute/pull/83) ([axsaucedo](https://github.com/axsaucedo))
- codespell spelling fixes [\#80](https://github.com/EthicalML/vulkan-kompute/pull/80) ([pH5](https://github.com/pH5))

**Closed issues:**

- Android example throws runtime error.  [\#77](https://github.com/EthicalML/vulkan-kompute/issues/77)
- Document the utilities to convert shaders into C++ header files [\#53](https://github.com/EthicalML/vulkan-kompute/issues/53)
- Document the three types of memory ownership in classes - never, optional and always [\#31](https://github.com/EthicalML/vulkan-kompute/issues/31)

**Merged pull requests:**

- Add link to official Vulkan website to download the SDK [\#79](https://github.com/EthicalML/vulkan-kompute/pull/79) ([DonaldWhyte](https://github.com/DonaldWhyte))
- 77 Fix end to end examples by creating tensors on separate sequence [\#78](https://github.com/EthicalML/vulkan-kompute/pull/78) ([axsaucedo](https://github.com/axsaucedo))

## [v0.4.0](https://github.com/EthicalML/vulkan-kompute/tree/v0.4.0) (2020-10-18)

[Full Changelog](https://github.com/EthicalML/vulkan-kompute/compare/v0.3.2...v0.4.0)

**Implemented enhancements:**

- Error compiling on ubuntu 20.04 [\#67](https://github.com/EthicalML/vulkan-kompute/issues/67)
- Add explicit multi-threading interfaces to ensure correctness when running in parallel [\#51](https://github.com/EthicalML/vulkan-kompute/issues/51)

**Fixed bugs:**

- Ensure sequences are cleared when begin is run [\#74](https://github.com/EthicalML/vulkan-kompute/issues/74)

**Merged pull requests:**

- 74 Fixing manager default sequence creation [\#75](https://github.com/EthicalML/vulkan-kompute/pull/75) ([axsaucedo](https://github.com/axsaucedo))
- Adding Asynchronous Processing Capabilities with Multiple Queue Support [\#73](https://github.com/EthicalML/vulkan-kompute/pull/73) ([axsaucedo](https://github.com/axsaucedo))
- Fix README typo [\#71](https://github.com/EthicalML/vulkan-kompute/pull/71) ([nihui](https://github.com/nihui))

## [v0.3.2](https://github.com/EthicalML/vulkan-kompute/tree/v0.3.2) (2020-10-04)

[Full Changelog](https://github.com/EthicalML/vulkan-kompute/compare/v0.3.1...v0.3.2)

**Implemented enhancements:**

- Fix compiler errors on compilers other than msvc [\#66](https://github.com/EthicalML/vulkan-kompute/pull/66) ([Dudecake](https://github.com/Dudecake))

**Fixed bugs:**

- Fix bug in OpAlgoRhsLhs [\#61](https://github.com/EthicalML/vulkan-kompute/issues/61)

**Closed issues:**

- Change c++ to 14 from 17 for support with older frameworks [\#59](https://github.com/EthicalML/vulkan-kompute/issues/59)

**Merged pull requests:**

- Updated readme and single kompute for 0.3.2 [\#69](https://github.com/EthicalML/vulkan-kompute/pull/69) ([axsaucedo](https://github.com/axsaucedo))
- Added android example and upgraded build configurations [\#68](https://github.com/EthicalML/vulkan-kompute/pull/68) ([axsaucedo](https://github.com/axsaucedo))
- Added readme to explain high level explanation for Godot example [\#65](https://github.com/EthicalML/vulkan-kompute/pull/65) ([axsaucedo](https://github.com/axsaucedo))
- Removing vulkan dependencies in examples [\#64](https://github.com/EthicalML/vulkan-kompute/pull/64) ([axsaucedo](https://github.com/axsaucedo))
- Updated godot example to use logistic regression usecase [\#63](https://github.com/EthicalML/vulkan-kompute/pull/63) ([axsaucedo](https://github.com/axsaucedo))

## [v0.3.1](https://github.com/EthicalML/vulkan-kompute/tree/v0.3.1) (2020-09-20)

[Full Changelog](https://github.com/EthicalML/vulkan-kompute/compare/v0.3.0...v0.3.1)

**Implemented enhancements:**

- Add example of how vulkan kompute can be used for ML in Godot Game Engine [\#60](https://github.com/EthicalML/vulkan-kompute/issues/60)

**Merged pull requests:**

- Adding godot example [\#62](https://github.com/EthicalML/vulkan-kompute/pull/62) ([axsaucedo](https://github.com/axsaucedo))

## [v0.3.0](https://github.com/EthicalML/vulkan-kompute/tree/v0.3.0) (2020-09-19)

[Full Changelog](https://github.com/EthicalML/vulkan-kompute/compare/0.3.0...v0.3.0)

**Implemented enhancements:**

- Make Kompute installable locally to work with examples [\#58](https://github.com/EthicalML/vulkan-kompute/issues/58)
- Remove ability to copy output parameters from OpAlgoBase now that there's OpTensorSync [\#56](https://github.com/EthicalML/vulkan-kompute/issues/56)

## [0.3.0](https://github.com/EthicalML/vulkan-kompute/tree/0.3.0) (2020-09-13)

[Full Changelog](https://github.com/EthicalML/vulkan-kompute/compare/v0.2.0...0.3.0)

**Implemented enhancements:**

- Add tests and documentation for loops passing data to/from device [\#50](https://github.com/EthicalML/vulkan-kompute/issues/50)
- Add preSubmit function to OpBase to account for multiple eval commands in parallel [\#47](https://github.com/EthicalML/vulkan-kompute/issues/47)
- Remove vulkan commandbuffer from Tensor [\#42](https://github.com/EthicalML/vulkan-kompute/issues/42)
- Provide further granularity on handling staging tensors [\#40](https://github.com/EthicalML/vulkan-kompute/issues/40)
- Create operation to copy data from local to device memory with staging [\#39](https://github.com/EthicalML/vulkan-kompute/issues/39)
- Add more advanced ML implementations \(starting with LR, then DL, etc\) [\#19](https://github.com/EthicalML/vulkan-kompute/issues/19)

**Fixed bugs:**

- OpCreateTensor doesn't map data into GPU with OpCreateTensor for host tensors [\#43](https://github.com/EthicalML/vulkan-kompute/issues/43)

## [v0.2.0](https://github.com/EthicalML/vulkan-kompute/tree/v0.2.0) (2020-09-05)

[Full Changelog](https://github.com/EthicalML/vulkan-kompute/compare/v0.1.0...v0.2.0)

**Implemented enhancements:**

- Migrate to GTest  [\#37](https://github.com/EthicalML/vulkan-kompute/issues/37)
- Move all todos in the code into github issues [\#33](https://github.com/EthicalML/vulkan-kompute/issues/33)
- Remove spdlog as a required dependency [\#30](https://github.com/EthicalML/vulkan-kompute/issues/30)
- Improve access to tensor underlying data for speed and ease of access [\#18](https://github.com/EthicalML/vulkan-kompute/issues/18)
- Enable for compute shaders to be provided in raw form [\#17](https://github.com/EthicalML/vulkan-kompute/issues/17)
- Enable OpCreateTensor for more than 1 tensor  [\#13](https://github.com/EthicalML/vulkan-kompute/issues/13)
- Add specialisation data to algorithm with default tensor size [\#11](https://github.com/EthicalML/vulkan-kompute/issues/11)
- Add documentation with Doxygen and Sphinx [\#9](https://github.com/EthicalML/vulkan-kompute/issues/9)

**Fixed bugs:**

- Diagnose memory profiling to ensure there are no memory leaks on objects created  \[CPU\] [\#15](https://github.com/EthicalML/vulkan-kompute/issues/15)

**Merged pull requests:**

- Migrating to gtest [\#38](https://github.com/EthicalML/vulkan-kompute/pull/38) ([axsaucedo](https://github.com/axsaucedo))

## [v0.1.0](https://github.com/EthicalML/vulkan-kompute/tree/v0.1.0) (2020-08-28)

[Full Changelog](https://github.com/EthicalML/vulkan-kompute/compare/2879d3d274967e87087d567bcc659804b1707d0a...v0.1.0)



\* *This Changelog was automatically generated by [github_changelog_generator](https://github.com/github-changelog-generator/github-changelog-generator)*
