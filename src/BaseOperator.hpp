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
    BaseOperator(vk::CommandBuffer* commandBuffer);
    virtual ~BaseOperator();

    void init(std::string one, std::string two);
    void record();

private:
    vk::Device* mDevice;
    vk::CommandBuffer* mCommandBuffer;

};

} // End namespace kp

