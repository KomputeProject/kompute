
#include "kompute/Tensor.hpp"

namespace kp {

Tensor::Tensor()
{
    SPDLOG_DEBUG("Kompute Tensor base constructor");
    this->mTensorType = TensorTypes::eDevice;
}

Tensor::Tensor(const std::vector<float>& data, TensorTypes tensorType)
{
#if DEBUG
    SPDLOG_DEBUG("Kompute Tensor constructor data length: {}, and type: {}",
                 data.size(),
                 tensorType);
#endif

    this->mData = data;
    this->mShape = { static_cast<uint32_t>(data.size()) };
    this->mTensorType = tensorType;
}

Tensor::~Tensor()
{
    SPDLOG_DEBUG("Kompute Tensor destructor started. Type: {}",
                 this->tensorType());

    if (this->isInit()) {
        this->freeMemoryDestroyGPUResources();
    }

    SPDLOG_DEBUG("Kompute Tensor destructor success");
}

void
Tensor::init(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
             std::shared_ptr<vk::Device> device)
{
    SPDLOG_DEBUG("Kompute Tensor running init with Vulkan params and num data "
                 "elementS: {}",
                 this->mData.size());

    this->mPhysicalDevice = physicalDevice;
    this->mDevice = device;

    this->mIsInit = true;

    this->allocateMemoryCreateGPUResources();
}

std::vector<float>&
Tensor::data()
{
    return this->mData;
}

float&
Tensor::operator[](int index)
{
    return this->mData[index];
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
    return this->mIsInit && this->mPrimaryBuffer && this->mPrimaryMemory;
}

void
Tensor::setData(const std::vector<float>& data)
{
    if (data.size() != this->mData.size()) {
        throw std::runtime_error(
          "Kompute Tensor Cannot set data of different sizes");
    }
    this->mData = data;
}

void
Tensor::recordCopyFrom(std::shared_ptr<vk::CommandBuffer> commandBuffer,
                       std::shared_ptr<Tensor> copyFromTensor,
                       bool createBarrier)
{

    vk::DeviceSize bufferSize(this->memorySize());
    vk::BufferCopy copyRegion(0, 0, bufferSize);

    SPDLOG_DEBUG("Kompute Tensor recordCopyFrom data size {}.", bufferSize);

    this->copyBuffer(commandBuffer,
                     copyFromTensor->mPrimaryBuffer,
                     this->mPrimaryBuffer,
                     bufferSize,
                     copyRegion,
                     createBarrier);
}

void
Tensor::recordCopyFromStagingToDevice(
  std::shared_ptr<vk::CommandBuffer> commandBuffer,
  bool createBarrier)
{
    vk::DeviceSize bufferSize(this->memorySize());
    vk::BufferCopy copyRegion(0, 0, bufferSize);

    SPDLOG_DEBUG("Kompute Tensor copying data size {}.", bufferSize);

    this->copyBuffer(commandBuffer,
                     this->mStagingBuffer,
                     this->mPrimaryBuffer,
                     bufferSize,
                     copyRegion,
                     createBarrier);
}

void
Tensor::recordCopyFromDeviceToStaging(
  std::shared_ptr<vk::CommandBuffer> commandBuffer,
  bool createBarrier)
{
    vk::DeviceSize bufferSize(this->memorySize());
    vk::BufferCopy copyRegion(0, 0, bufferSize);

    SPDLOG_DEBUG("Kompute Tensor copying data size {}.", bufferSize);

    this->copyBuffer(commandBuffer,
                     this->mPrimaryBuffer,
                     this->mStagingBuffer,
                     bufferSize,
                     copyRegion,
                     createBarrier);
}

void
Tensor::copyBuffer(std::shared_ptr<vk::CommandBuffer> commandBuffer,
                   std::shared_ptr<vk::Buffer> bufferFrom,
                   std::shared_ptr<vk::Buffer> bufferTo,
                   vk::DeviceSize bufferSize,
                   vk::BufferCopy copyRegion,
                   bool createBarrier)
{

    if (!this->mIsInit) {
        throw std::runtime_error(
          "Kompute Tensor attempted to run copyBuffer without init");
    }

    commandBuffer->copyBuffer(*bufferFrom, *bufferTo, copyRegion);

    if (createBarrier) {
        // Buffer to ensure wait until data is copied to staging buffer
        this->recordBufferMemoryBarrier(commandBuffer,
                                        vk::AccessFlagBits::eTransferWrite,
                                        vk::AccessFlagBits::eHostRead,
                                        vk::PipelineStageFlagBits::eTransfer,
                                        vk::PipelineStageFlagBits::eHost);
    }
}

void
Tensor::recordBufferMemoryBarrier(
  std::shared_ptr<vk::CommandBuffer> commandBuffer,
  vk::AccessFlagBits srcAccessMask,
  vk::AccessFlagBits dstAccessMask,
  vk::PipelineStageFlagBits srcStageMask,
  vk::PipelineStageFlagBits dstStageMask)
{
    SPDLOG_DEBUG("Kompute Tensor recording buffer memory barrier");

    vk::DeviceSize bufferSize = this->memorySize();

    vk::BufferMemoryBarrier bufferMemoryBarrier;
    bufferMemoryBarrier.buffer = *this->mPrimaryBuffer;
    bufferMemoryBarrier.size = bufferSize;
    bufferMemoryBarrier.srcAccessMask = srcAccessMask;
    bufferMemoryBarrier.dstAccessMask = dstAccessMask;
    bufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    bufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    commandBuffer->pipelineBarrier(srcStageMask,
                                   dstStageMask,
                                   vk::DependencyFlags(),
                                   nullptr,
                                   bufferMemoryBarrier,
                                   nullptr);
}

vk::DescriptorBufferInfo
Tensor::constructDescriptorBufferInfo()
{
    vk::DeviceSize bufferSize = this->memorySize();
    return vk::DescriptorBufferInfo(*this->mPrimaryBuffer,
                                    0, // offset
                                    bufferSize);
}

void
Tensor::mapDataFromHostMemory()
{
    SPDLOG_DEBUG("Kompute Tensor mapping data from host buffer");

    std::shared_ptr<vk::DeviceMemory> hostVisibleMemory = nullptr;

    if (this->mTensorType == TensorTypes::eHost) {
        hostVisibleMemory = this->mPrimaryMemory;
    } else {
        hostVisibleMemory = this->mStagingMemory;
    }

    vk::DeviceSize bufferSize = this->memorySize();
    void* mapped = this->mDevice->mapMemory(
      *hostVisibleMemory, 0, bufferSize, vk::MemoryMapFlags());
    vk::MappedMemoryRange mappedMemoryRange(*hostVisibleMemory, 0, bufferSize);
    this->mDevice->invalidateMappedMemoryRanges(mappedMemoryRange);
    memcpy(this->mData.data(), mapped, bufferSize);
    this->mDevice->unmapMemory(*hostVisibleMemory);
}

void
Tensor::mapDataIntoHostMemory()
{

    SPDLOG_DEBUG("Kompute Tensor local mapping tensor data to host buffer");

    std::shared_ptr<vk::DeviceMemory> hostVisibleMemory = nullptr;

    if (this->mTensorType == TensorTypes::eHost) {
        hostVisibleMemory = this->mPrimaryMemory;
    } else {
        hostVisibleMemory = this->mStagingMemory;
    }

    vk::DeviceSize bufferSize = this->memorySize();

    void* mapped = this->mDevice->mapMemory(
      *hostVisibleMemory, 0, bufferSize, vk::MemoryMapFlags());
    memcpy(mapped, this->mData.data(), bufferSize);
    vk::MappedMemoryRange mappedRange(*hostVisibleMemory, 0, bufferSize);
    this->mDevice->flushMappedMemoryRanges(1, &mappedRange);
    this->mDevice->unmapMemory(*hostVisibleMemory);
}

vk::BufferUsageFlags
Tensor::getPrimaryBufferUsageFlags()
{
    switch (this->mTensorType) {
        case TensorTypes::eDevice:
            return vk::BufferUsageFlagBits::eStorageBuffer |
                   vk::BufferUsageFlagBits::eTransferSrc |
                   vk::BufferUsageFlagBits::eTransferDst;
            break;
        case TensorTypes::eHost:
            return vk::BufferUsageFlagBits::eStorageBuffer |
                   vk::BufferUsageFlagBits::eTransferSrc |
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
Tensor::getPrimaryMemoryPropertyFlags()
{
    switch (this->mTensorType) {
        case TensorTypes::eDevice:
            return vk::MemoryPropertyFlagBits::eDeviceLocal;
            break;
        case TensorTypes::eHost:
            return vk::MemoryPropertyFlagBits::eHostVisible;
            break;
        case TensorTypes::eStorage:
            return vk::MemoryPropertyFlagBits::eDeviceLocal;
            break;
        default:
            throw std::runtime_error("Kompute Tensor invalid tensor type");
    }
}

vk::BufferUsageFlags
Tensor::getStagingBufferUsageFlags()
{
    switch (this->mTensorType) {
        case TensorTypes::eDevice:
            return vk::BufferUsageFlagBits::eTransferSrc |
                   vk::BufferUsageFlagBits::eTransferDst;
            break;
        default:
            throw std::runtime_error("Kompute Tensor invalid tensor type");
    }
}

vk::MemoryPropertyFlags
Tensor::getStagingMemoryPropertyFlags()
{
    switch (this->mTensorType) {
        case TensorTypes::eDevice:
            return vk::MemoryPropertyFlagBits::eHostVisible;
            break;
        default:
            throw std::runtime_error("Kompute Tensor invalid tensor type");
    }
}

void
Tensor::allocateMemoryCreateGPUResources()
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

    SPDLOG_DEBUG("Kompute Tensor creating primary buffer and memory");

    this->mPrimaryBuffer = std::make_shared<vk::Buffer>();
    this->createBuffer(this->mPrimaryBuffer,
                       this->getPrimaryBufferUsageFlags());
    this->mFreePrimaryBuffer = true;
    this->mPrimaryMemory = std::make_shared<vk::DeviceMemory>();
    this->allocateBindMemory(this->mPrimaryBuffer,
                             this->mPrimaryMemory,
                             this->getPrimaryMemoryPropertyFlags());
    this->mFreePrimaryMemory = true;

    if (this->mTensorType == TensorTypes::eDevice) {
        SPDLOG_DEBUG("Kompute Tensor creating staging buffer and memory");

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

    SPDLOG_DEBUG("Kompute Tensor buffer & memory creation successful");
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

    SPDLOG_DEBUG("Kompute Tensor creating buffer with memory size: {}, and "
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

    SPDLOG_DEBUG("Kompute Tensor allocating and binding memory");

    vk::PhysicalDeviceMemoryProperties memoryProperties =
      this->mPhysicalDevice->getMemoryProperties();

    vk::MemoryRequirements memoryRequirements =
      this->mDevice->getBufferMemoryRequirements(*buffer);

    uint32_t memoryTypeIndex = -1;
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
        if (memoryRequirements.memoryTypeBits & (1 << i)) {
            if (((memoryProperties.memoryTypes[i]).propertyFlags &
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

    SPDLOG_DEBUG(
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
Tensor::freeMemoryDestroyGPUResources()
{
    SPDLOG_DEBUG("Kompute Tensor started freeMemoryDestroyGPUResources");

    this->mIsInit = false;

    if (!this->mDevice) {
        SPDLOG_ERROR(
          "Kompute Tensor destructor reached with null Device pointer");
        return;
    }

    if (this->mFreePrimaryBuffer) {
        if (!this->mPrimaryBuffer) {
            SPDLOG_ERROR("Kompose Tensor expected to destroy primary buffer "
                         "but got null buffer");
        } else {
            SPDLOG_DEBUG("Kompose Tensor destroying primary buffer");
            this->mDevice->destroy(
              *this->mPrimaryBuffer,
              (vk::Optional<const vk::AllocationCallbacks>)nullptr);
            this->mPrimaryBuffer = nullptr;
        }
    }

    if (this->mFreeStagingBuffer) {
        if (!this->mStagingBuffer) {
            SPDLOG_ERROR("Kompose Tensor expected to destroy staging buffer "
                         "but got null buffer");
        } else {
            SPDLOG_DEBUG("Kompose Tensor destroying staging buffer");
            this->mDevice->destroy(
              *this->mStagingBuffer,
              (vk::Optional<const vk::AllocationCallbacks>)nullptr);
            this->mStagingBuffer = nullptr;
        }
    }

    if (this->mFreePrimaryMemory) {
        if (!this->mPrimaryMemory) {
            SPDLOG_ERROR("Kompose Tensor expected to free primary memory but "
                         "got null memory");
        } else {
            SPDLOG_DEBUG("Kompose Tensor freeing primary memory");
            this->mDevice->freeMemory(
              *this->mPrimaryMemory,
              (vk::Optional<const vk::AllocationCallbacks>)nullptr);
            this->mPrimaryMemory = nullptr;
        }
    }

    if (this->mFreeStagingMemory) {
        if (!this->mStagingMemory) {
            SPDLOG_ERROR("Kompose Tensor expected to free staging memory but "
                         "got null memory");
        } else {
            SPDLOG_DEBUG("Kompose Tensor freeing staging memory");
            this->mDevice->freeMemory(
              *this->mStagingMemory,
              (vk::Optional<const vk::AllocationCallbacks>)nullptr);
            this->mStagingMemory = nullptr;
        }
    }

    SPDLOG_DEBUG("Kompute Tensor successful freeMemoryDestroyGPUResources");
}

}
