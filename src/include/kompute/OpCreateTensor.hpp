#pragma once

#include "kompute/Core.hpp"

#include "kompute/Tensor.hpp"

#include "kompute/OpBase.hpp"

namespace kp {

class OpCreateTensor : public OpBase
{
  public:
    OpCreateTensor();

    OpCreateTensor(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
                   std::shared_ptr<vk::Device> device,
                   std::shared_ptr<vk::CommandBuffer> commandBuffer,
                   std::vector<std::shared_ptr<Tensor>>& tensors,
                   bool freeTensors = true);

    ~OpCreateTensor();

    void init() override;

    void record() override;

    void postSubmit() override;

  private:
    // Never owned resources
    std::shared_ptr<Tensor> mPrimaryTensor;
    std::shared_ptr<Tensor> mStagingTensor;
};

} // End namespace kp
