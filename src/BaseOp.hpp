#pragma once

#include <string>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>

namespace kp {

class BaseOp
{
  private:
  public:
    BaseOp();
    BaseOp(std::shared_ptr<vk::CommandBuffer> commandBuffer);
    virtual ~BaseOp();

    void init(std::string one, std::string two);
    void record();

  private:
    std::shared_ptr<vk::Device> mDevice;
    std::shared_ptr<vk::CommandBuffer> mCommandBuffer;
};

} // End namespace kp
