// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "kompute/Core.hpp"
#include "kompute/Memory.hpp"
#include "logger/Logger.hpp"
#include <memory>
#include <string>

namespace kp {

// Forward-declare the Image class
class Image;
/**
 * Structured data used in GPU operations.
 *
 * Tensors are the base building block in Kompute to perform operations across
 * GPUs. Each tensor would have a respective Vulkan memory and buffer, which
 * would be used to store their respective data. The tensors can be used for GPU
 * data storage or transfer.
 */
class Tensor : public Memory
{
  public:
    /**
     *  Constructor with data provided which would be used to create the
     * respective vulkan buffer and memory.
     *
     *  @param physicalDevice The physical device to use to fetch properties
     *  @param device The device to use to create the buffer and memory from
     *  @param data Non-zero-sized vector of data that will be used by the
     * tensor
     *  @param tensorTypes Type for the tensor which is of type MemoryTypes
     */
    Tensor(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
           std::shared_ptr<vk::Device> device,
           void* data,
           uint32_t elementTotalCount,
           uint32_t elementMemorySize,
           const DataTypes& dataType,
           const MemoryTypes& tensorType = MemoryTypes::eDevice);

    /**
     *  Constructor with size provided which would be used to create the
     * respective vulkan buffer and memory. Data is not copied.
     *
     *  @param physicalDevice The physical device to use to fetch properties
     *  @param device The device to use to create the buffer and memory from
     *  @param elmentTotalCount the number of elements of the array
     *  @param elementMemorySize the size of the element
     *  @param tensorTypes Type for the tensor which is of type TensorTypes
     */
    Tensor(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
           std::shared_ptr<vk::Device> device,
           uint32_t elementTotalCount,
           uint32_t elementMemorySize,
           const DataTypes& dataType,
           const MemoryTypes& memoryType = MemoryTypes::eDevice);

    /**
     * Destructor which is in charge of freeing vulkan resources unless they
     * have been provided externally.
     */
    virtual ~Tensor();

    /**
     * Destroys and frees the GPU resources which include the buffer and memory.
     */
    void destroy() override;

    /**
     * Check whether tensor is initialized based on the created gpu resources.
     *
     * @returns Boolean stating whether tensor is initialized
     */
    bool isInit() override;

    /**
     * Records a copy from the memory of the tensor provided to the current
     * tensor. This is intended to pass memory into a processing, to perform
     * a staging buffer transfer, or to gather output (between others).
     *
     * @param commandBuffer Vulkan Command Buffer to record the commands into
     * @param copyFromTensor Tensor to copy the data from
     */
    void recordCopyFrom(const vk::CommandBuffer& commandBuffer,
                        std::shared_ptr<Tensor> copyFromTensor) override;

    /**
     * Records a copy from the memory of the image provided to the current
     * tensor. This is intended to pass memory into a processing, to perform
     * a staging buffer transfer, or to gather output (between others).
     *
     * @param commandBuffer Vulkan Command Buffer to record the commands into
     * @param copyFromImage Image to copy the data from
     */
    void recordCopyFrom(const vk::CommandBuffer& commandBuffer,
                        std::shared_ptr<Image> copyFromImage) override;

    /**
     * Records a copy from the internal staging memory to the device memory
     * using an optional barrier to wait for the operation. This function would
     * only be relevant for kp::Tensors of type eDevice.
     *
     * @param commandBuffer Vulkan Command Buffer to record the commands into
     */
    void recordCopyFromStagingToDevice(
      const vk::CommandBuffer& commandBuffer) override;

    /**
     * Records a copy from the internal device memory to the staging memory
     * using an optional barrier to wait for the operation. This function would
     * only be relevant for kp::Tensors of type eDevice.
     *
     * @param commandBuffer Vulkan Command Buffer to record the commands into
     */
    void recordCopyFromDeviceToStaging(
      const vk::CommandBuffer& commandBuffer) override;

    /**
     * Records the memory barrier into the primary buffer and command
     * buffer which ensures that relevant data transfers are carried out
     * correctly.
     *
     * @param commandBuffer Vulkan Command Buffer to record the commands into
     * @param srcAccessMask Access flags for source access mask
     * @param dstAccessMask Access flags for destination access mask
     * @param scrStageMask Pipeline stage flags for source stage mask
     * @param dstStageMask Pipeline stage flags for destination stage mask
     */
    void recordPrimaryMemoryBarrier(
      const vk::CommandBuffer& commandBuffer,
      vk::AccessFlagBits srcAccessMask,
      vk::AccessFlagBits dstAccessMask,
      vk::PipelineStageFlagBits srcStageMask,
      vk::PipelineStageFlagBits dstStageMask) override;
    /**
     * Records the memory barrier into the staging buffer and command
     * buffer which ensures that relevant data transfers are carried out
     * correctly.
     *
     * @param commandBuffer Vulkan Command Buffer to record the commands into
     * @param srcAccessMask Access flags for source access mask
     * @param dstAccessMask Access flags for destination access mask
     * @param scrStageMask Pipeline stage flags for source stage mask
     * @param dstStageMask Pipeline stage flags for destination stage mask
     */
    void recordStagingMemoryBarrier(
      const vk::CommandBuffer& commandBuffer,
      vk::AccessFlagBits srcAccessMask,
      vk::AccessFlagBits dstAccessMask,
      vk::PipelineStageFlagBits srcStageMask,
      vk::PipelineStageFlagBits dstStageMask) override;

