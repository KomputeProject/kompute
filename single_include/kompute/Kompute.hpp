#pragma once

#if VK_USE_PLATFORM_ANDROID_KHR
#include <android/log.h>
#include <kompute_vk_ndk_wrapper.hpp>
// VK_NO_PROTOTYPES required before vulkan import but after wrapper.hpp
#undef VK_NO_PROTOTYPES
static const char* KOMPUTE_LOG_TAG = "KomputeLog";
#endif

#include <fmt/core.h>

#include <vulkan/vulkan.hpp>

// Typedefs to simplify interaction with core types
namespace kp {
typedef std::array<uint32_t, 3> Workgroup;
typedef std::vector<float> Constants;
}

// Must be after vulkan is included
#ifndef KOMPUTE_VK_API_VERSION
#ifndef KOMPUTE_VK_API_MAJOR_VERSION
#define KOMPUTE_VK_API_MAJOR_VERSION 1
#endif // KOMPUTE_VK_API_MAJOR_VERSION
#ifndef KOMPUTE_VK_API_MINOR_VERSION
#define KOMPUTE_VK_API_MINOR_VERSION 1
#endif // KOMPUTE_VK_API_MINOR_VERSION
#define KOMPUTE_VK_API_VERSION                                                 \
    VK_MAKE_VERSION(                                                           \
      KOMPUTE_VK_API_MAJOR_VERSION, KOMPUTE_VK_API_MINOR_VERSION, 0)
#endif // KOMPUTE_VK_API_VERSION

// SPDLOG_ACTIVE_LEVEL must be defined before spdlog.h import
#ifndef SPDLOG_ACTIVE_LEVEL
#if DEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#else
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#endif
#endif

#if defined(KOMPUTE_BUILD_PYTHON)
#include <pybind11/pybind11.h>
namespace py = pybind11;
// from python/src/main.cpp
extern py::object kp_debug, kp_info, kp_warning, kp_error;
#endif

#ifndef KOMPUTE_LOG_OVERRIDE
#if KOMPUTE_ENABLE_SPDLOG
#include <spdlog/spdlog.h>
#define KP_LOG_DEBUG(...) SPDLOG_DEBUG(__VA_ARGS__)
#define KP_LOG_INFO(...) SPDLOG_INFO(__VA_ARGS__)
#define KP_LOG_WARN(...) SPDLOG_WARN(__VA_ARGS__)
#define KP_LOG_ERROR(...) SPDLOG_ERROR(__VA_ARGS__)
#else
#include <iostream>
#if SPDLOG_ACTIVE_LEVEL > 1
#define KP_LOG_DEBUG(...)
#else
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
#define KP_LOG_DEBUG(...)                                             \
    ((void)__android_log_print(ANDROID_LOG_DEBUG, KOMPUTE_LOG_TAG, fmt::format(__VA_ARGS__)))
#elif defined(KOMPUTE_BUILD_PYTHON)
#define KP_LOG_DEBUG(...) kp_debug(fmt::format(__VA_ARGS__))
#else
#define KP_LOG_DEBUG(...) fmt::print("[{} {}] [debug] [{}:{}] {}\n", __DATE__, __TIME__, __FILE__, __LINE__, fmt::format(__VA_ARGS__))
#endif // VK_USE_PLATFORM_ANDROID_KHR
#endif // SPDLOG_ACTIVE_LEVEL > 1

#if SPDLOG_ACTIVE_LEVEL > 2
#define KP_LOG_INFO(...)
#else
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
#define KP_LOG_INFO(...)                                              \
    ((void)__android_log_print(ANDROID_LOG_INFO, KOMPUTE_LOG_TAG, fmt::format(__VA_ARGS__)))
#elif defined(KOMPUTE_BUILD_PYTHON)
#define KP_LOG_INFO(...) kp_info(fmt::format(__VA_ARGS__))
#else
#define KP_LOG_INFO(...) fmt::print("[{} {}] [debug] [{}:{}] {}\n", __DATE__, __TIME__, __FILE__, __LINE__, fmt::format(__VA_ARGS__))
#endif // VK_USE_PLATFORM_ANDROID_KHR
#endif // SPDLOG_ACTIVE_LEVEL > 2

