
#include <string>

#include "spdlog/spdlog.h"

#include "Sequence.hpp"

namespace kp {

Sequence::Sequence() 
{
    // TODO: Create device, queue, etc
}

Sequence::Sequence(vk::Device* device, vk::Queue* computeQueue, uint32_t queueIndex)
{
    SPDLOG_DEBUG("Kompute Sequence Created with existing device & queue");
    this->mDevice = device;
    this->mComputeQueue = computeQueue;
    this->mQueueIndex = queueIndex;
}

Sequence::~Sequence() {
    SPDLOG_DEBUG("Kompute Sequence Destructor started");

    if (this->mDevice == nullptr) {
        spdlog::error("Kompute Sequence destructor reached with null Device pointer");
        return;
    }

    if (this->mCommandBuffer == nullptr) {
        spdlog::error("Kompute Sequence destructor reached with null CommandPool pointer");
        return;
    }

    if (this->mFreeCommandBuffer) {
        spdlog::info("Freeing CommandBuffer");
        this->mDevice->freeCommandBuffers(*this->mCommandPool, 1, this->mCommandBuffer);
        SPDLOG_DEBUG("Kompute Manager Freed CommandBuffer");
    }

    if (this->mCommandPool == nullptr) {
        spdlog::error("Kompute Sequence destructor reached with null CommandPool pointer");
        return;
    }

    if (this->mFreeCommandPool) {
        spdlog::info("Destroying CommandPool");
        this->mDevice->destroy(*this->mCommandPool);
        SPDLOG_DEBUG("Kompute Manager Destroyed CommandPool");
    }

}

void Sequence::begin() {
    if (this->mCommandPool == nullptr) {
        throw std::runtime_error("Kompute Sequence command pool is null");
    }

    if (!this->mRecording) {
        spdlog::info("Kompute Sequence starting command recording");
        this->mCommandBuffer->begin(vk::CommandBufferBeginInfo());
        this->mRecording = true;
    }
    else {
        spdlog::warn("Kompute Sequence attempted to start command recording but recording already started");
    }
}

void Sequence::end() { 
    if (this->mCommandPool == nullptr) {
        throw std::runtime_error("Kompute Sequence command pool is null");
    }

    if (this->mRecording) {
        spdlog::info("Kompute Sequence ending command recording");
        this->mCommandBuffer->end();
        this->mRecording = false;
    }
    else {
        spdlog::warn("Kompute Sequence attempted to end command recording but recording not started");
    }
}

void Sequence::eval() { 
    bool toggleSingleRecording = !this->mRecording;
    if (toggleSingleRecording) {
        this->begin();
    }

    const vk::PipelineStageFlags waitStageMask = vk::PipelineStageFlagBits::eTransfer;
    vk::SubmitInfo submitInfo(0, nullptr, &waitStageMask, 1, this->mCommandBuffer);

    vk::Fence fence = this->mDevice->createFence(vk::FenceCreateInfo());
    this->mComputeQueue->submit(1, &submitInfo, fence);
    this->mDevice->waitForFences(1, &fence, VK_TRUE, UINT64_MAX);
    this->mDevice->destroy(fence);

    if (toggleSingleRecording) {
        this->end();
    }
}

void Sequence::createCommandPool() {
    SPDLOG_DEBUG("Kompute Sequence creating command pool");
    if (this->mQueueIndex < 0) {
        throw std::runtime_error("Kompute Sequence queue index not provided");
    }

    this->mFreeCommandPool = true;

    vk::CommandPoolCreateInfo commandPoolInfo(vk::CommandPoolCreateFlags(), this->mQueueIndex);
    vk::CommandPool commandPool = this->mDevice->createCommandPool(commandPoolInfo);
    this->mCommandPool = &commandPool;
}

void Sequence::createCommandBuffer() {
    SPDLOG_DEBUG("Kompute Sequence creating command buffer");
    if (this->mDevice == nullptr) {
        throw std::runtime_error("Kompute Sequence device is null");
    }
    if (this->mCommandPool == nullptr) {
        throw std::runtime_error("Kompute Sequence command pool is null");
    }

    this->mFreeCommandBuffer = true;

    vk::CommandBufferAllocateInfo commandBufferAllocateInfo(*this->mCommandPool, vk::CommandBufferLevel::ePrimary, 1);

    vk::CommandBuffer commandBuffer;
    this->mDevice->allocateCommandBuffers(&commandBufferAllocateInfo, &commandBuffer);
    this->mCommandBuffer = &commandBuffer;
}

}
