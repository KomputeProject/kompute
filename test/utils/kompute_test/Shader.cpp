
#include "kompute/Kompute.hpp"

#include "kompute_test/Shader.hpp"

namespace kp_test_utils {

std::vector<uint32_t>
Shader::compileSource(
  const std::string& source)
{
    std::string cmd = "glslc -fshader-stage=compute -o tmp_kp_shader.comp.spv - << END\n" + source + "\nEND";
    system(cmd.c_str());

    std::ifstream fileStream("tmp_kp_shader.comp.spv",
                     std::ios::binary | std::ios::in | std::ios::ate);

    size_t shaderFileSize = fileStream.tellg();
    fileStream.seekg(0, std::ios::beg);
    char* shaderDataRaw = new char[shaderFileSize];
    fileStream.read(shaderDataRaw, shaderFileSize);
    fileStream.close();
    std::vector<uint32_t> tmpResult = {(uint32_t*)shaderDataRaw, (uint32_t*)(shaderDataRaw + shaderFileSize)};
    return tmpResult;
}

}
