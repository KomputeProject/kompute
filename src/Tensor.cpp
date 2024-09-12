// SPDX-License-Identifier: Apache-2.0

#include "kompute/Tensor.hpp"
#include "kompute/Image.hpp"

namespace kp {

Tensor::Tensor(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
               std::shared_ptr<vk::Device> device,
               void* data,
               uint32_t elementTotalCount,
               uint32_t elementMemorySize,
               const DataTypes& dataType,
               const MemoryTypes& memoryType)
  : Memory(physicalDevice, device, dataType, memoryType, elementTotalCount, 1)
{
    this->mSize = elementTotalCount;

    // This is required if dataType is eCustom
    this->mDataTypeMemorySize = elementMemorySize;

    KP_LOG_DEBUG("Kompute Tensor constructor data length: {}, and type: {}",
                 elementTotalCount,
                 Memory::toString(memoryType));

    this->mDescriptorType = vk::DescriptorType::eStorageBuffer;

    this->reserve();
    this->updateRawData(data);
}

Tensor::Tensor(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
               std::shared_ptr<vk::Device> device,
               uint32_t elementTotalCount,
               uint32_t elementMemorySize,
               const DataTypes& dataType,
               const MemoryTypes& memoryType)
  : Memory(physicalDevice, device, dataType, memoryType, elementTotalCount, 1)
{
    this->mSize = elementTotalCount;

    // This is required if dataType is eCustom
    this->mDataTypeMemorySize = elementMemorySize;

    KP_LOG_DEBUG("Kompute Tensor constructor data length: {}, and type: {}",
                 elementTotalCount,
                 Memory::toString(memoryType));

    this->mDescriptorType = vk::DescriptorType::eStorageBuffer;

    this->reserve();
}

Tensor::~Tensor()
{
    KP_LOG_DEBUG("Kompute Tensor destructor started. Type: {}",
                 Memory::toString(this->memoryType()));

    if (this->mDevice) {
        this->destroy();
    }

    KP_LOG_DEBUG("Kompute Tensor destructor success");
}

void
Tensor::reserve()
{
    KP_LOG_DEBUG("Reserving {} bytes for memory",
                 this->mSize * this->mDataTypeMemorySize);

    if (this->mPrimaryBuffer || this->mPrimaryMemory) {
        KP_LOG_DEBUG(
          "Kompute Tensor destroying existing resources before rebuild");
        this->destroy();
    }

    this->allocateMemoryCreateGPUResources();
}

bool
Tensor::isInit()
{
    return this->mDevice && this->mPrimaryBuffer && this->mPrimaryMemory;
}

void
Tensor::recordCopyFrom(const vk::CommandBuffer& commandBuffer,
                       std::shared_ptr<Tensor> copyFromTensor)
{

    vk::DeviceSize bufferSize(this->memorySize());
    vk::BufferCopy copyRegion(0, 0, bufferSize);

    KP_LOG_DEBUG("Kompute Tensor recordCopyFrom data size {}.", bufferSize);

    this->recordCopyBuffer(commandBuffer,
                           copyFromTensor->mPrimaryBuffer,
                           this->mPrimaryBuffer,
                           bufferSize,
                           copyRegion);
}

void
Tensor::recordCopyFrom(const vk::CommandBuffer& commandBuffer,
                       std::shared_ptr<Image> copyFromImage)
{

    vk::DeviceSize bufferSize(this->memorySize());

    vk::ImageSubresourceLayers layer = {};
    layer.aspectMask = vk::ImageAspectFlagBits::eColor;
    layer.layerCount = 1;
    vk::Offset3D offset = { 0, 0, 0 };

    vk::Extent3D size = { copyFromImage->getX(), copyFromImage->getY(), 1 };

    vk::BufferImageCopy copyRegion(0, 0, 0, layer, offset, size);

    KP_LOG_DEBUG("Kompute Tensor recordCopyFrom data size {}.", bufferSize);

    copyFromImage->recordPrimaryImageBarrier(
      commandBuffer,
      vk::AccessFlagBits::eMemoryRead,
      vk::AccessFlagBits::eMemoryWrite,
      vk::PipelineStageFlagBits::eTransfer,
      vk::PipelineStageFlagBits::eTransfer,
      vk::ImageLayout::eTransferSrcOptimal);

    this->recordCopyBufferFromImage(commandBuffer,
                                    copyFromImage->getPrimaryImage(),
                                    this->mPrimaryBuffer,
                                    copyFromImage->getPrimaryImageLayout(),
                                    bufferSize,
                                    copyRegion);
}

void
Tensor::recordCopyFromStagingToDevice(const vk::CommandBuffer& commandBuffer)
{
    vk::DeviceSize bufferSize(this->memorySize());
    vk::BufferCopy copyRegion(0, 0, bufferSize);

    KP_LOG_DEBUG("Kompute Tensor copying data size {}.", bufferSize);

    this->recordCopyBuffer(commandBuffer,
                           this->mStagingBuffer,
                           this->mPrimaryBuffer,
                           bufferSize,
                           copyRegion);
}

void
Tensor::recordCopyFromDeviceToStaging(const vk::CommandBuffer& commandBuffer)
{
    vk::DeviceSize bufferSize(this->memorySize());
    vk::BufferCopy copyRegion(0, 0, bufferSize);

    KP_LOG_DEBUG("Kompute Tensor copying data size {}.", bufferSize);

    this->recordCopyBuffer(commandBuffer,
                           this->mPrimaryBuffer,
                           this->mStagingBuffer,
                           bufferSize,
                           copyRegion);
}

void
Tensor::recordCopyBuffer(const vk::CommandBuffer& commandBuffer,
                         std::shared_ptr<vk::Buffer> bufferFrom,
                         std::shared_ptr<vk::Buffer> bufferTo,
                         vk::DeviceSize /*bufferSize*/,
                         vk::BufferCopy copyRegion)
{

    commandBuffer.copyBuffer(*bufferFrom, *bufferTo, copyRegion);
}

void
Tensor::recordCopyBufferFromImage(const vk::CommandBuffer& commandBuffer,
                                  std::shared_ptr<vk::Image> imageFrom,
                                  std::shared_ptr<vk::Buffer> bufferTo,
                                  vk::ImageLayout fromLayout,
                                  vk::DeviceSize /*bufferSize*/,
                                  vk::BufferImageCopy copyRegion)
{
    commandBuffer.copyImageToBuffer(
      *imageFrom, fromLayout, *bufferTo, 1, &copyRegion);
}

void
Tensor::recordPrimaryMemoryBarrier(const vk::CommandBuffer& commandBuffer,
                                   vk::AccessFlagBits srcAccessMask,
                                   vk::AccessFlagBits dstAccessMask,
                                   vk::PipelineStageFlagBits srcStageMask,
                                   vk::PipelineStageFlagBits dstStageMask)
{
    KP_LOG_DEBUG("Kompute Tensor recording PRIMARY buffer memory barrier");

    this->recordBufferMemoryBarrier(commandBuffer,
                                    *this->mPrimaryBuffer,
                                    srcAccessMask,
                                    dstAccessMask,
                                    srcStageMask,
                                    dstStageMask);
}

void
Tensor::recordStagingMemoryBarrier(const vk::CommandBuffer& commandBuffer,
                                   vk::AccessFlagBits srcAccessMask,
                                   vk::AccessFlagBits dstAccessMask,
                                   vk::PipelineStageFlagBits srcStageMask,
                                   vk::PipelineStageFlagBits dstStageMask)
{
    KP_LOG_DEBUG("Kompute Tensor recording STAGING buffer memory barrier");

    this->recordBufferMemoryBarrier(commandBuffer,
                                    *this->mStagingBuffer,
                                    srcAccessMask,
                                    dstAccessMask,
                                    srcStageMask,
                                    dstStageMask);
}

void
Tensor::recordBufferMemoryBarrier(const vk::CommandBuffer& commandBuffer,
                                  const vk::Buffer& buffer,
                                  vk::AccessFlagBits srcAccessMask,
                                  vk::AccessFlagBits dstAccessMask,
                                  vk::PipelineStageFlagBits srcStageMask,
                                  vk::PipelineStageFlagBits dstStageMask)
{
    KP_LOG_DEBUG("Kompute Tensor recording buffer memory barrier");

    vk::DeviceSize bufferSize = this->memorySize();

    vk::BufferMemoryBarrier bufferMemoryBarrier;
    bufferMemoryBarrier.buffer = buffer;
    bufferMemoryBarrier.size = bufferSize;
    bufferMemoryBarrier.srcAccessMask = srcAccessMask;
    bufferMemoryBarrier.dstAccessMask = dstAccessMask;
    bufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    bufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    commandBuffer.pipelineBarrier(srcStageMask,
                                  dstStageMask,
                                  vk::DependencyFlags(),
                                  nullptr,
                                  bufferMemoryBarrier,
                                  nullptr);
}

vk::DescriptorBufferInfo
Tensor::constructDescriptorBufferInfo()
{
    KP_LOG_DEBUG("Kompute Tensor construct descriptor buffer info size {}",
                 this->memorySize());
    vk::DeviceSize bufferSize = this->memorySize();
    return vk::DescriptorBufferInfo(*this->mPrimaryBuffer,
                                    0, // offset
                                    bufferSize);
}

vk::WriteDescriptorSet
Tensor::constructDescriptorSet(vk::DescriptorSet descriptorSet,
                               uint32_t binding)
{
    KP_LOG_DEBUG("Kompute Tensor construct descriptor set for binding {}",
                 binding);

    mDescriptorBufferInfo = this->constructDescriptorBufferInfo();

    vk::WriteDescriptorSet writeDesciptorSet = vk::WriteDescriptorSet(
      descriptorSet,
      binding, // Destination binding
      0,       // Destination array element
      1,       // Descriptor count
      vk::DescriptorType::eStorageBuffer,
      nullptr, // Descriptor image info
      // Using a member variable here means the address is still valid later
      &mDescriptorBufferInfo);

    return writeDesciptorSet;
}

vk::BufferUsageFlags
Tensor::getPrimaryBufferUsageFlags()
{
    switch (this->mMemoryType) {
        case MemoryTypes::eDevice:
        case MemoryTypes::eHost:
        case MemoryTypes::eDeviceAndHost:
            return vk::BufferUsageFlagBits::eStorageBuffer |
                   vk::BufferUsageFlagBits::eTransferSrc |
                   vk::BufferUsageFlagBits::eTransferDst;
            break;
        case MemoryTypes::eStorage:
            return vk::BufferUsageFlagBits::eStorageBuffer |
                   // You can still copy buffers to/from storage memory
                   // so set the transfer usage flags here.
                   vk::BufferUsageFlagBits::eTransferSrc |
                   vk::BufferUsageFlagBits::eTransferDst;
            break;
        default:
            throw std::runtime_error("Kompute Tensor invalid tensor type");
    }
}

vk::BufferUsageFlags
Tensor::getStagingBufferUsageFlags()
{
    switch (this->mMemoryType) {
        case MemoryTypes::eDevice:
            return vk::BufferUsageFlagBits::eTransferSrc |
                   vk::BufferUsageFlagBits::eTransferDst;
            break;
        default:
            throw std::runtime_error("Kompute Tensor invalid tensor type");
    }
}

std::shared_ptr<vk::Buffer>
Tensor::getPrimaryBuffer()
{
    return this->mPrimaryBuffer;
}

void
Tensor::allocateMemoryCreateGPUResources()
{
    KP_LOG_DEBUG("Kompute Tensor creating buffer");

    if (!this->mPhysicalDevice) {
        throw std::runtime_error("Kompute Tensor phyisical device is null");
    }
    if (!this->mDevice) {
        throw std::runtime_error("Kompute Tensor device is null");
    }

    KP_LOG_DEBUG("Kompute Tensor creating primary buffer and memory");

    this->mPrimaryBuffer = std::make_shared<vk::Buffer>();
    this->createBuffer(this->mPrimaryBuffer,
                       this->getPrimaryBufferUsageFlags());
    this->mFreePrimaryBuffer = true;
    this->mPrimaryMemory = std::make_shared<vk::DeviceMemory>();
    this->allocateBindMemory(this->mPrimaryBuffer,
                             this->mPrimaryMemory,
                             this->getPrimaryMemoryPropertyFlags());
    this->mFreePrimaryMemory = true;

    if (this->mMemoryType == MemoryTypes::eDevice) {
        KP_LOG_DEBUG("Kompute Tensor creating staging buffer and memory");

        this->mStagingBuffer = std::make_shared<vk::Buffer>();
        this->createBuffer(this->mStagingBuffer,
                           this->getStagingBufferUsageFlags());
        this->mFreeStagingBuffer = true;
        this->mStagingMemory = std::make_shared<vk::DeviceMemory>();
        this->allocateBindMemory(this->mStagingBuffer,
                                 this->mStagingMemory,
                                 this->getStagingMemoryPropertyFlags());
        this->mFreeStagingMemory = true;
    }

    KP_LOG_DEBUG("Kompute Tensor buffer & memory creation successful");
}

void
Tensor::createBuffer(std::shared_ptr<vk::Buffer> buffer,
                     vk::BufferUsageFlags bufferUsageFlags)
{

    vk::DeviceSize bufferSize = this->memorySize();

    if (bufferSize < 1) {
        throw std::runtime_error(
          "Kompute Tensor attempted to create a zero-sized buffer");
    }

    KP_LOG_DEBUG("Kompute Tensor creating buffer with memory size: {}, and "
                 "usage flags: {}",
                 bufferSize,
                 vk::to_string(bufferUsageFlags));

    // TODO: Explore having concurrent sharing mode (with option)
    vk::BufferCreateInfo bufferInfo(vk::BufferCreateFlags(),
                                    bufferSize,
                                    bufferUsageFlags,
                                    vk::SharingMode::eExclusive);

    this->mDevice->createBuffer(&bufferInfo, nullptr, buffer.get());
}

void
Tensor::allocateBindMemory(std::shared_ptr<vk::Buffer> buffer,
                           std::shared_ptr<vk::DeviceMemory> memory,
                           vk::MemoryPropertyFlags memoryPropertyFlags)
{

    KP_LOG_DEBUG("Kompute Tensor allocating and binding memory");

    vk::PhysicalDeviceMemoryProperties memoryProperties =
      this->mPhysicalDevice->getMemoryProperties();

    vk::MemoryRequirements memoryRequirements =
      this->mDevice->getBufferMemoryRequirements(*buffer);

    uint32_t memoryTypeIndex = -1;
    bool memoryTypeIndexFound = false;
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
        if (memoryRequirements.memoryTypeBits & (1 << i)) {
            if (((memoryProperties.memoryTypes[i]).propertyFlags &
                 memoryPropertyFlags) == memoryPropertyFlags) {
                memoryTypeIndex = i;
                memoryTypeIndexFound = true;
                break;
            }
        }
    }
    if (!memoryTypeIndexFound) {
        throw std::runtime_error(
          "Memory type index for buffer creation not found");
    }

