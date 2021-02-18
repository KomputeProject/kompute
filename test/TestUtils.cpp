
#include <iostream>
#include <vector>

#include <shaderc/shaderc.hpp>

static std::vector<char> spirv_from_string(const std::string& source,
                                   shaderc_optimization_level optimization = shaderc_optimization_level_size,
                                   std::vector<std::pair<std::string,std::string>> definitions = {}) {
  shaderc::Compiler compiler;
  shaderc::CompileOptions options;

  for (const std::pair<std::string,std::string>& def : definitions) {
      options.AddMacroDefinition(def.first, def.second);
  }
  if (optimization) options.SetOptimizationLevel(optimization);

  std::string errorTag = "kompute";
  shaderc::SpvCompilationResult module =
      compiler.CompileGlslToSpv(source, shaderc_glsl_compute_shader, errorTag.c_str(), options);

  if (module.GetCompilationStatus() != shaderc_compilation_status_success)  {
    throw std::runtime_error("Shader string invalid: " + module.GetErrorMessage());
  }

  return {module.cbegin(), module.cend()};
}
