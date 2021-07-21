
#include "kompute/Kompute.hpp"

#include "kompute_test/Shader.hpp"

namespace kp_test_utils {

std::vector<uint32_t>
Shader::compileSource(
  const std::string& source)
{
    system(std::string("glslc -fshader-stage=compute -o tmp_kp_shader.comp.spv - << END\n" + source + "\nEND").c_str());
    std::ifstream fileStream("tmp_kp_shader.comp.spv", std::ios::binary);
    std::vector<char> buffer;
    buffer.insert(buffer.begin(), std::istreambuf_iterator<char>(fileStream), {});
    return {(uint32_t*)buffer.data(), (uint32_t*)(buffer.data() + buffer.size())};
}

}