#if SPDLOG_ACTIVE_LEVEL > 3
#define KP_LOG_WARN(...)
#else
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
#define KP_LOG_WARN(...)                                              \
    ((void)__android_log_print(ANDROID_LOG_WARN, KOMPUTE_LOG_TAG, fmt::format(__VA_ARGS__)))
#elif defined(KOMPUTE_BUILD_PYTHON)
#define KP_LOG_WARN(...) kp_warning(fmt::format(__VA_ARGS__))
#else
#define KP_LOG_WARN(...) fmt::print("[{} {}] [debug] [{}:{}] {}\n", __DATE__, __TIME__, __FILE__, __LINE__, fmt::format(__VA_ARGS__))
#endif // VK_USE_PLATFORM_ANDROID_KHR
#endif // SPDLOG_ACTIVE_LEVEL > 3

#if SPDLOG_ACTIVE_LEVEL > 4
#define KP_LOG_ERROR(...)
#else
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
#define KP_LOG_ERROR(...)                                             \
    ((void)__android_log_print(ANDROID_LOG_ERROR, KOMPUTE_LOG_TAG, fmt::format(__VA_ARGS__)))
#elif defined(KOMPUTE_BUILD_PYTHON)
#define KP_LOG_ERROR(...) kp_error(fmt::format(__VA_ARGS__))
#else
#define KP_LOG_ERROR(...) fmt::print("[{} {}] [debug] [{}:{}] {}\n", __DATE__, __TIME__, __FILE__, __LINE__, fmt::format(__VA_ARGS__))
#endif // VK_USE_PLATFORM_ANDROID_KHR
#endif // SPDLOG_ACTIVE_LEVEL > 4
#endif // KOMPUTE_SPDLOG_ENABLED
#endif // KOMPUTE_LOG_OVERRIDE

#if !defined(KOMPUTE_DISABLE_SHADER_UTILS) || !KOMPUTE_DISABLE_SHADER_UTILS
#include <iostream>
#include <vector>

#include <glslang/Include/ResourceLimits.h>
#include <glslang/Public/ShaderLang.h>
#include <SPIRV/GlslangToSpv.h>

