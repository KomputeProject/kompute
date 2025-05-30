// SPDX-License-Identifier: Apache-2.0

#include "kompute/Memory.hpp"
#include "kompute/Image.hpp"
#include "kompute/Tensor.hpp"
#if KOMPUTE_OPT_USE_SPDLOG
#include <spdlog/fmt/fmt.h>
#else
#include <fmt/core.h>
#endif

namespace kp {

Memory::Memory(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
               std::shared_ptr<vk::Device> device,
               const DataTypes& dataType,
               const MemoryTypes& memoryType,
               uint32_t x,
               uint32_t y)
{
    if (x == 0 || y == 0) {
        throw std::runtime_error(
          "Kompute Memory attempted to create a zero-sized memory object");
    }

    this->mPhysicalDevice = physicalDevice;
    this->mDevice = device;
    this->mDataType = dataType;
    this->mMemoryType = memoryType;
    this->mDataTypeMemorySize = Memory::dataTypeMemorySize(dataType);
    this->mX = x;
    this->mY = y;
}

std::string
Memory::toString(Memory::MemoryTypes dt)
{
    switch (dt) {
        case MemoryTypes::eDevice:
            return "eDevice";
        case MemoryTypes::eHost:
            return "eHost";
        case MemoryTypes::eStorage:
            return "eStorage";
        default:
            return "unknown";
    }
}

std::string
Memory::toString(Memory::DataTypes dt)
{
    switch (dt) {
        case DataTypes::eBool:
            return "eBool";
        case DataTypes::eChar:
            return "eChar";
        case DataTypes::eUnsignedChar:
            return "eUnsignedChar";
        case DataTypes::eShort:
            return "eShort";
        case DataTypes::eUnsignedShort:
            return "eUnsignedShort";
        case DataTypes::eInt:
            return "eInt";
        case DataTypes::eUnsignedInt:
            return "eUnsignedInt";
        case DataTypes::eFloat:
            return "eFloat";
        case DataTypes::eDouble:
            return "eDouble";
        default:
            return "unknown";
    }
}

Memory::MemoryTypes
Memory::memoryType()
{
    return this->mMemoryType;
}

uint32_t
Memory::size()
{
    return this->mSize;
}

uint32_t
Memory::dataTypeMemorySize()
{
    return this->mDataTypeMemorySize;
}

uint32_t
Memory::dataTypeMemorySize(DataTypes dt)
{
    switch (dt) {
        case DataTypes::eBool:
            return sizeof(bool);
        case DataTypes::eChar:
            return sizeof(int8_t);
        case DataTypes::eUnsignedChar:
            return sizeof(uint8_t);
        case DataTypes::eShort:
            return sizeof(int16_t);
        case DataTypes::eUnsignedShort:
            return sizeof(uint16_t);
        case DataTypes::eInt:
            return sizeof(int32_t);
        case DataTypes::eUnsignedInt:
            return sizeof(uint32_t);
        case DataTypes::eFloat:
            return sizeof(float);
        case DataTypes::eDouble:
            return sizeof(double);
        default:
            return 0;
    }
}

Memory::DataTypes
Memory::dataType()
{
    return this->mDataType;
}

uint32_t
Memory::memorySize()
{
    return this->mSize * this->mDataTypeMemorySize;
}

void*
Memory::rawData()
{
    if (!this->mRawData) {
        this->mapRawData();
    }
    return this->mRawData;
}

void
Memory::setData(const void* data, size_t size)
{
    if (size != this->memorySize()) {
        throw std::runtime_error(
          "Kompute Memory Cannot set data of different sizes");
    }

    if (!this->mRawData) {
        this->mapRawData();
    }
    memcpy(this->mRawData, data, this->memorySize());
}

void
Memory::mapRawData()
{
    KP_LOG_DEBUG("Kompute Memory mapping data from host buffer");

    std::shared_ptr<vk::DeviceMemory> hostVisibleMemory = nullptr;

    if (this->mMemoryType == MemoryTypes::eHost ||
        this->mMemoryType == MemoryTypes::eDeviceAndHost) {
        hostVisibleMemory = this->mPrimaryMemory;
    } else if (this->mMemoryType == MemoryTypes::eDevice) {
        hostVisibleMemory = this->mStagingMemory;
    } else {
        KP_LOG_WARN("Kompute Memory mapping data not supported on {} memory",
                    Memory::toString(this->memoryType()));
        return;
    }

    vk::DeviceSize size = this->memorySize();

    // Given we request coherent host memory we don't need to invalidate /
    // flush
    this->mRawData = this->mDevice->mapMemory(
      *hostVisibleMemory, 0, size, vk::MemoryMapFlags());

    this->mUnmapMemory = true;
}

void
Memory::unmapRawData()
{
    KP_LOG_DEBUG("Kompute Memory unmapping data from host buffer");
    if (!this->mUnmapMemory) {
        return;
    }

    std::shared_ptr<vk::DeviceMemory> hostVisibleMemory = nullptr;

    if (this->mMemoryType == MemoryTypes::eHost ||
        this->mMemoryType == MemoryTypes::eDeviceAndHost) {
        hostVisibleMemory = this->mPrimaryMemory;
    } else if (this->mMemoryType == MemoryTypes::eDevice) {
        hostVisibleMemory = this->mStagingMemory;
    } else {
        KP_LOG_WARN("Kompute Memory mapping data not supported on {} memory",
                    Memory::toString(this->memoryType()));
        return;
    }

    vk::DeviceSize size = this->memorySize();
    vk::MappedMemoryRange mappedRange(*hostVisibleMemory, 0, size);
    this->mDevice->flushMappedMemoryRanges(1, &mappedRange);
    this->mDevice->unmapMemory(*hostVisibleMemory);

    this->mUnmapMemory = false;
}

void
Memory::updateRawData(void* data)
{
    if (this->memoryType() != Memory::MemoryTypes::eStorage &&
        data != nullptr) {
        this->mapRawData();
        memcpy(this->mRawData, data, this->memorySize());
    }
}

vk::MemoryPropertyFlags
Memory::getPrimaryMemoryPropertyFlags()
{
    switch (this->mMemoryType) {
        case MemoryTypes::eDevice:
            return vk::MemoryPropertyFlagBits::eDeviceLocal;
            break;
        case MemoryTypes::eHost:
            return vk::MemoryPropertyFlagBits::eHostVisible |
                   vk::MemoryPropertyFlagBits::eHostCoherent;
            break;
        case MemoryTypes::eDeviceAndHost:
            return vk::MemoryPropertyFlagBits::eDeviceLocal |
                   vk::MemoryPropertyFlagBits::eHostVisible |
                   vk::MemoryPropertyFlagBits::eHostCoherent;
        case MemoryTypes::eStorage:
            return vk::MemoryPropertyFlagBits::eDeviceLocal;
            break;
        default:
            throw std::runtime_error("Kompute Memory invalid memory type");
    }
}

vk::MemoryPropertyFlags
Memory::getStagingMemoryPropertyFlags()
{
    switch (this->mMemoryType) {
        case MemoryTypes::eDevice:
            return vk::MemoryPropertyFlagBits::eHostVisible |
                   vk::MemoryPropertyFlagBits::eHostCoherent;
            break;
        default:
            throw std::runtime_error("Kompute Memory invalid memory type");
    }
}

void
Memory::recordCopyFrom(const vk::CommandBuffer& commandBuffer,
                       std::shared_ptr<Memory> copyFromMemory)
{
    if (copyFromMemory->dataType() != this->dataType()) {
        throw std::runtime_error(fmt::format(
          "Attempting to copy memory of different types from {} to {}",
          Memory::toString(copyFromMemory->dataType()),
          Memory::toString(this->dataType())));
    }

    if (copyFromMemory->size() != this->size()) {
        throw std::runtime_error(fmt::format(
          "Attempting to copy tensors of different sizes from {} to {}",
          copyFromMemory->size(),
          this->size()));
    }

    if (copyFromMemory->type() == Memory::Type::eTensor) {
        this->recordCopyFrom(commandBuffer,
                             std::static_pointer_cast<Tensor>(copyFromMemory));
    } else if (copyFromMemory->type() == Memory::Type::eImage) {
        this->recordCopyFrom(commandBuffer,
                             std::static_pointer_cast<Image>(copyFromMemory));
    } else {
        throw std::runtime_error("Kompute Memory unsupported memory type");
    }
}

void
Memory::destroy(void)
{
    // Setting raw data to null regardless whether device is available to
    // invalidate Memory
    this->mRawData = nullptr;
    this->mSize = 0;
    this->mDataTypeMemorySize = 0;

    // Unmap the current memory data
    if (this->memoryType() != Memory::MemoryTypes::eStorage) {
        this->unmapRawData();
    }

    if (this->mFreePrimaryMemory) {
        if (!this->mPrimaryMemory) {
            KP_LOG_WARN("Kompose Memory expected to free primary memory but "
                        "got null memory");
        } else {
            KP_LOG_DEBUG("Kompose Memory freeing primary memory");
            this->mDevice->freeMemory(
              *this->mPrimaryMemory,
              (vk::Optional<const vk::AllocationCallbacks>)nullptr);
            this->mPrimaryMemory = nullptr;
            this->mFreePrimaryMemory = false;
        }
    }

    if (this->mFreeStagingMemory) {
        if (!this->mStagingMemory) {
            KP_LOG_WARN("Kompose Memory expected to free staging memory but "
                        "got null memory");
        } else {
            KP_LOG_DEBUG("Kompose Memory freeing staging memory");
            this->mDevice->freeMemory(
              *this->mStagingMemory,
              (vk::Optional<const vk::AllocationCallbacks>)nullptr);
            this->mStagingMemory = nullptr;
            this->mFreeStagingMemory = false;
        }
    }

    if (this->mDevice) {
        this->mDevice = nullptr;
    }
}

} // end namespace kp
