// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "kompute/Core.hpp"
#include "logger/Logger.hpp"
#include <memory>
#include <string>

namespace kp {

// Forward declare the Tensor and Image classes
class Tensor;
class Image;

class Memory
{
    // This is the base class for Tensors and Images.
    // It's required so that algorithms and sequences can mix tensors and
    // images.
  public:
    /**
     * Type for memory created: Device allows memory to be transferred from
     * staging memory. Staging are host memory visible. Storage are device
     * visible but are not set up to transfer or receive data (only for shader
     * storage).
     */
    enum class MemoryTypes
    {
        eDevice = 0,  ///< Type is device memory, source and destination
        eHost = 1,    ///< Type is host memory, source and destination
        eStorage = 2, ///< Type is Device memory (only)
        eDeviceAndHost =
          3, ///< Type is host-visible and host-coherent device memory
    };

    enum class DataTypes
    {
        eBool = 0,
        eInt = 1,
        eUnsignedInt = 2,
        eFloat = 3,
        eDouble = 4,
        eCustom = 5,
        eShort = 6,
        eUnsignedShort = 7,
        eChar = 8,
        eUnsignedChar = 9
    };

    enum class Type
    {
        eTensor = 0,
        eImage = 1
    };

    static std::string toString(MemoryTypes dt);
    static std::string toString(DataTypes dt);

    Memory(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
           std::shared_ptr<vk::Device> device,
           const DataTypes& dataType,
           const MemoryTypes& memoryType,
           uint32_t x,
           uint32_t y);

    /**
     * Destructor which is in charge of freeing vulkan resources unless they
     * have been provided externally.
     */
    virtual ~Memory(){};

    /**
     * Destroys and frees the GPU resources which includes the memory.
     */
    virtual void destroy();

    /**
     * Retrieve the memory type of the memory
     *
     * @return memory type of memory
     */
    MemoryTypes memoryType();

    /**
     * Retrieve the data type for T
     *
     * @return Data type of T of type kp::Memory::DataTypes
     */
    template<typename T>
    static constexpr DataTypes dataType();

    /**
     * Retrieve the data type of the tensor (int, float, unsigned short etc.)
     *
     * @return Data type of memory of type kp::Memory::DataTypes
     */
    DataTypes dataType();

    /**
     * Check whether memory object is initialized based on the created gpu
     * resources.
     *
     * @returns Boolean stating whether memory object is initialized
     */
    virtual bool isInit() = 0;

    /**
     * Records a copy from the internal staging memory to the device memory
     * using an optional barrier to wait for the operation. This function would
     * only be relevant for kp::Tensors of type eDevice.
     *
     * @param commandBuffer Vulkan Command Buffer to record the commands into
     */
    virtual void recordCopyFromStagingToDevice(
      const vk::CommandBuffer& commandBuffer) = 0;

    /**
     * Records a copy from the internal device memory to the staging memory
     * using an optional barrier to wait for the operation. This function would
     * only be relevant for kp::Tensors of type eDevice.
     *
     * @param commandBuffer Vulkan Command Buffer to record the commands into
     */
    virtual void recordCopyFromDeviceToStaging(
      const vk::CommandBuffer& commandBuffer) = 0;
    /**
     * Records the buffer memory barrier into the primary buffer and command
     * buffer which ensures that relevant data transfers are carried out
     * correctly.
     *
     * @param commandBuffer Vulkan Command Buffer to record the commands into
     * @param srcAccessMask Access flags for source access mask
     * @param dstAccessMask Access flags for destination access mask
     * @param scrStageMask Pipeline stage flags for source stage mask
     * @param dstStageMask Pipeline stage flags for destination stage mask
     */
    virtual void recordPrimaryMemoryBarrier(
      const vk::CommandBuffer& commandBuffer,
      vk::AccessFlagBits srcAccessMask,
      vk::AccessFlagBits dstAccessMask,
      vk::PipelineStageFlagBits srcStageMask,
      vk::PipelineStageFlagBits dstStageMask) = 0;
    /**
     * Records the buffer memory barrier into the staging buffer and command
     * buffer which ensures that relevant data transfers are carried out
     * correctly.
     *
     * @param commandBuffer Vulkan Command Buffer to record the commands into
     * @param srcAccessMask Access flags for source access mask
     * @param dstAccessMask Access flags for destination access mask
     * @param scrStageMask Pipeline stage flags for source stage mask
     * @param dstStageMask Pipeline stage flags for destination stage mask
     */
    virtual void recordStagingMemoryBarrier(
      const vk::CommandBuffer& commandBuffer,
      vk::AccessFlagBits srcAccessMask,
      vk::AccessFlagBits dstAccessMask,
      vk::PipelineStageFlagBits srcStageMask,
      vk::PipelineStageFlagBits dstStageMask) = 0;

    /**
     * Records a copy from the memory provided to the current
     * memory. This is intended to pass memory into a processing, to perform
     * a staging image transfer, or to gather output (between others).
     *
     * @param commandBuffer Vulkan Command Buffer to record the commands into
     * @param copyFromMemory Memory to copy the data from
     */
    void recordCopyFrom(const vk::CommandBuffer& commandBuffer,
                        std::shared_ptr<Memory> copyFromMemory);

    /**
     * Adds this object to a Vulkan descriptor set at \p binding.
     *
     * @param descriptorSet The descriptor set to add to.
     * @param binding The binding number to use.
     * @return Add this object to a descriptor set at \p binding.
     */
    virtual vk::WriteDescriptorSet constructDescriptorSet(
      vk::DescriptorSet descriptorSet,
      uint32_t binding) = 0;

