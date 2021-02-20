#pragma once

#include <iostream>
#include <vector>

#include "Core.hpp"

#include <glslang/Public/ShaderLang.h>
#include <StandAlone/ResourceLimits.h>
#include <SPIRV/GlslangToSpv.h>

namespace kp {

class Shader {
public:
    static std::vector<uint32_t> compile_sources(
            const std::vector<std::string>& sources,
            const std::vector<std::string>& files = {},
            const std::string& entryPoint = "main",
            std::vector<std::pair<std::string,std::string>> definitions = {});

    static std::vector<uint32_t> compile_source(
            const std::string& source,
            const std::string& entryPoint = "main",
            std::vector<std::pair<std::string,std::string>> definitions = {});

};
}
