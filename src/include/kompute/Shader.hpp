#pragma once

#if !defined(KOMPUTE_DISABLE_SHADER_UTILS) || !KOMPUTE_DISABLE_SHADER_UTILS
#include <iostream>
#include <vector>

#include <glslang/Include/ResourceLimits.h>
#include <glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>

#include "kompute/Core.hpp"

namespace kp {

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
            const TBuiltInResource resources = defaultResource);

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
            const TBuiltInResource resources = defaultResource);

private:
    // The default resource limit for the GLSL compiler, can be overwritten
    static constexpr TBuiltInResource defaultResource = {
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
};
}
#endif // DKOMPUTE_DISABLE_SHADER_UTILS

