// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "kompute/Core.hpp"
#include "kompute/Memory.hpp"
#include "kompute/Tensor.hpp"
#include "logger/Logger.hpp"
#include <memory>
#include <string>

namespace kp {

/**
 * Image data used in GPU operations.
 *
 * Each image would has a respective Vulkan memory and image, which
 * would be used to store their respective data. The images can be used for GPU
 * data storage or transfer.
 */
class Image : public Memory
{
  public:
    /**
     *  Constructor with data provided which would be used to create the
     *  respective vulkan image and memory.
     *
     *  @param physicalDevice The physical device to use to fetch properties
     *  @param device The device to use to create the image and memory from
     *  @param data Pointer to data that will be used to initialise the image
     *  @param dataSize Size in bytes of the data pointed to by \p data
     *  @param x Width of the image in pixels
     *  @param y Height of the image in pixels
     *  @param numChannels The number of channels in the image
     *  @param dataType Data type for the image which is of type DataTypes
     *  @param memoryType Type for the image which is of type MemoryTypes
     *  @param tiling Tiling mode to use for the image.
     */
    Image(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
          std::shared_ptr<vk::Device> device,
          void* data,
          size_t dataSize,
          uint32_t x,
          uint32_t y,
          uint32_t numChannels,
          const DataTypes& dataType,
          vk::ImageTiling tiling,
          const MemoryTypes& memoryType = MemoryTypes::eDevice)
      : Memory(physicalDevice, device, dataType, memoryType, x, y)
    {
        if (dataType == DataTypes::eCustom) {
            throw std::runtime_error(
              "Custom data types are not supported for Kompute Images");
        }

        init(data, dataSize, numChannels, tiling);
    }

    /**
     *  Constructor with no data provided.
     *
     *  @param physicalDevice The physical device to use to fetch properties
     *  @param device The device to use to create the image and memory from
     *  @param x Width of the image in pixels
     *  @param y Height of the image in pixels
     *  @param dataType Data type for the image which is of type ImageDataTypes
     *  @param memoryType Type for the image which is of type MemoryTypes
     *  @param tiling Tiling mode to use for the image.
     */
    Image(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
          std::shared_ptr<vk::Device> device,
          uint32_t x,
          uint32_t y,
          uint32_t numChannels,
          const DataTypes& dataType,
          vk::ImageTiling tiling,
          const MemoryTypes& memoryType = MemoryTypes::eDevice)
      : Image(physicalDevice,
              device,
              nullptr,
              0,
              x,
              y,
              numChannels,
              dataType,
              tiling,
              memoryType)
    {
    }

    /**
     *  Constructor with data provided which would be used to create the
     *  respective vulkan image and memory. No tiling has been provided
     *  so will be inferred from \p memoryType.
     *
     *  @param physicalDevice The physical device to use to fetch properties
     *  @param device The device to use to create the image and memory from
     *  @param data Pointer to data that will be used to initialise the image
     *  @param dataSize Size in bytes of the data pointed to by \p data
     *  @param x Width of the image in pixels
     *  @param y Height of the image in pixels
     *  @param numChannels The number of channels in the image
     *  @param dataType Data type for the image which is of type DataTypes
     *  @param memoryType Type for the image which is of type MemoryTypes
     */
    Image(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
          std::shared_ptr<vk::Device> device,
          void* data,
          size_t dataSize,
          uint32_t x,
          uint32_t y,
          uint32_t numChannels,
          const DataTypes& dataType,
          const MemoryTypes& memoryType = MemoryTypes::eDevice)
      : Memory(physicalDevice, device, dataType, memoryType, x, y)
    {
        vk::ImageTiling tiling;

        if (dataType == DataTypes::eCustom) {
            throw std::runtime_error(
              "Custom data types are not supported for Kompute Images");
        }

        if (memoryType == MemoryTypes::eHost ||
            memoryType == MemoryTypes::eDeviceAndHost) {
            // Host-accessible memory must be linear-tiled.
            tiling = vk::ImageTiling::eLinear;
        } else if (memoryType == MemoryTypes::eDevice ||
                   memoryType == MemoryTypes::eStorage) {
            tiling = vk::ImageTiling::eOptimal;
        } else {
            throw std::runtime_error("Kompute Image unsupported memory type");
        }

        init(data, dataSize, numChannels, tiling);
    }

