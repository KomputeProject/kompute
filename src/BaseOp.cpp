
// SPDLOG_ACTIVE_LEVEL must be defined before spdlog.h import
#if DEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#endif

#include <spdlog/spdlog.h>

#include "BaseOp.hpp"

namespace kp {

template<class T>
BaseOp<T>::BaseOp()
{}

template<class T>
BaseOp<T>::BaseOp(std::shared_ptr<vk::PhysicalDevice> physicalDevice, 
                  std::shared_ptr<vk::Device> device,
                  std::shared_ptr<vk::CommandBuffer> commandBuffer)
{
    SPDLOG_DEBUG("Compute BaseOp constructor started");

    this->mPhysicalDevice = physicalDevice;
    this->mDevice = device;
    this->mCommandBuffer = commandBuffer;
}

template<class T>
BaseOp<T>::~BaseOp()
{
    SPDLOG_DEBUG("Compute BaseOp destructor started");
}

}
