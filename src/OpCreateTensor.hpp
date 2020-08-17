#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>

// SPDLOG_ACTIVE_LEVEL must be defined before spdlog.h import
#if DEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#endif

#include <spdlog/spdlog.h>

#include "BaseOp.hpp"

namespace kp {

class OpCreateTensor : BaseOp
{
  private:
  public:
    OpCreateTensor();

    OpCreateTensor(std::shared_ptr<vk::CommandBuffer> commandBuffer);

    ~OpCreateTensor();

  private:
    std::shared_ptr<vk::CommandBuffer> mCommandBuffer;
};

} // End namespace kp