    /**
     *  Constructor with no data provided. No tiling has been provided
     *  so will be inferred from \p memoryType.
     *
     *  @param physicalDevice The physical device to use to fetch properties
     *  @param device The device to use to create the image and memory from
     *  @param x Width of the image in pixels
     *  @param y Height of the image in pixels
     *  @param dataType Data type for the image which is of type ImageDataTypes
     *  @param memoryType Type for the image which is of type MemoryTypes
     */
    Image(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
          std::shared_ptr<vk::Device> device,
          uint32_t x,
          uint32_t y,
          uint32_t numChannels,
          const DataTypes& dataType,
          const MemoryTypes& memoryType = MemoryTypes::eDevice)
      : Image(physicalDevice,
              device,
              nullptr,
              0,
              x,
              y,
              numChannels,
              dataType,
              memoryType)
    {
    }


    /**
     * @brief Make Image uncopyable
     *
     */
    Image(const Image&) = delete;
    Image(const Image&&) = delete;
    Image& operator=(const Image&) = delete;
    Image& operator=(const Image&&) = delete;

    /**
     * Destructor which is in charge of freeing vulkan resources unless they
     * have been provided externally.
     */
    virtual ~Image();

    /**
     * Destroys and frees the GPU resources which include the image and memory.
     */
    void destroy() override;

    /**
     * Check whether image is initialized based on the created gpu resources.
     *
     * @returns Boolean stating whether image is initialized
     */
    bool isInit() override;

    /**
     * Records a copy from the memory of the image provided to the current
     * image. This is intended to pass memory into a processing, to perform
     * a staging image transfer, or to gather output (between others).
     *
     * @param commandBuffer Vulkan Command Buffer to record the commands into
     * @param copyFromImage Image to copy the data from
     */
    void recordCopyFrom(const vk::CommandBuffer& commandBuffer,
                        std::shared_ptr<Image> copyFromImage) override;

    /**
     * Records a copy from the memory of the tensor provided to the current
     * image. This is intended to pass memory into a processing, to perform
     * a staging image transfer, or to gather output (between others).
     *
     * @param commandBuffer Vulkan Command Buffer to record the commands into
     * @param copyFromTensor Tensor to copy the data from
     */
    void recordCopyFrom(const vk::CommandBuffer& commandBuffer,
                        std::shared_ptr<Tensor> copyFromTensor) override;

    /**
     * Records a copy from the internal staging memory to the device memory
     * using an optional barrier to wait for the operation. This function would
     * only be relevant for kp::images of type eDevice.
     *
     * @param commandBuffer Vulkan Command Buffer to record the commands into
     */
    void recordCopyFromStagingToDevice(
      const vk::CommandBuffer& commandBuffer) override;

    /**
     * Records a copy from the internal device memory to the staging memory
     * using an optional barrier to wait for the operation. This function would
     * only be relevant for kp::images of type eDevice.
     *
     * @param commandBuffer Vulkan Command Buffer to record the commands into
     */
    void recordCopyFromDeviceToStaging(
      const vk::CommandBuffer& commandBuffer) override;

    /**
     * Records the image memory barrier into the primary image and command
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
     * Records the image memory barrier into the staging image and command
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
     * Records the image memory barrier into the primary image and command
     * buffer which ensures that relevant data transfers are carried out
     * correctly.
     *
     * @param commandBuffer Vulkan Command Buffer to record the commands into
     * @param srcAccessMask Access flags for source access mask
     * @param dstAccessMask Access flags for destination access mask
     * @param scrStageMask Pipeline stage flags for source stage mask
     * @param dstStageMask Pipeline stage flags for destination stage mask
     * @param dstLayout Image layout for the image after the barrier completes
     */
    void recordPrimaryImageBarrier(const vk::CommandBuffer& commandBuffer,
                                   vk::AccessFlagBits srcAccessMask,
                                   vk::AccessFlagBits dstAccessMask,
                                   vk::PipelineStageFlagBits srcStageMask,
                                   vk::PipelineStageFlagBits dstStageMask,
                                   vk::ImageLayout dstLayout);

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

