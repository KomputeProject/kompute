#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>

// SPDLOG_ACTIVE_LEVEL must be defined before spdlog.h import
#if DEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#endif

#include <spdlog/spdlog.h>

#define KP_MAX_DIM_SIZE 1

namespace kp {

class Tensor
{
  public:
    enum class TensorTypes
    {
        eDevice = 0,
        eStaging = 1,
        eStorage = 2,
    };

    Tensor();

    Tensor(std::vector<uint32_t> data,
           TensorTypes tensorType = TensorTypes::eDevice);

    ~Tensor();

    void init(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
              std::shared_ptr<vk::Device> device,
              std::shared_ptr<vk::CommandBuffer> commandBuffer);

    // Create functions
    void createBuffer();

    // Getter functions
    std::vector<uint32_t> data();
    uint32_t size();
    std::array<uint32_t, KP_MAX_DIM_SIZE> shape();
    TensorTypes tensorType();
    bool isInit();

    // Setters
    void setData(const std::vector<uint32_t>& data);

    // Record functions
    void recordCopyFrom(std::shared_ptr<Tensor> copyFromTensor);
    // TODO: Explore simplifying by infering pipeline stage flag bits from
    // access flag bits (as seems to be superset)
    void recordBufferMemoryBarrier(vk::AccessFlagBits srcAccessMask,
                                   vk::AccessFlagBits dstAccessMask,
                                   vk::PipelineStageFlagBits srcStageMask,
                                   vk::PipelineStageFlagBits dstStageMask);

    // Util functions
    vk::DescriptorBufferInfo constructDescriptorBufferInfo();
    void mapDataFromHostMemory();
    void mapDataIntoHostMemory();

  private:
    std::shared_ptr<vk::PhysicalDevice> mPhysicalDevice;
    std::shared_ptr<vk::Device> mDevice;
    std::shared_ptr<vk::CommandBuffer> mCommandBuffer;

    std::shared_ptr<vk::Buffer> mBuffer;
    bool mFreeBuffer;
    std::shared_ptr<vk::DeviceMemory> mMemory;
    bool mFreeMemory;

    std::vector<uint32_t> mData;

    TensorTypes mTensorType = TensorTypes::eDevice;

    std::array<uint32_t, KP_MAX_DIM_SIZE> mShape; // TODO: Only 1D supported
    bool mIsInit = false;
    // uint32_t mDataType;

    // Private util functions
    vk::BufferUsageFlags getBufferUsageFlags();
    vk::MemoryPropertyFlags getMemoryPropertyFlags();
    uint64_t memorySize();
};

} // End namespace kp
