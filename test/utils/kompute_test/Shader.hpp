// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <fstream>
#include <iostream>
#include <vector>

/**
 * Compile a single glslang source from string value. This is only meant
 * to be used for testing as it's non threadsafe, and it had to be removed
 * from the glslang dependency and now can only run the CLI directly due to
 * license issues: see https://github.com/KomputeProject/kompute/pull/235
 *
 * @param source An individual raw glsl shader in string format
 * @return The compiled SPIR-V binary in unsigned int32 format
 */
static std::vector<uint32_t>
compileSource(const std::string& source)
{
    std::ofstream fileOut("tmp_kp_shader.comp");
    fileOut << source;
    fileOut.close();
    if (system(
          std::string(
            "glslangValidator -V tmp_kp_shader.comp -o tmp_kp_shader.comp.spv")
            .c_str()))
        throw std::runtime_error("Error running glslangValidator command");
    std::ifstream fileStream("tmp_kp_shader.comp.spv", std::ios::binary);
    std::vector<char> buffer;
    buffer.insert(
      buffer.begin(), std::istreambuf_iterator<char>(fileStream), {});
    return { (uint32_t*)buffer.data(),
             (uint32_t*)(buffer.data() + buffer.size()) };
}