    std::shared_ptr<vk::Image> getPrimaryImage();
    vk::ImageLayout getPrimaryImageLayout();

    /***
     * Retreive the number of channels in the image
     *
     * @return Number of channels in the image
     */
    uint32_t getNumChannels();

    Type type() override { return Type::eImage; }

  protected:
    // -------------- ALWAYS OWNED RESOURCES
    uint32_t mNumChannels;
    vk::DescriptorImageInfo mDescriptorImageInfo;
    vk::ImageLayout mPrimaryImageLayout = vk::ImageLayout::eUndefined;
    vk::ImageLayout mStagingImageLayout = vk::ImageLayout::eUndefined;
    std::shared_ptr<vk::ImageView> mImageView = nullptr;
    vk::ImageTiling mTiling = vk::ImageTiling::eOptimal;

  private:
    // -------------- OPTIONALLY OWNED RESOURCES
    std::shared_ptr<vk::Image> mPrimaryImage;
    bool mFreePrimaryImage = false;
    std::shared_ptr<vk::Image> mStagingImage;
    bool mFreeStagingImage = false;

    void allocateMemoryCreateGPUResources(); // Creates the vulkan image
    void createImage(std::shared_ptr<vk::Image> image,
                     vk::ImageUsageFlags imageUsageFlags,
                     vk::ImageTiling imageTiling);
    void allocateBindMemory(std::shared_ptr<vk::Image> image,
                            std::shared_ptr<vk::DeviceMemory> memory,
                            vk::MemoryPropertyFlags memoryPropertyFlags);
    void recordCopyImage(const vk::CommandBuffer& commandBuffer,
                         std::shared_ptr<vk::Image> srcImage,
                         std::shared_ptr<vk::Image> dstImage,
                         vk::ImageLayout srcLayout,
                         vk::ImageLayout dstLayout,
                         vk::ImageCopy copyRegion);
    void recordCopyImageFromTensor(const vk::CommandBuffer& commandBuffer,
                                   std::shared_ptr<vk::Buffer> srcBuffer,
                                   std::shared_ptr<vk::Image> dstImage,
                                   vk::ImageLayout dstLayout,
                                   vk::BufferImageCopy copyRegion);

    /**
     * Records the image memory barrier into the staging image and command
     * buffer which ensures that relevant data transfers are carried out
     * correctly.
     *
     * @param commandBuffer Vulkan Command Buffer to record the commands into
     * @param srcAccessMask Access flags for source access mask
     * @param dstAccessMask Access flags for destination access mask
     * @param scrStageMask Pipeline stage flags for source stage mask
     * @param dstStageMask Pipeline stage flags for destination stage mask
     * @param dstLayout Image layout for the image after the barrier completes
     */
    void recordStagingImageBarrier(const vk::CommandBuffer& commandBuffer,
                                   vk::AccessFlagBits srcAccessMask,
                                   vk::AccessFlagBits dstAccessMask,
                                   vk::PipelineStageFlagBits srcStageMask,
                                   vk::PipelineStageFlagBits dstStageMask,
                                   vk::ImageLayout dstLayout);

    void recordImageMemoryBarrier(const vk::CommandBuffer& commandBuffer,
                                  const vk::Image& image,
                                  vk::AccessFlagBits srcAccessMask,
                                  vk::AccessFlagBits dstAccessMask,
                                  vk::PipelineStageFlagBits srcStageMask,
                                  vk::PipelineStageFlagBits dstStageMask,
                                  vk::ImageLayout oldLayout,
                                  vk::ImageLayout newLayout);

