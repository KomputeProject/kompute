
#include "Sequence.hpp"

namespace kp {

Sequence::Sequence()
{
    SPDLOG_DEBUG("Kompute Sequence base constructor");
}

Sequence::Sequence(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
                   std::shared_ptr<vk::Device> device,
                   std::shared_ptr<vk::Queue> computeQueue,
                   uint32_t queueIndex)
{
    SPDLOG_DEBUG("Kompute Sequence Constructor with existing device & queue");

    this->mPhysicalDevice = physicalDevice;
    this->mDevice = device;
    this->mComputeQueue = computeQueue;
    this->mQueueIndex = queueIndex;

    this->createCommandPool();
    this->createCommandBuffer();
}

Sequence::~Sequence()
{
    SPDLOG_DEBUG("Kompute Sequence Destructor started");

    if (!this->mDevice) {
        spdlog::error(
          "Kompute Sequence destructor reached with null Device pointer");
        return;
    }

    if (this->mFreeCommandBuffer) {
        spdlog::info("Freeing CommandBuffer");
        if (!this->mCommandBuffer) {
            spdlog::error("Kompute Sequence destructor reached with null "
                          "CommandPool pointer");
            return;
        }
        this->mDevice->freeCommandBuffers(
          *this->mCommandPool, 1, this->mCommandBuffer.get());
        SPDLOG_DEBUG("Kompute Manager Freed CommandBuffer");
    }

    if (this->mFreeCommandPool) {
        spdlog::info("Destroying CommandPool");
        if (this->mCommandPool == nullptr) {
            spdlog::error("Kompute Sequence destructor reached with null "
                          "CommandPool pointer");
            return;
        }
        this->mDevice->destroy(*this->mCommandPool);
        SPDLOG_DEBUG("Kompute Manager Destroyed CommandPool");
    }
}

void
Sequence::begin()
{
    if (this->mCommandPool == nullptr) {
        throw std::runtime_error("Kompute Sequence command pool is null");
    }

    if (!this->mRecording) {
        spdlog::info("Kompute Sequence command recording BEGIN");
        this->mCommandBuffer->begin(vk::CommandBufferBeginInfo());
        this->mRecording = true;
    } else {
        spdlog::warn("Kompute Sequence attempted to start command recording "
                     "but recording already started");
    }
}

void
Sequence::end()
{
    if (this->mCommandPool == nullptr) {
        throw std::runtime_error("Kompute Sequence command pool is null");
    }

    if (this->mRecording) {
        spdlog::info("Kompute Sequence command recording END");
        this->mCommandBuffer->end();
        this->mRecording = false;
    } else {
        spdlog::warn("Kompute Sequence attempted to end command recording but "
                     "recording not started");
    }
}

void
Sequence::eval()
{
    SPDLOG_DEBUG("Kompute sequence compute recording EVAL");

    bool toggleSingleRecording = !this->mRecording;
    if (toggleSingleRecording) {
        this->begin();
    }

    const vk::PipelineStageFlags waitStageMask =
      vk::PipelineStageFlagBits::eTransfer;
    vk::SubmitInfo submitInfo(
      0, nullptr, &waitStageMask, 1, this->mCommandBuffer.get());

    vk::Fence fence = this->mDevice->createFence(vk::FenceCreateInfo());

    SPDLOG_DEBUG(
      "Kompute sequence submitting command buffer into compute queue");

    this->mComputeQueue->submit(1, &submitInfo, fence);
    this->mDevice->waitForFences(1, &fence, VK_TRUE, UINT64_MAX);
    this->mDevice->destroy(fence);

    if (toggleSingleRecording) {
        this->end();
    }

    // TODO: Explore whether moving postSubmit calls to a separate sequence function that is explicitly called by the manager
    for (size_t i = 0; i < this->mOperations.size(); i++) {
        this->mOperations[i]->postSubmit();
    }

    SPDLOG_DEBUG("Kompute sequence EVAL success");
}

void
Sequence::createCommandPool()
{
    SPDLOG_DEBUG("Kompute Sequence creating command pool");

    if (this->mDevice == nullptr) {
        throw std::runtime_error("Kompute Sequence device is null");
    }
    if (this->mQueueIndex < 0) {
        throw std::runtime_error("Kompute Sequence queue index not provided");
    }

    this->mFreeCommandPool = true;

    vk::CommandPoolCreateInfo commandPoolInfo(vk::CommandPoolCreateFlags(),
                                              this->mQueueIndex);
    this->mCommandPool = std::make_shared<vk::CommandPool>();
    this->mDevice->createCommandPool(
      &commandPoolInfo, nullptr, this->mCommandPool.get());
    SPDLOG_DEBUG("Kompute Manager Command Pool Created");
}

void
Sequence::createCommandBuffer()
{
    SPDLOG_DEBUG("Kompute Sequence creating command buffer");
    if (this->mDevice == nullptr) {
        throw std::runtime_error("Kompute Sequence device is null");
    }
    if (this->mCommandPool == nullptr) {
        throw std::runtime_error("Kompute Sequence command pool is null");
    }

    this->mFreeCommandBuffer = true;

    vk::CommandBufferAllocateInfo commandBufferAllocateInfo(
      *this->mCommandPool, vk::CommandBufferLevel::ePrimary, 1);

    this->mCommandBuffer = std::make_shared<vk::CommandBuffer>();
    this->mDevice->allocateCommandBuffers(&commandBufferAllocateInfo,
                                          this->mCommandBuffer.get());
    SPDLOG_DEBUG("Kompute Manager Command Buffer Created");
}

}