namespace kp {

// The default resource limit for the GLSL compiler, can be overwritten
// Has been adobted by:
// https://github.com/KhronosGroup/glslang/blob/master/StandAlone/ResourceLimits.cpp
const TBuiltInResource defaultResource = {
/* .MaxLights = */ 0,
/* .MaxClipPlanes = */ 0,
/* .MaxTextureUnits = */ 0,
/* .MaxTextureCoords = */ 0,
/* .MaxVertexAttribs = */ 64,
/* .MaxVertexUniformComponents = */ 4096,
/* .MaxVaryingFloats = */ 64,
/* .MaxVertexTextureImageUnits = */ 0,
/* .MaxCombinedTextureImageUnits = */ 0,
/* .MaxTextureImageUnits = */ 0,
/* .MaxFragmentUniformComponents = */ 0,
/* .MaxDrawBuffers = */ 0,
/* .MaxVertexUniformVectors = */ 128,
/* .MaxVaryingVectors = */ 8,
/* .MaxFragmentUniformVectors = */ 0,
/* .MaxVertexOutputVectors = */ 16,
/* .MaxFragmentInputVectors = */ 0,
/* .MinProgramTexelOffset = */ -8,
/* .MaxProgramTexelOffset = */ 7,
/* .MaxClipDistances = */ 8,
/* .MaxComputeWorkGroupCountX = */ 65535,
/* .MaxComputeWorkGroupCountY = */ 65535,
/* .MaxComputeWorkGroupCountZ = */ 65535,
/* .MaxComputeWorkGroupSizeX = */ 1024,
/* .MaxComputeWorkGroupSizeY = */ 1024,
/* .MaxComputeWorkGroupSizeZ = */ 64,
/* .MaxComputeUniformComponents = */ 1024,
/* .MaxComputeTextureImageUnits = */ 16,
/* .MaxComputeImageUniforms = */ 8,
/* .MaxComputeAtomicCounters = */ 8,
/* .MaxComputeAtomicCounterBuffers = */ 1,
/* .MaxVaryingComponents = */ 60,
/* .MaxVertexOutputComponents = */ 64,
/* .MaxGeometryInputComponents = */ 64,
/* .MaxGeometryOutputComponents = */ 128,
/* .MaxFragmentInputComponents = */ 0,
/* .MaxImageUnits = */ 0,
/* .MaxCombinedImageUnitsAndFragmentOutputs = */ 0,
/* .MaxCombinedShaderOutputResources = */ 8,
/* .MaxImageSamples = */ 0,
/* .MaxVertexImageUniforms = */ 0,
/* .MaxTessControlImageUniforms = */ 0,
/* .MaxTessEvaluationImageUniforms = */ 0,
/* .MaxGeometryImageUniforms = */ 0,
/* .MaxFragmentImageUniforms = */ 0,
/* .MaxCombinedImageUniforms = */ 0,
/* .MaxGeometryTextureImageUnits = */ 0,
/* .MaxGeometryOutputVertices = */ 256,
/* .MaxGeometryTotalOutputComponents = */ 1024,
/* .MaxGeometryUniformComponents = */ 1024,
/* .MaxGeometryVaryingComponents = */ 64,
/* .MaxTessControlInputComponents = */ 128,
/* .MaxTessControlOutputComponents = */ 128,
/* .MaxTessControlTextureImageUnits = */ 0,
/* .MaxTessControlUniformComponents = */ 1024,
/* .MaxTessControlTotalOutputComponents = */ 4096,
/* .MaxTessEvaluationInputComponents = */ 128,
/* .MaxTessEvaluationOutputComponents = */ 128,
/* .MaxTessEvaluationTextureImageUnits = */ 16,
/* .MaxTessEvaluationUniformComponents = */ 1024,
/* .MaxTessPatchComponents = */ 120,
/* .MaxPatchVertices = */ 32,
/* .MaxTessGenLevel = */ 64,
/* .MaxViewports = */ 16,
/* .MaxVertexAtomicCounters = */ 0,
/* .MaxTessControlAtomicCounters = */ 0,
/* .MaxTessEvaluationAtomicCounters = */ 0,
/* .MaxGeometryAtomicCounters = */ 0,
/* .MaxFragmentAtomicCounters = */ 0,
/* .MaxCombinedAtomicCounters = */ 8,
/* .MaxAtomicCounterBindings = */ 1,
/* .MaxVertexAtomicCounterBuffers = */ 0,
/* .MaxTessControlAtomicCounterBuffers = */ 0,
/* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
/* .MaxGeometryAtomicCounterBuffers = */ 0,
/* .MaxFragmentAtomicCounterBuffers = */ 0,
/* .MaxCombinedAtomicCounterBuffers = */ 1,
/* .MaxAtomicCounterBufferSize = */ 16384,
/* .MaxTransformFeedbackBuffers = */ 4,
/* .MaxTransformFeedbackInterleavedComponents = */ 64,
/* .MaxCullDistances = */ 8,
/* .MaxCombinedClipAndCullDistances = */ 8,
/* .MaxSamples = */ 4,
/* .maxMeshOutputVerticesNV = */ 256,
/* .maxMeshOutputPrimitivesNV = */ 512,
/* .maxMeshWorkGroupSizeX_NV = */ 32,
/* .maxMeshWorkGroupSizeY_NV = */ 1,
/* .maxMeshWorkGroupSizeZ_NV = */ 1,
/* .maxTaskWorkGroupSizeX_NV = */ 32,
/* .maxTaskWorkGroupSizeY_NV = */ 1,
/* .maxTaskWorkGroupSizeZ_NV = */ 1,
/* .maxMeshViewCountNV = */ 4,
/* .maxDualSourceDrawBuffersEXT = */ 1,

/* .limits = */ {
    /* .nonInductiveForLoops = */ 1,
    /* .whileLoops = */ 1,
    /* .doWhileLoops = */ 1,
    /* .generalUniformIndexing = */ 1,
    /* .generalAttributeMatrixVectorIndexing = */ 1,
    /* .generalVaryingIndexing = */ 1,
    /* .generalSamplerIndexing = */ 1,
    /* .generalVariableIndexing = */ 1,
    /* .generalConstantMatrixVectorIndexing = */ 1,
}};
    
/**
    Shader utily class with functions to compile and process glsl files.
*/
class Shader {
public:
    /**
     * Compile multiple sources with optional filenames. Currently this function
     * uses the glslang C++ interface which is not thread safe so this funciton
     * should not be called from multiple threads concurrently. If you have a
     * online shader processing multithreading use-case that can't use offline 
     * compilation please open an issue.
     *
     * @param sources A list of raw glsl shaders in string format
     * @param files A list of file names respective to each of the sources
     * @param entryPoint The function name to use as entry point
     * @param definitions List of pairs containing key value definitions
     * @param resourcesLimit A list that contains the resource limits for the GLSL compiler
     * @return The compiled SPIR-V binary in unsigned int32 format
     */
    static std::vector<uint32_t> compile_sources(
            const std::vector<std::string>& sources,
            const std::vector<std::string>& files = {},
            const std::string& entryPoint = "main",
            std::vector<std::pair<std::string,std::string>> definitions = {},
            const TBuiltInResource& resources = defaultResource);

