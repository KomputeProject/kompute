#pragma once

#if !defined(KOMPUTE_DISABLE_SHADER_UTILS) || !KOMPUTE_DISABLE_SHADER_UTILS
#include <iostream>
#include <vector>

#include <glslang/Public/ShaderLang.h>
#include <StandAlone/ResourceLimits.h>
#include <SPIRV/GlslangToSpv.h>

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
     * @return The compiled SPIR-V binary in unsigned int32 format
     */
    static std::vector<uint32_t> compile_sources(
            const std::vector<std::string>& sources,
            const std::vector<std::string>& files = {},
            const std::string& entryPoint = "main",
            std::vector<std::pair<std::string,std::string>> definitions = {});

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
     * @return The compiled SPIR-V binary in unsigned int32 format
     */
    static std::vector<uint32_t> compile_source(
            const std::string& source,
            const std::string& entryPoint = "main",
            std::vector<std::pair<std::string,std::string>> definitions = {});

};
}
#endif // DKOMPUTE_DISABLE_SHADER_UTILS