    // Private util functions
    vk::ImageUsageFlags getPrimaryImageUsageFlags();
    vk::ImageUsageFlags getStagingImageUsageFlags();

    vk::Format getFormat();

    vk::DescriptorImageInfo constructDescriptorImageInfo();

    void init(void* data,
              size_t dataSize,
              uint32_t numChannels,
              vk::ImageTiling tiling);
    /**
     * Function to reserve memory on the image. This does not copy any data, it
     * just reserves memory, similarly to std::vector reserve() method.
     */
    void reserve();
};

template<typename T>
class ImageT : public Image
{

  public:
    ImageT(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
           std::shared_ptr<vk::Device> device,
           const std::vector<T>& data,
           uint32_t x,
           uint32_t y,
           uint32_t numChannels,
           vk::ImageTiling tiling,
           const MemoryTypes& imageType = MemoryTypes::eDevice)
      : Image(physicalDevice,
              device,
              (void*)data.data(),
              data.size(),
              x,
              y,
              numChannels,
              Memory::dataType<T>(),
              tiling,
              imageType)
    {
        // Images cannot be created with custom types
        static_assert(Memory::dataType<T>() != DataTypes::eCustom,
                      "Custom data types are not supported for Kompute Images");

        KP_LOG_DEBUG("Kompute imageT constructor with data size {}, x {}, "
                     "y {}, and num channels {}",
                     data.size(),
                     x,
                     y,
                     numChannels);
    }

    ImageT(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
           std::shared_ptr<vk::Device> device,
           const std::vector<T>& data,
           uint32_t x,
           uint32_t y,
           uint32_t numChannels,
           const MemoryTypes& imageType = MemoryTypes::eDevice)
      : Image(physicalDevice,
              device,
              (void*)data.data(),
              data.size(),
              x,
              y,
              numChannels,
              Memory::dataType<T>(),
              imageType)
    {
        // Images cannot be created with custom types
        static_assert(Memory::dataType<T>() != DataTypes::eCustom,
                      "Custom data types are not supported for Kompute Images");

        KP_LOG_DEBUG("Kompute imageT constructor with data size {}, x {}, "
                     "y {}, and num channels {}",
                     data.size(),
                     x,
                     y,
                     numChannels);
    }

    ImageT(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
           std::shared_ptr<vk::Device> device,
           uint32_t x,
           uint32_t y,
           uint32_t numChannels,
           vk::ImageTiling tiling,
           const MemoryTypes& imageType = MemoryTypes::eDevice)
      : Image(physicalDevice,
              device,
              x,
              y,
              numChannels,
              Memory::dataType<T>(),
              tiling,
              imageType)
    {
        // Images cannot be created with custom types
        static_assert(Memory::dataType<T>() != DataTypes::eCustom,
                      "Custom data types are not supported for Kompute Images");

        KP_LOG_DEBUG("Kompute imageT constructor with no data, x {}, "
                     "y {}, and num channels {}",
                     x,
                     y,
                     numChannels);
    }

    ImageT(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
           std::shared_ptr<vk::Device> device,
           uint32_t x,
           uint32_t y,
           uint32_t numChannels,
           const MemoryTypes& imageType = MemoryTypes::eDevice)
      : Image(physicalDevice,
              device,
              x,
              y,
              numChannels,
              Memory::dataType<T>(),
              imageType)
    {
        // Images cannot be created with custom types
        static_assert(Memory::dataType<T>() != DataTypes::eCustom,
                      "Custom data types are not supported for Kompute Images");

        KP_LOG_DEBUG("Kompute imageT constructor with no data, x {}, "
                     "y {}, and num channels {}",
                     x,
                     y,
                     numChannels);
    }

    ~ImageT() { KP_LOG_DEBUG("Kompute imageT destructor"); }

    std::vector<T> vector() { return Memory::vector<T>(); }

    T& operator[](int index) { return *(Memory::data<T>() + index); }
};

} // End namespace kp
