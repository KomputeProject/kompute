
#if !defined(KOMPUTE_DISABLE_SHADER_UTILS) || !KOMPUTE_DISABLE_SHADER_UTILS
#include "kompute/Shader.hpp"

namespace kp {

std::vector<uint32_t>
Shader::compileSources(
  const std::vector<std::string>& sources,
  const std::vector<std::string>& files,
  const std::string& entryPoint,
  std::vector<std::pair<std::string, std::string>> definitions,
  const TBuiltInResource& resources)
{

    // Initialize glslang library.
    glslang::InitializeProcess();

    // Currently we don't support other shader types nor plan to
    const EShLanguage language = EShLangCompute;
    glslang::TShader shader(language);

    std::vector<const char*> filesCStr(files.size()),
      sourcesCStr(sources.size());
    for (size_t i = 0; i < sources.size(); i++)
        sourcesCStr[i] = sources[i].c_str();

    if (files.size() > 1) {
        assert(files.size() == sources.size());
        for (size_t i = 0; i < files.size(); i++)
            filesCStr[i] = files[i].c_str();
        shader.setStringsWithLengthsAndNames(
          sourcesCStr.data(), nullptr, filesCStr.data(), filesCStr.size());
    } else {
        filesCStr = { "" };
        shader.setStringsWithLengthsAndNames(
          sourcesCStr.data(), nullptr, filesCStr.data(), sourcesCStr.size());
    }

    shader.setEntryPoint(entryPoint.c_str());
    shader.setSourceEntryPoint(entryPoint.c_str());

    std::string info_log = "";
    const EShMessages messages = static_cast<EShMessages>(
      EShMsgDefault | EShMsgVulkanRules | EShMsgSpvRules);
    if (!shader.parse(&resources, 100, false, messages)) {
        info_log = std::string(shader.getInfoLog()) + "\n" +
                   std::string(shader.getInfoDebugLog());
        KP_LOG_ERROR("Kompute Shader Error: {}", info_log);
        throw std::runtime_error(info_log);
    }

    // Add shader to new program object.
    glslang::TProgram program;
    program.addShader(&shader);
    // Link program.
    if (!program.link(messages)) {
        info_log = std::string(program.getInfoLog()) + "\n" +
                   std::string(program.getInfoDebugLog());
        KP_LOG_ERROR("Kompute Shader Error: {}", info_log);
        throw std::runtime_error(info_log);
    }

    // Save any info log that was generated.
    if (shader.getInfoLog()) {
        info_log += std::string(shader.getInfoLog()) + "\n" +
                    std::string(shader.getInfoDebugLog()) + "\n";
        KP_LOG_INFO("Kompute Shader Information: {}", info_log);
    }

    glslang::TIntermediate* intermediate = program.getIntermediate(language);
    // Translate to SPIRV.
    if (!intermediate) {
        info_log += "Failed to get shared intermediate code.\n";
        KP_LOG_ERROR("Kompute Shader Error: {}", info_log);
        throw std::runtime_error(info_log);
    }

    spv::SpvBuildLogger logger;
    std::vector<std::uint32_t> spirv;
    glslang::GlslangToSpv(*intermediate, spirv, &logger);

    if (shader.getInfoLog()) {
        info_log += logger.getAllMessages() + "\n";
        KP_LOG_DEBUG("Kompute Shader all result messages: {}", info_log);
    }

    // Shutdown glslang library.
    glslang::FinalizeProcess();

    return spirv;
}

std::vector<uint32_t>
Shader::compileSource(
  const std::string& source,
  const std::string& entryPoint,
  std::vector<std::pair<std::string, std::string>> definitions,
  const TBuiltInResource& resource)
{
    return compileSources({ source },
                          std::vector<std::string>({}),
                          entryPoint,
                          definitions,
                          resource);
}

const TBuiltInResource Shader::defaultResource = {
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

    /* .limits = */
    {
      /* .nonInductiveForLoops = */ 1,
      /* .whileLoops = */ 1,
      /* .doWhileLoops = */ 1,
      /* .generalUniformIndexing = */ 1,
      /* .generalAttributeMatrixVectorIndexing = */ 1,
      /* .generalVaryingIndexing = */ 1,
      /* .generalSamplerIndexing = */ 1,
      /* .generalVariableIndexing = */ 1,
      /* .generalConstantMatrixVectorIndexing = */ 1,
    }
};

}
#endif // DKOMPUTE_DISABLE_SHADER_UTILS
