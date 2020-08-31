
#if DEBUG
#include <fmt/ranges.h>
#endif

#include "kompute/Tensor.hpp"

namespace kp {

Tensor::Tensor()
{
    SPDLOG_DEBUG("Kompute Tensor base constructor");
    this->mTensorType = TensorTypes::eDevice;
}

Tensor::Tensor(std::vector<float> data, TensorTypes tensorType)
{
    SPDLOG_DEBUG(
      "Kompute Tensor constructor data: {}, and type: {}", data, tensorType);

    this->mData = data;
    this->mShape = { static_cast<uint32_t>(data.size()) };
    this->mTensorType = tensorType;
}

Tensor::~Tensor()
{
    SPDLOG_DEBUG("Kompute Tensor destructor started. Type: {}", this->tensorType());

    if (this->isInit()) {
        this->freeMemoryDestroyGPUResources();
    }

    SPDLOG_DEBUG("Kompute Tensor destructor success");
}

void
Tensor::init(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
             std::shared_ptr<vk::Device> device,
             std::shared_ptr<vk::CommandBuffer> commandBuffer)
{
    SPDLOG_DEBUG("Kompute Tensor running init with Vulkan params and num data "
                 "elementS: {}",
                 this->mData.size());

    this->mPhysicalDevice = physicalDevice;
    this->mDevice = device;
    this->mCommandBuffer = commandBuffer;

    this->mIsInit = true;

    this->createBuffer();
}

std::vector<float>
Tensor::data()
{
    return this->mData;
}

uint64_t
Tensor::memorySize()
{
    return this->size() * sizeof(float);
}

uint32_t
Tensor::size()
{
    return this->mShape[0];
}

std::array<uint32_t, KP_MAX_DIM_SIZE>
Tensor::shape()
{
    return this->mShape;
}

Tensor::TensorTypes
Tensor::tensorType()
{
    return this->mTensorType;
}

bool
Tensor::isInit()
{
    return this->mIsInit && this->mBuffer && this->mMemory;
}

void
Tensor::setData(const std::vector<float>& data)
{
    this->mData = data;
}

void
Tensor::recordCopyFrom(std::shared_ptr<Tensor> copyFromTensor,
                       bool createBarrier)
{
    SPDLOG_DEBUG("Kompute Tensor recordCopyFrom called");

    if (!this->mIsInit || !copyFromTensor->mIsInit) {
        throw std::runtime_error(
          "Kompute Tensor attempted to run createBuffer without init");
    }

    // TODO: Allow for dst and src offsets to be configured
    // TODO: Test and ensure sizes for tensors are compatible
    vk::DeviceSize bufferSize(this->memorySize());
    vk::BufferCopy copyRegion(0, 0, bufferSize);

    SPDLOG_DEBUG("Kompute Tensor copying data size {}.", bufferSize);

    // TODO: Ensure command buffer is in same device from buffer
    this->mCommandBuffer->copyBuffer(
      *copyFromTensor->mBuffer, *this->mBuffer, copyRegion);

    if (createBarrier) {
        // Buffer to ensure wait until data is copied to staging buffer
        this->recordBufferMemoryBarrier(vk::AccessFlagBits::eTransferWrite,
                                        vk::AccessFlagBits::eHostRead,
                                        vk::PipelineStageFlagBits::eTransfer,
                                        vk::PipelineStageFlagBits::eHost);
    }
}

void
Tensor::recordBufferMemoryBarrier(vk::AccessFlagBits srcAccessMask,
                                  vk::AccessFlagBits dstAccessMask,
                                  vk::PipelineStageFlagBits srcStageMask,
                                  vk::PipelineStageFlagBits dstStageMask)
{
    SPDLOG_DEBUG("Kompute Tensor recording buffer memory barrier");

    vk::DeviceSize bufferSize = this->memorySize();

    vk::BufferMemoryBarrier bufferMemoryBarrier;
    bufferMemoryBarrier.buffer = *this->mBuffer;
    bufferMemoryBarrier.size = bufferSize;
    bufferMemoryBarrier.srcAccessMask = srcAccessMask;
    bufferMemoryBarrier.dstAccessMask = dstAccessMask;
    bufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    bufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    this->mCommandBuffer->pipelineBarrier(srcStageMask,
                                          dstStageMask,
                                          vk::DependencyFlags(),
                                          nullptr,
                                          bufferMemoryBarrier,
                                          nullptr);
}

// TODO: Explore if this function should be here or expose buffer
vk::DescriptorBufferInfo
Tensor::constructDescriptorBufferInfo()
{
    vk::DeviceSize bufferSize = this->memorySize();
    return vk::DescriptorBufferInfo(*this->mBuffer,
                                    0, // offset
                                    bufferSize);
}

void
Tensor::mapDataFromHostMemory()
{
    SPDLOG_DEBUG("Kompute Tensor mapping data from host buffer");

    if (this->mTensorType != TensorTypes::eStaging) {
        spdlog::error(
          "Mapping tensor data manually from DEVICE buffer instead of "
          "using record GPU command with staging buffer");
        return;
    }

    vk::DeviceSize bufferSize = this->memorySize();
    void* mapped = this->mDevice->mapMemory(
      *this->mMemory, 0, bufferSize, vk::MemoryMapFlags());
    vk::MappedMemoryRange mappedMemoryRange(*this->mMemory, 0, bufferSize);
    this->mDevice->invalidateMappedMemoryRanges(mappedMemoryRange);
    memcpy(this->mData.data(), mapped, bufferSize);
    this->mDevice->unmapMemory(*this->mMemory);
}

void
Tensor::mapDataIntoHostMemory()
{

    SPDLOG_DEBUG("Kompute Tensor local mapping tensor data to host buffer");

    // TODO: Verify if there are situations where we want to copy to device
    // memory
    if (this->mTensorType != TensorTypes::eStaging) {
        spdlog::error(
          "Mapping tensor data manually to DEVICE memory instead of "
          "using record GPU command with staging buffer");
        return;
    }

    vk::DeviceSize bufferSize = this->memorySize();

    // TODO: Verify if flushed memory ranges should happend in sequence
    void* mapped = this->mDevice->mapMemory(
      *this->mMemory, 0, bufferSize, vk::MemoryMapFlags());
    memcpy(mapped, this->mData.data(), bufferSize);
    this->mDevice->unmapMemory(*this->mMemory);

    mapped = this->mDevice->mapMemory(
      *this->mMemory, 0, bufferSize, vk::MemoryMapFlags());
    vk::MappedMemoryRange mappedRange(*this->mMemory, 0, bufferSize);
    this->mDevice->flushMappedMemoryRanges(1, &mappedRange);
    this->mDevice->unmapMemory(*this->mMemory);
}

vk::BufferUsageFlags
Tensor::getBufferUsageFlags()
{
    switch (this->mTensorType) {
        case TensorTypes::eDevice:
            return vk::BufferUsageFlagBits::eStorageBuffer |
                   vk::BufferUsageFlagBits::eTransferSrc |
                   vk::BufferUsageFlagBits::eTransferDst;
            break;
        case TensorTypes::eStaging:
            return vk::BufferUsageFlagBits::eTransferSrc |
                   vk::BufferUsageFlagBits::eTransferDst;
            break;
        case TensorTypes::eStorage:
            return vk::BufferUsageFlagBits::eStorageBuffer;
            break;
        default:
            throw std::runtime_error("Kompute Tensor invalid tensor type");
    }
}

vk::MemoryPropertyFlags
Tensor::getMemoryPropertyFlags()
{
    switch (this->mTensorType) {
        case TensorTypes::eDevice:
            return vk::MemoryPropertyFlagBits::eDeviceLocal;
            break;
        case TensorTypes::eStaging:
            return vk::MemoryPropertyFlagBits::eHostVisible;
            break;
        case TensorTypes::eStorage:
            return vk::MemoryPropertyFlagBits::eDeviceLocal;
            break;
        default:
            throw std::runtime_error("Kompute Tensor invalid tensor type");
    }
}

void
Tensor::createBuffer()
{
    SPDLOG_DEBUG("Kompute Tensor creating buffer");

    if (!this->mIsInit) {
        throw std::runtime_error(
          "Kompute Tensor attempted to run createBuffer without init");
    }

    if (!this->mPhysicalDevice) {
        throw std::runtime_error("Kompute Tensor phyisical device is null");
    }
    if (!this->mDevice) {
        throw std::runtime_error("Kompute Tensor device is null");
    }

    this->mFreeBuffer = true;

    vk::BufferUsageFlags usageFlags = this->getBufferUsageFlags();
    vk::DeviceSize bufferSize = this->memorySize();

    SPDLOG_DEBUG("Kompute Tensor creating buffer with memory size: {}, and "
                 "usage flags: {}",
                 bufferSize,
                 vk::to_string(usageFlags));

    vk::BufferCreateInfo bufferInfo(vk::BufferCreateFlags(),
                                    bufferSize,
                                    usageFlags,
                                    vk::SharingMode::eExclusive);

    this->mBuffer = std::make_shared<vk::Buffer>();
    this->mDevice->createBuffer(&bufferInfo, nullptr, this->mBuffer.get());

    SPDLOG_DEBUG("Kompute Tensor buffer created now creating memory");

    vk::PhysicalDeviceMemoryProperties memoryProperties =
      this->mPhysicalDevice->getMemoryProperties();

    vk::MemoryRequirements memoryRequirements =
      this->mDevice->getBufferMemoryRequirements(*this->mBuffer);

    vk::MemoryPropertyFlags memoryPropertyFlags =
      this->getMemoryPropertyFlags();

    uint32_t memoryTypeIndex = -1;
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
        if (memoryRequirements.memoryTypeBits & (1 << i)) {
            if ((memoryProperties.memoryTypes[i].propertyFlags &
                 memoryPropertyFlags) == memoryPropertyFlags) {
                memoryTypeIndex = i;
                break;
            }
        }
    }
    if (memoryTypeIndex < 0) {
        throw std::runtime_error(
          "Memory type index for buffer creation not found");
    }

