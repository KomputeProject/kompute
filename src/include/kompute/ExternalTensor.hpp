// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "kompute/Core.hpp"
#include "kompute/Memory.hpp"
#include "logger/Logger.hpp"
#include <memory>
#include <string>

#ifdef ANDROID
  #include <android/hardware_buffer.h>
#endif

namespace kp {

class Image;

/**
 * Structured data used in GPU operations.
 *
 * External tensors are storage buffers that are imported
 */
class ExternalTensor : public Memory
{
  public:
    /**
     *  Constructor with size provided which would be used to create the
     * respective vulkan buffer and memory. Data is not copied.
     *
     *  @param physicalDevice The physical device to use to fetch properties
     *  @param device The device to use to create the buffer and memory from
     */
    ExternalTensor(
      std::shared_ptr<vk::PhysicalDevice> physicalDevice,
      std::shared_ptr<vk::Device> device);

    /**
     * @brief Make ExternalTensor uncopyable
     *
     */
    ExternalTensor(const ExternalTensor&) = delete;
    ExternalTensor(const ExternalTensor&&) = delete;
    ExternalTensor& operator=(const ExternalTensor&) = delete;
    ExternalTensor& operator=(const ExternalTensor&&) = delete;

    /**
     * Destructor which is in charge of freeing vulkan resources unless they
     * have been provided externally.
     */
    virtual ~ExternalTensor();

    /**
     * Destroys and frees the GPU resources which include the buffer and memory.
     */
    void destroy() override;

    /**
     * Check whether ExternalTensor is initialized based on the created gpu resources.
     *
     * @returns Boolean stating whether ExternalTensor is initialized
     */
    bool isInit() override;

    /**
     * This is an externally managed tensor.
     */
    bool isExternal() const override {
        return true;
    }

  #ifdef ANDROID
    void import(AHardwareBuffer* hardwareBuffer);
  #endif

    /**
     * Records a copy from the memory of the ExternalTensor provided to the current
     * ExternalTensor. This is intended to pass memory into a processing, to perform
     * a staging buffer transfer, or to gather output (between others).
     *
     * @param commandBuffer Vulkan Command Buffer to record the commands into
     * @param copyFromTensor ExternalTensor to copy the data from
     */
    void recordCopyFrom(const vk::CommandBuffer& commandBuffer,
                        std::shared_ptr<Tensor> copyFromTensor) override;

    /**
     * Records a copy from the memory of the image provided to the current
     * ExternalTensor. This is intended to pass memory into a processing, to perform
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

    Type type() override { return Type::eTensor; }

  protected:
    // -------------- ALWAYS OWNED RESOURCES
    vk::DescriptorBufferInfo mDescriptorBufferInfo;

  private:
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
    vk::DescriptorBufferInfo constructDescriptorBufferInfo();

    std::unique_ptr<vk::Buffer> mPrimaryBuffer;
};

} // End namespace kp
