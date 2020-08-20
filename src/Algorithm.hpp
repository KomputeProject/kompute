#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>

// SPDLOG_ACTIVE_LEVEL must be defined before spdlog.h import
#if DEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#endif

#include <spdlog/spdlog.h>

#include "Tensor.hpp"

namespace kp {

class Algorithm
{
  public:
    Algorithm();

    Algorithm(std::shared_ptr<vk::Device> device);

    // TODO: Add specialisation data
    void init(std::string shaderFilePath,
              std::vector<std::shared_ptr<Tensor>> tensorParams);

    ~Algorithm();
};

} // End namespace kp