    /**
     * Adds this object to a Vulkan descriptor set at \p binding.
     *
     * @param descriptorSet The descriptor set to add to.
     * @param binding The binding number to use.
     * @return Add this object to a descriptor set at \p binding.
     */
    vk::WriteDescriptorSet constructDescriptorSet(
      vk::DescriptorSet descriptorSet,
      uint32_t binding) override;

    std::shared_ptr<vk::Buffer> getPrimaryBuffer();

    Type type() override { return Type::eTensor; }

  protected:
    // -------------- ALWAYS OWNED RESOURCES
    vk::DescriptorBufferInfo mDescriptorBufferInfo;

  private:
    // -------------- OPTIONALLY OWNED RESOURCES
    std::shared_ptr<vk::Buffer> mPrimaryBuffer;
    bool mFreePrimaryBuffer = false;
    std::shared_ptr<vk::Buffer> mStagingBuffer;
    bool mFreeStagingBuffer = false;

    void allocateMemoryCreateGPUResources(); // Creates the vulkan buffer
    void createBuffer(std::shared_ptr<vk::Buffer> buffer,
                      vk::BufferUsageFlags bufferUsageFlags);
    void allocateBindMemory(std::shared_ptr<vk::Buffer> buffer,
                            std::shared_ptr<vk::DeviceMemory> memory,
                            vk::MemoryPropertyFlags memoryPropertyFlags);
    void recordCopyBuffer(const vk::CommandBuffer& commandBuffer,
                          std::shared_ptr<vk::Buffer> bufferFrom,
                          std::shared_ptr<vk::Buffer> bufferTo,
                          vk::DeviceSize bufferSize,
                          vk::BufferCopy copyRegion);
    void recordCopyBufferFromImage(const vk::CommandBuffer& commandBuffer,
                                   std::shared_ptr<vk::Image> imageFrom,
                                   std::shared_ptr<vk::Buffer> bufferTo,
                                   vk::ImageLayout fromLayout,
                                   vk::DeviceSize /*bufferSize*/,
                                   vk::BufferImageCopy copyRegion);
    void recordBufferMemoryBarrier(const vk::CommandBuffer& commandBuffer,
                                   const vk::Buffer& buffer,
                                   vk::AccessFlagBits srcAccessMask,
                                   vk::AccessFlagBits dstAccessMask,
                                   vk::PipelineStageFlagBits srcStageMask,
                                   vk::PipelineStageFlagBits dstStageMask);

    // Private util functions
    vk::BufferUsageFlags getPrimaryBufferUsageFlags();
    vk::BufferUsageFlags getStagingBufferUsageFlags();

    vk::DescriptorBufferInfo constructDescriptorBufferInfo();

    /**
     * Function to reserve memory on the tensor. This does not copy any data, it
     * just reserves memory, similarly to std::vector reserve() method.
     */
    void reserve();
};

template<typename T>
class TensorT : public Tensor
{
  public:
    TensorT(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
            std::shared_ptr<vk::Device> device,
            const size_t size,
            const MemoryTypes& tensorType = MemoryTypes::eDevice)
      : Tensor(physicalDevice,
               device,
               size,
               sizeof(T),
               Memory::dataType<T>(),
               tensorType)
    {
        KP_LOG_DEBUG("Kompute TensorT constructor with data size {}", size);
    }

    TensorT(
      std::shared_ptr<vk::PhysicalDevice> physicalDevice,
      std::shared_ptr<vk::Device> device,
      const std::vector<T>& data,
      const Memory::MemoryTypes& tensorType = Memory::MemoryTypes::eDevice)
      : Tensor(physicalDevice,
               device,
               (void*)data.data(),
               static_cast<uint32_t>(data.size()),
               sizeof(T),
               Memory::dataType<T>(),
               tensorType)
    {
        KP_LOG_DEBUG("Kompute TensorT filling constructor with data size {}",
                     data.size());
    }

    ~TensorT() { KP_LOG_DEBUG("Kompute TensorT destructor"); }

    DataTypes dataType() { return Memory::dataType<T>(); }
    std::vector<T> vector() { return Memory::vector<T>(); }
    T* data() { return Memory::data<T>(); }
};

} // End namespace kp
