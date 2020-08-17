#pragma once

#include <string>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>

namespace kp {

class BaseOperator
{
private:


public:
    BaseOperator();
    BaseOperator(std::shared_ptr<vk::CommandBuffer> commandBuffer);
    virtual ~BaseOperator();

    void init(std::string one, std::string two);
    void record();

private:
    std::shared_ptr<vk::Device> mDevice;
    std::shared_ptr<vk::CommandBuffer> mCommandBuffer;

};

} // End namespace kp