    KP_LOG_DEBUG(
      "Kompute Tensor allocating memory index: {}, size {}, flags: {}",
      memoryTypeIndex,
      memoryRequirements.size,
      vk::to_string(memoryPropertyFlags));

    vk::MemoryAllocateInfo memoryAllocateInfo(memoryRequirements.size,
                                              memoryTypeIndex);

    this->mDevice->allocateMemory(&memoryAllocateInfo, nullptr, memory.get());

    this->mDevice->bindBufferMemory(*buffer, *memory, 0);
}

void
Tensor::destroy()
{
    KP_LOG_DEBUG("Kompute Tensor started destroy()");

    if (!this->mDevice) {
        KP_LOG_WARN(
          "Kompute Tensor destructor reached with null Device pointer");
        return;
    }

    if (this->mFreePrimaryBuffer) {
        if (!this->mPrimaryBuffer) {
            KP_LOG_WARN("Kompose Tensor expected to destroy primary buffer "
                        "but got null buffer");
        } else {
            KP_LOG_DEBUG("Kompose Tensor destroying primary buffer");
            this->mDevice->destroy(
              *this->mPrimaryBuffer,
              (vk::Optional<const vk::AllocationCallbacks>)nullptr);
            this->mPrimaryBuffer = nullptr;
            this->mFreePrimaryBuffer = false;
        }
    }

    if (this->mFreeStagingBuffer) {
        if (!this->mStagingBuffer) {
            KP_LOG_WARN("Kompose Tensor expected to destroy staging buffer "
                        "but got null buffer");
        } else {
            KP_LOG_DEBUG("Kompose Tensor destroying staging buffer");
            this->mDevice->destroy(
              *this->mStagingBuffer,
              (vk::Optional<const vk::AllocationCallbacks>)nullptr);
            this->mStagingBuffer = nullptr;
            this->mFreeStagingBuffer = false;
        }
    }

    Memory::destroy();

    KP_LOG_DEBUG("Kompute Tensor successful destroy()");
}
}
