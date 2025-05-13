// SPDX-License-Identifier: Apache-2.0

#include "kompute/ExternalTensor.hpp"
#include "kompute/Image.hpp"

namespace kp {

ExternalTensor::ExternalTensor(std::shared_ptr<vk::PhysicalDevice> physicalDevice, std::shared_ptr<vk::Device> device)
  : Memory(physicalDevice, device, kp::Memory::DataTypes::eCustom, kp::Memory::MemoryTypes::eDevice, 1, 1, 1)
{
    // We don't know the size yet
    this->mSize = 0;
    this->mDataTypeMemorySize = 0;

    KP_LOG_DEBUG("Kompute ExternalTensor constructor");

    this->mDescriptorType = vk::DescriptorType::eStorageBuffer;
}

ExternalTensor::~ExternalTensor()
{
    KP_LOG_DEBUG("Kompute ExternalTensor destructor started. Type: {}",
                 Memory::toString(this->memoryType()));

    if (this->mDevice) {
        this->destroy();
    }

    KP_LOG_DEBUG("Kompute ExternalTensor destructor success");
}

bool
ExternalTensor::isInit()
{
    return this->mDevice && this->mPrimaryMemory;
}

#ifdef ANDROID

void ExternalTensor::import(AHardwareBuffer* hardwareBuffer) {
    KP_LOG_DEBUG("Kompute ExternalTensor importing Android external buffer");

    // Acquire the hardware buffer
    vk::AndroidHardwareBufferPropertiesANDROID props;

    auto result = mDevice->getAndroidHardwareBufferPropertiesANDROID(hardwareBuffer, &props);
    if(result != vk::Result::eSuccess)
        throw std::runtime_error("Failed to get external buffer properties (error: " + vk::to_string(result) + ")");

    // Import memory
    vk::ImportAndroidHardwareBufferInfoANDROID importInfo;
    importInfo.buffer = hardwareBuffer;

    vk::MemoryAllocateInfo allocInfo;

    allocInfo.pNext = &importInfo;
    allocInfo.allocationSize = props.allocationSize;
    allocInfo.memoryTypeIndex = props.memoryTypeBits;

    KP_LOG_DEBUG("Importing buffer of size: {} and type: {}", props.allocationSize, props.memoryTypeBits);

    mPrimaryMemory = std::make_shared<vk::DeviceMemory>();

    result = mDevice->allocateMemory(&allocInfo, nullptr, mPrimaryMemory.get());
    if(result != vk::Result::eSuccess)
        throw std::runtime_error("Failed to allocate memory for external buffer (error: " + vk::to_string(result) + ")");

    // Create buffer
    vk::ExternalMemoryBufferCreateInfo externalInfo;
    externalInfo.handleTypes = vk::ExternalMemoryHandleTypeFlagBits::eAndroidHardwareBufferANDROID;

    vk::BufferCreateInfo bufferInfo;
    bufferInfo.pNext = &externalInfo;
    bufferInfo.size = props.allocationSize;
    bufferInfo.usage = vk::BufferUsageFlagBits::eStorageBuffer;

    mPrimaryBuffer = std::make_unique<vk::Buffer>();

    result = mDevice->createBuffer(&bufferInfo, nullptr, mPrimaryBuffer.get());
    if(result != vk::Result::eSuccess)
        throw std::runtime_error("Failed to create buffer for external memory (error: " + vk::to_string(result) + ")");

    // Bind buffer to memory
    mDevice->bindBufferMemory(*mPrimaryBuffer, *mPrimaryMemory, 0);    

    // Update the buffer size now that we know what it is
    this->mSize = bufferInfo.size;
    this->mDataTypeMemorySize = 1;
}
#endif

void
ExternalTensor::recordCopyFrom(UNUSED(const vk::CommandBuffer& commandBuffer), UNUSED(std::shared_ptr<Tensor> copyFromTensor))
{
    throw std::runtime_error("Unsupported operation");
}

void
ExternalTensor::recordCopyFrom(UNUSED(const vk::CommandBuffer& commandBuffer), UNUSED(std::shared_ptr<Image> copyFromImage))
{
    throw std::runtime_error("Unsupported operation");
}

void
ExternalTensor::recordCopyFromStagingToDevice(UNUSED(const vk::CommandBuffer& commandBuffer))
{
    throw std::runtime_error("Unsupported operation");
}

void
ExternalTensor::recordCopyFromDeviceToStaging(UNUSED(const vk::CommandBuffer& commandBuffer))
{
    throw std::runtime_error("Unsupported operation");
}

void
ExternalTensor::recordCopyBuffer(
                         UNUSED(const vk::CommandBuffer& commandBuffer),
                         UNUSED(std::shared_ptr<vk::Buffer> bufferFrom),
                         UNUSED(std::shared_ptr<vk::Buffer> bufferTo),
                         UNUSED(vk::DeviceSize /*bufferSize*/),
                         UNUSED(vk::BufferCopy copyRegion))
{

    throw std::runtime_error("Unsupported operation");
}

void
ExternalTensor::recordCopyBufferFromImage(
                                  UNUSED(const vk::CommandBuffer& commandBuffer),
                                  UNUSED(std::shared_ptr<vk::Image> imageFrom),
                                  UNUSED(std::shared_ptr<vk::Buffer> bufferTo),
                                  UNUSED(vk::ImageLayout fromLayout),
                                  UNUSED(vk::DeviceSize /*bufferSize*/),
                                  UNUSED(vk::BufferImageCopy copyRegion))
{
    throw std::runtime_error("Unsupported operation");
}

void
ExternalTensor::recordPrimaryMemoryBarrier(const vk::CommandBuffer& commandBuffer,
                                   vk::AccessFlagBits srcAccessMask,
                                   vk::AccessFlagBits dstAccessMask,
                                   vk::PipelineStageFlagBits srcStageMask,
                                   vk::PipelineStageFlagBits dstStageMask)
{
    KP_LOG_DEBUG("Kompute ExternalTensor recording PRIMARY buffer memory barrier");

    if(!this->mPrimaryBuffer)
        throw std::runtime_error("Buffer has not been imported");

    this->recordBufferMemoryBarrier(commandBuffer,
                                    *this->mPrimaryBuffer,
                                    srcAccessMask,
                                    dstAccessMask,
                                    srcStageMask,
                                    dstStageMask);
}

void
ExternalTensor::recordStagingMemoryBarrier(
                                   UNUSED(const vk::CommandBuffer& commandBuffer),
                                   UNUSED(vk::AccessFlagBits srcAccessMask),
                                   UNUSED(vk::AccessFlagBits dstAccessMask),
                                   UNUSED(vk::PipelineStageFlagBits srcStageMask),
                                   UNUSED(vk::PipelineStageFlagBits dstStageMask))
{
    throw std::runtime_error("Unsupported operation");
}

void
ExternalTensor::recordBufferMemoryBarrier(const vk::CommandBuffer& commandBuffer,
                                  const vk::Buffer& buffer,
                                  vk::AccessFlagBits srcAccessMask,
                                  vk::AccessFlagBits dstAccessMask,
                                  vk::PipelineStageFlagBits srcStageMask,
                                  vk::PipelineStageFlagBits dstStageMask)
{
    KP_LOG_DEBUG("Kompute ExternalTensor recording buffer memory barrier");

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
ExternalTensor::constructDescriptorBufferInfo()
{
    KP_LOG_DEBUG("Kompute ExternalTensor construct descriptor buffer info size {}",
                 this->memorySize());
    vk::DeviceSize bufferSize = this->memorySize();
    return vk::DescriptorBufferInfo(*this->mPrimaryBuffer,
                                    0, // offset
                                    bufferSize);
}

vk::WriteDescriptorSet
ExternalTensor::constructDescriptorSet(vk::DescriptorSet descriptorSet, uint32_t binding)
{
    KP_LOG_DEBUG("Kompute ExternalTensor construct descriptor set for binding {}", binding);

    if(!this->mPrimaryBuffer)
        throw std::runtime_error("Attempting to use external tensor that has not been imported");

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
ExternalTensor::getPrimaryBufferUsageFlags()
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
            throw std::runtime_error("Kompute ExternalTensor invalid ExternalTensor type");
    }
}

void
ExternalTensor::destroy()
{
    KP_LOG_DEBUG("Kompute ExternalTensor started destroy()");

    if (!this->mDevice) {
        KP_LOG_WARN(
          "Kompute ExternalTensor destructor reached with null Device pointer");
        return;
    }

    if(this->mPrimaryBuffer)
        this->mDevice->destroyBuffer(*this->mPrimaryBuffer);

    if(this->mPrimaryMemory)
        this->mDevice->freeMemory(*this->mPrimaryMemory);

    Memory::destroy();

    KP_LOG_DEBUG("Kompute ExternalTensor successful destroy()");
}
}
