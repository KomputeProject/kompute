#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>

namespace kp {

class Sequence
{
private:

public:
    Sequence();
    Sequence(vk::Device* device, vk::Queue* computeQueue, uint32_t queueIndex);
    ~Sequence();

    // Record command functions
    void begin();
    void end();
    void eval();

    template <typename T, typename...TArgs>
    void record(TArgs&&... args) {
        T op(this->mCommandBuffer);
        op.init(std::forward<TArgs>(args)...);
        op.record();
    }


private:
    vk::Device* mDevice = nullptr;
    vk::Queue* mComputeQueue = nullptr;
    uint32_t mQueueIndex = -1;
    vk::CommandPool* mCommandPool = nullptr;
    bool mFreeCommandPool = false;
    vk::CommandBuffer* mCommandBuffer = nullptr;
    bool mFreeCommandBuffer = false;

    // Record state
    bool mRecording = false;

    // Create functions
    void createCommandPool();
    void createCommandBuffer();

};

} // End namespace kp

