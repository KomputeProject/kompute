// Defining OPMULT_H to ensure cpp class doesn't reimport
#define OPMULT_H
#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>

// SPDLOG_ACTIVE_LEVEL must be defined before spdlog.h import
#if DEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#endif

#include <spdlog/spdlog.h>

#include "Algorithm.hpp"
#include "Tensor.hpp"

#include "OpBase.hpp"

namespace kp {

template<uint32_t tX = 0, uint32_t tY = 0, uint32_t tZ = 0>
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

    uint32_t mX;
    uint32_t mY;
    uint32_t mZ;
};

} // End namespace kp

// Including implemenation for template class
#include "OpMult.tpp"

