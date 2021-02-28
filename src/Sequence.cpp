
#include "kompute/Sequence.hpp"

namespace kp {

Sequence::Sequence(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
                   std::shared_ptr<vk::Device> device,
                   std::shared_ptr<vk::Queue> computeQueue,
                   uint32_t queueIndex)
{
    KP_LOG_DEBUG("Kompute Sequence Constructor with existing device & queue");

    this->mPhysicalDevice = physicalDevice;
    this->mDevice = device;
    this->mComputeQueue = computeQueue;
    this->mQueueIndex = queueIndex;

    this->createCommandPool();
    this->createCommandBuffer();
}

Sequence::~Sequence()
{
    KP_LOG_DEBUG("Kompute Sequence Destructor started");

    this->destroy();
}

void
Sequence::begin()
{
    KP_LOG_DEBUG("Kompute sequence called BEGIN");

    if (this->isRecording()) {
        KP_LOG_DEBUG("Kompute Sequence begin called when already recording");
        return;
    }

    if (this->isRunning()) {
        throw std::runtime_error("Kompute Sequence begin called when sequence still running");
    }

    KP_LOG_INFO("Kompute Sequence command now started recording");
    this->mCommandBuffer->begin(vk::CommandBufferBeginInfo());
    this->mRecording = true;
}

void
Sequence::end()
{
    KP_LOG_DEBUG("Kompute Sequence calling END");

    if (!this->isRecording()) {
        KP_LOG_WARN("Kompute Sequence end called when not recording");
        return;
    } 
    else {
        KP_LOG_INFO("Kompute Sequence command recording END");
        this->mCommandBuffer->end();
        this->mRecording = false;
    }
}

void
Sequence::clear() {
    KP_LOG_DEBUG("Kompute Sequence  calling clear");
    this->end();
}

std::shared_ptr<Sequence>
Sequence::eval()
{
    KP_LOG_DEBUG("Kompute sequence EVAL BEGIN");

    return this->evalAsync()->evalAwait();
}

std::shared_ptr<Sequence>
Sequence::eval(std::shared_ptr<OpBase> op) {
    this->clear();
    return this->record(op)->eval();
}

std::shared_ptr<Sequence>
Sequence::evalAsync()
{
    if (this->isRecording()) {
        this->end();
    }

    if (this->mIsRunning) {
        throw std::runtime_error("Kompute Sequence evalAsync called when an eval async was "
                    "called without successful wait");
    }

    this->mIsRunning = true;

    for (size_t i = 0; i < this->mOperations.size(); i++) {
        this->mOperations[i]->preEval(*this->mCommandBuffer);
    }

    vk::SubmitInfo submitInfo(
      0, nullptr, nullptr, 1, this->mCommandBuffer.get());

    this->mFence = this->mDevice->createFence(vk::FenceCreateInfo());

    KP_LOG_DEBUG(
      "Kompute sequence submitting command buffer into compute queue");

    this->mComputeQueue->submit(1, &submitInfo, this->mFence);

    return shared_from_this();
}

std::shared_ptr<Sequence>
Sequence::evalAsync(std::shared_ptr<OpBase> op)
{
    this->clear();
    this->record(op);
    this->evalAsync();
    return shared_from_this();
}

std::shared_ptr<Sequence>
Sequence::evalAwait(uint64_t waitFor)
{
    if (!this->mIsRunning) {
        KP_LOG_WARN("Kompute Sequence evalAwait called without existing eval");
        return shared_from_this();
    }

    vk::Result result =
      this->mDevice->waitForFences(1, &this->mFence, VK_TRUE, waitFor);
    this->mDevice->destroy(
      this->mFence, (vk::Optional<const vk::AllocationCallbacks>)nullptr);

    this->mIsRunning = false;

    if (result == vk::Result::eTimeout) {
        KP_LOG_WARN("Kompute Sequence evalAwait reached timeout of {}", waitFor);
        return shared_from_this();
    }

    for (size_t i = 0; i < this->mOperations.size(); i++) {
        this->mOperations[i]->postEval(*this->mCommandBuffer);
    }

    return shared_from_this();
}

bool
Sequence::isRunning()
{
    return this->mIsRunning;
}

bool
Sequence::isRecording()
{
    return this->mRecording;
}

bool
Sequence::isInit() {
    return this->mDevice &&
        this->mCommandPool &&
        this->mCommandBuffer &&
        this->mComputeQueue;
}

void
Sequence::destroy()
{
    KP_LOG_DEBUG("Kompute Sequence destroy called");

    if (!this->mDevice) {
        KP_LOG_WARN("Kompute Sequence destroy called "
                     "with null Device pointer");
        return;
    }

    if (this->mFreeCommandBuffer) {
        KP_LOG_INFO("Freeing CommandBuffer");
        if (!this->mCommandBuffer) {
            KP_LOG_WARN(
              "Kompute Sequence destroy called with null "
              "CommandPool pointer");
            return;
        }
        this->mDevice->freeCommandBuffers(
          *this->mCommandPool, 1, this->mCommandBuffer.get());

        this->mCommandBuffer = nullptr;
        this->mFreeCommandBuffer = false;

        KP_LOG_DEBUG("Kompute Sequence Freed CommandBuffer");
    }

    if (this->mFreeCommandPool) {
        KP_LOG_INFO("Destroying CommandPool");
        if (this->mCommandPool == nullptr) {
            KP_LOG_WARN(
              "Kompute Sequence destroy called with null "
              "CommandPool pointer");
            return;
        }
        this->mDevice->destroy(
          *this->mCommandPool,
          (vk::Optional<const vk::AllocationCallbacks>)nullptr);

        this->mCommandPool = nullptr;
        this->mFreeCommandPool = false;

        KP_LOG_DEBUG("Kompute Sequence Destroyed CommandPool");
    }

    if (this->mOperations.size()) {
        KP_LOG_INFO("Kompute Sequence clearing operations buffer");
        this->mOperations.clear();
    }

    if (this->mDevice) {
        this->mDevice = nullptr;
    }
    if (this->mPhysicalDevice) {
        this->mPhysicalDevice = nullptr;
    }
    if (this->mComputeQueue) {
        this->mComputeQueue = nullptr;
    }

}

std::shared_ptr<Sequence>
Sequence::record(std::shared_ptr<OpBase> op)
{
    KP_LOG_DEBUG("Kompute Sequence record function started");

    this->begin();

    KP_LOG_DEBUG(
      "Kompute Sequence running record on OpBase derived class instance");

    op->record(*this->mCommandBuffer);

    this->mOperations.push_back(op);

    return shared_from_this();
}

void
Sequence::createCommandPool()
{
    KP_LOG_DEBUG("Kompute Sequence creating command pool");

    if (!this->mDevice) {
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
    KP_LOG_DEBUG("Kompute Sequence Command Pool Created");
}

void
Sequence::createCommandBuffer()
{
    KP_LOG_DEBUG("Kompute Sequence creating command buffer");
    if (!this->mDevice) {
        throw std::runtime_error("Kompute Sequence device is null");
    }
    if (!this->mCommandPool) {
        throw std::runtime_error("Kompute Sequence command pool is null");
    }

    this->mFreeCommandBuffer = true;

    vk::CommandBufferAllocateInfo commandBufferAllocateInfo(
      *this->mCommandPool, vk::CommandBufferLevel::ePrimary, 1);

    this->mCommandBuffer = std::make_shared<vk::CommandBuffer>();
    this->mDevice->allocateCommandBuffers(&commandBufferAllocateInfo,
                                          this->mCommandBuffer.get());
    KP_LOG_DEBUG("Kompute Sequence Command Buffer Created");
}

}
