#pragma once

#if !defined(KOMPUTE_DISABLE_SHADER_UTILS) || !KOMPUTE_DISABLE_SHADER_UTILS
#include <iostream>
#include <vector>

#include <SPIRV/GlslangToSpv.h>
#include <glslang/Include/ResourceLimits.h>
#include <glslang/Public/ShaderLang.h>

#include "kompute/Core.hpp"

namespace kp {

/**
    Shader utily class with functions to compile and process glsl files.
*/
class Shader
{
  public:
    // The default resource limit for the GLSL compiler, can be overwritten
    // Has been adopted by:
    // https://github.com/KhronosGroup/glslang/blob/master/StandAlone/ResourceLimits.cpp
    const static TBuiltInResource defaultResource;

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
     * @param resourcesLimit A list that contains the resource limits for the
     * GLSL compiler
     * @return The compiled SPIR-V binary in unsigned int32 format
     */
    static std::vector<uint32_t> compileSources(
      const std::vector<std::string>& sources,
      const std::vector<std::string>& files = {},
      const std::string& entryPoint = "main",
      std::vector<std::pair<std::string, std::string>> definitions = {},
      const TBuiltInResource& resources = Shader::defaultResource);

    /**
     * Compile a single glslang source from string value. Currently this
     * function uses the glslang C++ interface which is not thread safe so this
     * funciton should not be called from multiple threads concurrently. If you
     * have a online shader processing multithreading use-case that can't use
     * offline compilation please open an issue.
     *
     * @param source An individual raw glsl shader in string format
     * @param entryPoint The function name to use as entry point
     * @param definitions List of pairs containing key value definitions
     * @param resourcesLimit A list that contains the resource limits for the
     * GLSL compiler
     * @return The compiled SPIR-V binary in unsigned int32 format
     */
    static std::vector<uint32_t> compileSource(
      const std::string& source,
      const std::string& entryPoint = "main",
      std::vector<std::pair<std::string, std::string>> definitions = {},
      const TBuiltInResource& resources = Shader::defaultResource);
};

}
#endif // DKOMPUTE_DISABLE_SHADER_UTILS
