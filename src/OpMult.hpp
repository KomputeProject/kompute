#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>

// SPDLOG_ACTIVE_LEVEL must be defined before spdlog.h import
#if DEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#endif

#include <spdlog/spdlog.h>

#include "Tensor.hpp"
#include "Algorithm.hpp"

#include "OpBase.hpp"

namespace kp {

class OpMult : public OpBase
{
  public:
    OpMult();

    OpMult(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
           std::shared_ptr<vk::Device> device,
           std::shared_ptr<vk::CommandBuffer> commandBuffer);

    ~OpMult();

    void init(std::vector<std::shared_ptr<Tensor>> tensors) override;

    void record() override;

    void postSubmit() override;

  private:
    std::shared_ptr<Algorithm> mAlgorithm;
    std::shared_ptr<Tensor> mTensorLHS;
    std::shared_ptr<Tensor> mTensorRHS;
    std::shared_ptr<Tensor> mTensorOutput;
    std::shared_ptr<Tensor> mTensorOutputStaging;
};

} // End namespace kp
