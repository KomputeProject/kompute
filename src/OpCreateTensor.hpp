#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>

// SPDLOG_ACTIVE_LEVEL must be defined before spdlog.h import
#if DEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#endif

#include <spdlog/spdlog.h>

#include "Tensor.hpp"

#include "OpBase.hpp"

namespace kp {

class OpCreateTensor : public OpBase
{
  public:
    OpCreateTensor();

    OpCreateTensor(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
                   std::shared_ptr<vk::Device> device,
                   std::shared_ptr<vk::CommandBuffer> commandBuffer);

    ~OpCreateTensor();

    void init(std::vector<std::shared_ptr<Tensor>> tensors) override;

    void record() override;

  private:
    std::shared_ptr<Tensor> mPrimaryTensor;
    std::shared_ptr<Tensor> mStagingTensor;
};

} // End namespace kp
