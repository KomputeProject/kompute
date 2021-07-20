#pragma once

#include <iostream>
#include <vector>

namespace kp_test_utils {

/**
    Shader utily class with functions to compile and process glsl files.
*/
class Shader
{
  public:
    /**
     * Compile a single glslang source from string value. This is only meant
     * to be used for testing as it's non threadsafe, and it had to be removed
     * from the glslang dependency and now can only run the CLI directly due to 
     * license issues: see https://github.com/EthicalML/vulkan-kompute/pull/235
     *
     * @param source An individual raw glsl shader in string format
     * @return The compiled SPIR-V binary in unsigned int32 format
     */
    static std::vector<uint32_t> compileSource(
      const std::string& source);
};

}