    /**
     * Returns the size/magnitude of the Tensor/Image, which will be the total
     * number of elements across all dimensions
     *
     * @return Unsigned integer representing the total number of elements
     */
    uint32_t size();

    /**
     * Returns the total size of a single element of the respective data type
     * that this memory object holds.
     *
     * @return Unsigned integer representing the memory of a single element of
     * the respective data type.
     */
    uint32_t dataTypeMemorySize();

    /**
     * Returns the total size of a single element of the respective data type
     *
     * @return Unsigned integer representing the memory of a single element of
     * the respective data type.
     */
    static uint32_t dataTypeMemorySize(DataTypes dataType);

    /**
     * Returns the total memory size of the data contained by the memory object
     * which would equate to (this->size() * this->dataTypeMemorySize())
     *
     * @return Unsigned integer representing the total memory size of the data
     * contained by the image object.
     */
    uint32_t memorySize();

    vk::DescriptorType getDescriptorType() { return mDescriptorType; }

    /**
     * Retrieve the raw data via the pointer to the memory that contains the raw
     * memory of this current tensor. This tensor gets changed to a nullptr when
     * the Tensor is removed.
     *
     * @return Pointer to raw memory containing raw bytes data of Tensor/Image.
     */
    void* rawData();

    /**
     * Sets / resets the data of the tensor/image which is directly done on the
     * GPU host visible memory available by the tensor/image.
     */
    void setData(const void* data, size_t size);

    /**
     * Sets / resets the data of the tensor/image which is directly done on the
     * GPU host visible memory available by the tensor/image.
     */
    template<typename T>
    void setData(const std::vector<T>& data)
    {
        KP_LOG_DEBUG("Kompute Memory setting data with data size {}",
                     data.size() * sizeof(T));

        this->setData(data.data(), data.size() * sizeof(T));
    }

    /**
     * Template to return the pointer data converted by specific type, which
     * would be any of the supported types including float, double, int32,
     * uint32 and bool.
     *
     * @return Pointer to raw memory containing raw bytes data of Tensor/Image.
     */
    template<typename T>
    T* data()
    {
        if (this->mRawData == nullptr) {
            this->mapRawData();
        }

        return (T*)this->mRawData;
    }

    /**
     * Template to get the data of the current tensor/image as a vector of
     * specific type, which would be any of the supported types including float,
     * double, int32, uint32 and bool.
     *
     * @return Vector of type provided by template.
     */
    template<typename T>
    std::vector<T> vector()
    {
        if (this->mRawData == nullptr) {
            this->mapRawData();
        }

        return { (T*)this->mRawData, ((T*)this->mRawData) + this->size() };
    }

    /***
     * Retreive the size of the x-dimension of the memory
     *
     * @return Size of the x-dimension of the memory
     */
    uint32_t getX() { return this->mX; }

    /***
     * Retreive the size of the y-dimension of the memory
     *
     * @return Size of the y-dimension of the memory
     */
    uint32_t getY() { return this->mY; };

    /**
     * Return the object type of this Memory object.
     *
     * @return The object type of the Memory object.
     */
    virtual Type type() = 0;

  protected:
    // -------------- ALWAYS OWNED RESOURCES
    MemoryTypes mMemoryType;
    DataTypes mDataType;
    uint32_t mSize;
    uint32_t mDataTypeMemorySize;
    void* mRawData = nullptr;
    vk::DescriptorType mDescriptorType;
    bool mUnmapMemory = false;
    uint32_t mX;
    uint32_t mY;

    // -------------- NEVER OWNED RESOURCES
    std::shared_ptr<vk::PhysicalDevice> mPhysicalDevice;
    std::shared_ptr<vk::Device> mDevice;

    // -------------- OPTIONALLY OWNED RESOURCES
    std::shared_ptr<vk::DeviceMemory> mPrimaryMemory;
    bool mFreePrimaryMemory = false;
    std::shared_ptr<vk::DeviceMemory> mStagingMemory;
    bool mFreeStagingMemory = false;

    // Private util functions
    void mapRawData();
    void unmapRawData();
    void updateRawData(void* data);
    vk::MemoryPropertyFlags getPrimaryMemoryPropertyFlags();
    vk::MemoryPropertyFlags getStagingMemoryPropertyFlags();

    virtual void recordCopyFrom(const vk::CommandBuffer& commandBuffer,
                                std::shared_ptr<Tensor> copyFromMemory) = 0;
    virtual void recordCopyFrom(const vk::CommandBuffer& commandBuffer,
                                std::shared_ptr<Image> copyFromMemory) = 0;
};

template<>
constexpr Memory::DataTypes
Memory::dataType<bool>()
{
    return DataTypes::eBool;
}

template<>
constexpr Memory::DataTypes
Memory::dataType<int8_t>()
{
    return DataTypes::eChar;
}

template<>
constexpr Memory::DataTypes
Memory::dataType<uint8_t>()
{
    return DataTypes::eUnsignedChar;
}

template<>
constexpr Memory::DataTypes
Memory::dataType<int16_t>()
{
    return DataTypes::eShort;
}

template<>
constexpr Memory::DataTypes
Memory::dataType<uint16_t>()
{
    return DataTypes::eUnsignedShort;
}

template<>
constexpr Memory::DataTypes
Memory::dataType<int32_t>()
{
    return DataTypes::eInt;
}

template<>
constexpr Memory::DataTypes
Memory::dataType<uint32_t>()
{
    return DataTypes::eUnsignedInt;
}

template<>
constexpr Memory::DataTypes
Memory::dataType<float>()
{
    return DataTypes::eFloat;
}

template<>
constexpr Memory::DataTypes
Memory::dataType<double>()
{
    return DataTypes::eDouble;
}

} // End namespace kp