    this->mFreeMemory = true;

    SPDLOG_DEBUG(
      "Kompute Tensor allocating memory index: {}, size {}, flags: {}",
      memoryTypeIndex,
      memoryRequirements.size,
      vk::to_string(memoryPropertyFlags));

    vk::MemoryAllocateInfo memoryAllocateInfo(memoryRequirements.size,
                                              memoryTypeIndex);

    this->mMemory = std::make_shared<vk::DeviceMemory>();
    this->mDevice->allocateMemory(
      &memoryAllocateInfo, nullptr, this->mMemory.get());

    this->mDevice->bindBufferMemory(*this->mBuffer, *this->mMemory, 0);

    SPDLOG_DEBUG("Kompute Tensor buffer & memory creation successful");
}

void
Tensor::freeMemoryDestroyGPUResources()
{
    SPDLOG_DEBUG("Kompute Tensor started freeMemoryDestroyGPUResources");

    this->mIsInit = false;

    if (!this->mDevice) {
        spdlog::error(
          "Kompute Tensor destructor reached with null Device pointer");
        return;
    }

    if (this->mFreeBuffer) {
        if (!this->mBuffer) {
            spdlog::error(
              "Kompose Tensor expected to free buffer but got null buffer");
        } else {
            SPDLOG_DEBUG("Kompose Tensor destroying buffer");
            this->mDevice->destroy(*this->mBuffer);
            this->mBuffer = nullptr;
        }
    }

    if (this->mFreeMemory) {
        if (!this->mMemory) {
            spdlog::error(
              "Kompose Tensor expected to free buffer but got null memory");
        } else {
            SPDLOG_DEBUG("Kompose Tensor freeing memory");
            this->mDevice->freeMemory(*this->mMemory);
            this->mDevice = nullptr;
        }
    }

    SPDLOG_DEBUG("Kompute Tensor successful freeMemoryDestroyGPUResources");
}

}