    /**
     * Compile a single glslang source from string value. Currently this function
     * uses the glslang C++ interface which is not thread safe so this funciton
     * should not be called from multiple threads concurrently. If you have a
     * online shader processing multithreading use-case that can't use offline 
     * compilation please open an issue.
     *
     * @param source An individual raw glsl shader in string format
     * @param entryPoint The function name to use as entry point
     * @param definitions List of pairs containing key value definitions
     * @param resourcesLimit A list that contains the resource limits for the GLSL compiler
     * @return The compiled SPIR-V binary in unsigned int32 format
     */
    static std::vector<uint32_t> compile_source(
            const std::string& source,
            const std::string& entryPoint = "main",
            std::vector<std::pair<std::string,std::string>> definitions = {},
            const TBuiltInResource& resources = defaultResource);

};

}
#endif // DKOMPUTE_DISABLE_SHADER_UTILS

/*
    THIS FILE HAS BEEN AUTOMATICALLY GENERATED - DO NOT EDIT

    ---

    Copyright 2020 The Institute for Ethical AI & Machine Learning

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#ifndef SHADEROP_SHADEROPMULT_HPP
#define SHADEROP_SHADEROPMULT_HPP

namespace kp {
namespace shader_data {
static const unsigned char shaders_glsl_opmult_comp_spv[] = {
  0x03, 0x02, 0x23, 0x07, 0x00, 0x00, 0x01, 0x00, 0x0a, 0x00, 0x08, 0x00,
  0x2e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x02, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x06, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x47, 0x4c, 0x53, 0x4c, 0x2e, 0x73, 0x74, 0x64, 0x2e, 0x34, 0x35, 0x30,
  0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x06, 0x00, 0x05, 0x00, 0x00, 0x00,
  0x04, 0x00, 0x00, 0x00, 0x6d, 0x61, 0x69, 0x6e, 0x00, 0x00, 0x00, 0x00,
  0x0b, 0x00, 0x00, 0x00, 0x10, 0x00, 0x06, 0x00, 0x04, 0x00, 0x00, 0x00,
  0x11, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 0x02, 0x00, 0x00, 0x00,
  0xc2, 0x01, 0x00, 0x00, 0x05, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00,
  0x6d, 0x61, 0x69, 0x6e, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x04, 0x00,
  0x08, 0x00, 0x00, 0x00, 0x69,