#pragma once

#include <string>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>

// SPDLOG_ACTIVE_LEVEL must be defined before spdlog.h import
#if DEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#endif

#include <spdlog/spdlog.h>

#include "Tensor.hpp"

namespace kp {

class OpBase
{
  private:
  public:
    OpBase() {
        SPDLOG_DEBUG("Compute OpBase base constructor");
    }

    OpBase(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
           std::shared_ptr<vk::Device> device,
           std::shared_ptr<vk::CommandBuffer> commandBuffer)
    {
        SPDLOG_DEBUG("Compute OpBase constructor with params");

        this->mPhysicalDevice = physicalDevice;
        this->mDevice = device;
        this->mCommandBuffer = commandBuffer;
    }

    ~OpBase() { SPDLOG_DEBUG("Compute OpBase destructor started"); }

    virtual void init(std::vector<std::shared_ptr<Tensor>> tensors)
    {
        SPDLOG_DEBUG("Kompute OpBase init called");
    }

    virtual void record() { SPDLOG_DEBUG("Kompute OpBase record called"); }

    virtual void postSubmit()
    {
        SPDLOG_DEBUG("Kompute OpBase init called");
    }


  protected:
    std::shared_ptr<vk::PhysicalDevice> mPhysicalDevice;
    std::shared_ptr<vk::Device> mDevice;
    std::shared_ptr<vk::CommandBuffer> mCommandBuffer;
};

} // End namespace kp
