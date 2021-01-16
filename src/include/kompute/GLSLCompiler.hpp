#pragma once

#if KOMPUTE_INCLUDE_GLSLANG

#include <string>
#include <vector>

#include <glslang/Public/ShaderLang.h>

/// Adapted from Vulkan-Samples
/// A very simple version of the glslValidator application
namespace GLSLCompiler
{
  

/**
	* @brief Compiles GLSL to SPIRV code
	* @param glsl_source The GLSL source code to be compiled
	* @param entry_point The entrypoint function name of the shader stage
	*/
std::vector<char> compile_to_spirv(
	const std::string& glsl_source,
	const std::string& entry_point
);

};



#endif //KOMPUTE_INCLUDE_GLSLANG


