#pragma once

#include <string>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>

// SPDLOG_ACTIVE_LEVEL must be defined before spdlog.h import
#if DEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#endif

#include <spdlog/spdlog.h>

namespace kp {

template<class T>
class BaseOp
{
  private:
  public:
    BaseOp() {}

    BaseOp(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
           std::shared_ptr<vk::Device> device,
           std::shared_ptr<vk::CommandBuffer> commandBuffer) {
        SPDLOG_DEBUG("Compute BaseOp constructor started");

        this->mPhysicalDevice = physicalDevice;
        this->mDevice = device;
        this->mCommandBuffer = commandBuffer;
    }

    ~BaseOp()
    {
        SPDLOG_DEBUG("Compute BaseOp destructor started");
    }

    template<typename... TArgs>
    void init(TArgs&&... args)
    {
        SPDLOG_DEBUG("Compute BaseOp init started");
        static_cast<T*>(this)->init(std::forward<TArgs>(args)...);
    }

    template<typename... TArgs>
    void record(TArgs&&... args)
    {
        SPDLOG_DEBUG("Compute BaseOp record started");
        static_cast<T*>(this)->record(std::forward<TArgs>(args)...);
    }

  protected:
    std::shared_ptr<vk::PhysicalDevice> mPhysicalDevice;
    std::shared_ptr<vk::Device> mDevice;
    std::shared_ptr<vk::CommandBuffer> mCommandBuffer;
};

} // End namespace kp

