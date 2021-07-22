// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <iostream>
#include <vector>
#include <fstream>

/**
 * Compile a single glslang source from string value. This is only meant
 * to be used for testing as it's non threadsafe, and it had to be removed
 * from the glslang dependency and now can only run the CLI directly due to 
 * license issues: see https://github.com/KomputeProject/kompute/pull/235
 *
 * @param source An individual raw glsl shader in string format
 * @return The compiled SPIR-V binary in unsigned int32 format
 */
static
std::vector<uint32_t>
compileSource(
  const std::string& source)
{
    if (system(std::string("glslangValidator --stdin -S comp -V -o tmp_kp_shader.comp.spv << END\n" + source + "\nEND").c_str()))
        throw std::runtime_error("Error running glslangValidator command");
    std::ifstream fileStream("tmp_kp_shader.comp.spv", std::ios::binary);
    std::vector<char> buffer;
    buffer.insert(buffer.begin(), std::istreambuf_iterator<char>(fileStream), {});
    return {(uint32_t*)buffer.data(), (uint32_t*)(buffer.data() + buffer.size())};
}
