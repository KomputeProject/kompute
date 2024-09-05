// SPDX-License-Identifier: Apache-2.0

#include "kompute/Image.hpp"

namespace kp {

void
Image::init(void* data,
            size_t dataSize,
            uint32_t numChannels,
            vk::ImageTiling tiling)
{
    KP_LOG_DEBUG(
      "Kompute Image constructor data width: {}, height: {}, and type: {}",
      this->getX(),
      this->getY(),
      Memory::toString(this->memoryType()));

    if (numChannels == 0) {
        throw std::runtime_error(
          "Kompute Image attempted to create an image with no channels");
    }

    if (data != nullptr &&
        dataSize < this->getX() * this->getY() * numChannels) {
        throw std::runtime_error(
          "Kompute Image data is smaller than the requested image size");
    }

    if (numChannels > 4 || numChannels == 3) {
        // We don't support 3-channel images because they are not supported by
        // Metal or Mesa (llvmpipe) See comment here:
        // https://github.com/KomputeProject/kompute/pull/388#discussion_r1720959531
        throw std::runtime_error(
          "Kompute Images can only have up to 1, 2 or 4 channels");
    }

    if (tiling == vk::ImageTiling::eOptimal &&
        (this->memoryType() != Memory::MemoryTypes::eDevice &&
         this->memoryType() != Memory::MemoryTypes::eStorage)) {
        throw std::runtime_error("Kompute Image with optimal tiling is only "
                                 "supported for eDevice and eStorage images");
    }

    this->mNumChannels = numChannels;
    this->mDescriptorType = vk::DescriptorType::eStorageImage;
    this->mTiling = tiling;
    this->mSize = this->getX() * this->getY() * this->mNumChannels;

    this->reserve();
    this->updateRawData(data);
}

Image::~Image()
{
    KP_LOG_DEBUG("Kompute Image destructor started. Type: {}",
                 Memory::toString(this->memoryType()));

    if (this->mDevice) {
        this->destroy();
    }

    KP_LOG_DEBUG("Kompute Image destructor success");
}

void
Image::reserve()
{
    KP_LOG_DEBUG("Reserving {} bytes for memory",
                 this->mSize * this->mDataTypeMemorySize);

    if (this->mPrimaryImage || this->mPrimaryMemory) {
        KP_LOG_DEBUG(
          "Kompute Image destroying existing resources before rebuild");
        this->destroy();
    }

    this->allocateMemoryCreateGPUResources();
}

bool
Image::isInit()
{
    return this->mDevice && this->mPrimaryImage && this->mPrimaryMemory;
}

void
Image::recordCopyFrom(const vk::CommandBuffer& commandBuffer,
                      std::shared_ptr<Image> copyFromImage)
{
    vk::ImageSubresourceLayers layer = {};
    layer.aspectMask = vk::ImageAspectFlagBits::eColor;
    layer.layerCount = 1;
    vk::Offset3D offset = { 0, 0, 0 };

    if (this->getX() != copyFromImage->getX() ||
        this->getY() != copyFromImage->getY()) {
        throw std::runtime_error(
          "Kompute Image recordCopyFrom image sizes do not match");
    }

    vk::Extent3D size = { this->getX(), this->getY(), 1 };

    vk::ImageCopy copyRegion(layer, offset, layer, offset, size);

    KP_LOG_DEBUG(
      "Kompute Image recordCopyFrom size {},{}.", size.width, size.height);

    copyFromImage->recordPrimaryImageBarrier(
      commandBuffer,
      vk::AccessFlagBits::eMemoryRead,
      vk::AccessFlagBits::eMemoryWrite,
      vk::PipelineStageFlagBits::eTransfer,
      vk::PipelineStageFlagBits::eTransfer,
      vk::ImageLayout::eTransferSrcOptimal);

    this->recordPrimaryImageBarrier(commandBuffer,
                                    vk::AccessFlagBits::eMemoryRead,
                                    vk::AccessFlagBits::eMemoryWrite,
                                    vk::PipelineStageFlagBits::eTransfer,
                                    vk::PipelineStageFlagBits::eTransfer,
                                    vk::ImageLayout::eTransferDstOptimal);

    this->recordCopyImage(commandBuffer,
                          copyFromImage->mPrimaryImage,
                          this->mPrimaryImage,
                          copyFromImage->mPrimaryImageLayout,
                          this->mPrimaryImageLayout,
                          copyRegion);
}

void
Image::recordCopyFrom(const vk::CommandBuffer& commandBuffer,
                      std::shared_ptr<Tensor> copyFromTensor)
{
    vk::ImageSubresourceLayers layer = {};
    layer.aspectMask = vk::ImageAspectFlagBits::eColor;
    layer.layerCount = 1;
    vk::Offset3D offset = { 0, 0, 0 };

    vk::Extent3D size = { this->getX(), this->getY(), 1 };

    vk::BufferImageCopy copyRegion(0, 0, 0, layer, offset, size);

    KP_LOG_DEBUG(
      "Kompute Image recordCopyFrom size {},{}.", size.width, size.height);

    this->recordPrimaryImageBarrier(commandBuffer,
                                    vk::AccessFlagBits::eMemoryRead,
                                    vk::AccessFlagBits::eMemoryWrite,
                                    vk::PipelineStageFlagBits::eTransfer,
                                    vk::PipelineStageFlagBits::eTransfer,
                                    vk::ImageLayout::eTransferDstOptimal);

    this->recordCopyImageFromTensor(commandBuffer,
                                    copyFromTensor->getPrimaryBuffer(),
                                    this->mPrimaryImage,
                                    this->mPrimaryImageLayout,
                                    copyRegion);
}

void
Image::recordCopyFromStagingToDevice(const vk::CommandBuffer& commandBuffer)
{
    vk::ImageSubresourceLayers layer = {};
    layer.aspectMask = vk::ImageAspectFlagBits::eColor;
    layer.layerCount = 1;
    vk::Offset3D offset = { 0, 0, 0 };

    vk::Extent3D size = { this->getX(), this->getY(), 1 };

    vk::ImageCopy copyRegion(layer, offset, layer, offset, size);

    KP_LOG_DEBUG("Kompute Image copying size {},{}.", size.width, size.height);

    this->recordStagingImageBarrier(commandBuffer,
                                    vk::AccessFlagBits::eMemoryRead,
                                    vk::AccessFlagBits::eMemoryWrite,
                                    vk::PipelineStageFlagBits::eTransfer,
                                    vk::PipelineStageFlagBits::eTransfer,
                                    vk::ImageLayout::eTransferSrcOptimal);

    this->recordPrimaryImageBarrier(commandBuffer,
                                    vk::AccessFlagBits::eMemoryRead,
                                    vk::AccessFlagBits::eMemoryWrite,
                                    vk::PipelineStageFlagBits::eTransfer,
                                    vk::PipelineStageFlagBits::eTransfer,
                                    vk::ImageLayout::eTransferDstOptimal);

    this->recordCopyImage(commandBuffer,
                          this->mStagingImage,
                          this->mPrimaryImage,
                          this->mStagingImageLayout,
                          this->mPrimaryImageLayout,
                          copyRegion);
}

void
Image::recordCopyFromDeviceToStaging(const vk::CommandBuffer& commandBuffer)
{
    vk::ImageSubresourceLayers layer;
    layer.aspectMask = vk::ImageAspectFlagBits::eColor;
    layer.layerCount = 1;
    vk::Offset3D offset = { 0, 0, 0 };

    vk::Extent3D size = { this->getX(), this->getY(), 1 };

    vk::ImageCopy copyRegion(layer, offset, layer, offset, size);

    KP_LOG_DEBUG("Kompute Image copying size {},{}.", size.width, size.height);

    this->recordPrimaryImageBarrier(commandBuffer,
                                    vk::AccessFlagBits::eMemoryRead,
                                    vk::AccessFlagBits::eMemoryWrite,
                                    vk::PipelineStageFlagBits::eTransfer,
                                    vk::PipelineStageFlagBits::eTransfer,
                                    vk::ImageLayout::eTransferSrcOptimal);

    this->recordStagingImageBarrier(commandBuffer,
                                    vk::AccessFlagBits::eMemoryRead,
                                    vk::AccessFlagBits::eMemoryWrite,
                                    vk::PipelineStageFlagBits::eTransfer,
                                    vk::PipelineStageFlagBits::eTransfer,
                                    vk::ImageLayout::eTransferDstOptimal);

    this->recordCopyImage(commandBuffer,
                          this->mPrimaryImage,
                          this->mStagingImage,
                          this->mPrimaryImageLayout,
                          this->mStagingImageLayout,
                          copyRegion);
}

void
Image::recordCopyImage(const vk::CommandBuffer& commandBuffer,
                       std::shared_ptr<vk::Image> srcImage,
                       std::shared_ptr<vk::Image> dstImage,
                       vk::ImageLayout srcLayout,
                       vk::ImageLayout dstLayout,
                       vk::ImageCopy copyRegion)
{
    commandBuffer.copyImage(
      *srcImage, srcLayout, *dstImage, dstLayout, 1, &copyRegion);
}

void
Image::recordCopyImageFromTensor(const vk::CommandBuffer& commandBuffer,
                                 std::shared_ptr<vk::Buffer> srcBuffer,
                                 std::shared_ptr<vk::Image> dstImage,
                                 vk::ImageLayout dstLayout,
                                 vk::BufferImageCopy copyRegion)
{
    commandBuffer.copyBufferToImage(
      *srcBuffer, *dstImage, dstLayout, 1, &copyRegion);
}

void
Image::recordPrimaryMemoryBarrier(const vk::CommandBuffer& commandBuffer,
                                  vk::AccessFlagBits srcAccessMask,
                                  vk::AccessFlagBits dstAccessMask,
                                  vk::PipelineStageFlagBits srcStageMask,
                                  vk::PipelineStageFlagBits dstStageMask)
{
    vk::ImageLayout dstImageLayout;

    // Ideally the image would be set to eGeneral as soon as it was created
    // but this requires a command buffer, which we don't have at image
    // creation time, so use the first memory barrier we see to also
    // transition the image layout.
    if (this->mPrimaryImageLayout == vk::ImageLayout::eUndefined)
        dstImageLayout = vk::ImageLayout::eGeneral;
    else
        dstImageLayout = this->mPrimaryImageLayout;

    this->recordPrimaryImageBarrier(commandBuffer,
                                    srcAccessMask,
                                    dstAccessMask,
                                    srcStageMask,
                                    dstStageMask,
                                    dstImageLayout);
}

void
Image::recordStagingMemoryBarrier(const vk::CommandBuffer& commandBuffer,
                                  vk::AccessFlagBits srcAccessMask,
                                  vk::AccessFlagBits dstAccessMask,
                                  vk::PipelineStageFlagBits srcStageMask,
                                  vk::PipelineStageFlagBits dstStageMask)
{
    vk::ImageLayout dstImageLayout;

    // Ideally the image would be set to eGeneral as soon as it was created
    // but this requires a command buffer, which we don't have at image
    // creation time, so use the first memory barrier we see to also
    // transition the image layout.
    if (this->mStagingImageLayout == vk::ImageLayout::eUndefined)
        dstImageLayout = vk::ImageLayout::eGeneral;
    else
        dstImageLayout = this->mStagingImageLayout;

    this->recordStagingImageBarrier(commandBuffer,
                                    srcAccessMask,
                                    dstAccessMask,
                                    srcStageMask,
                                    dstStageMask,
                                    dstImageLayout);
}

void
Image::recordPrimaryImageBarrier(const vk::CommandBuffer& commandBuffer,
                                 vk::AccessFlagBits srcAccessMask,
                                 vk::AccessFlagBits dstAccessMask,
                                 vk::PipelineStageFlagBits srcStageMask,
                                 vk::PipelineStageFlagBits dstStageMask,
                                 vk::ImageLayout dstLayout)
{
    KP_LOG_DEBUG("Kompute Image recording PRIMARY image memory barrier");

    this->recordImageMemoryBarrier(commandBuffer,
                                   *this->mPrimaryImage,
                                   srcAccessMask,
                                   dstAccessMask,
                                   srcStageMask,
                                   dstStageMask,
                                   this->mPrimaryImageLayout,
                                   dstLayout);

    this->mPrimaryImageLayout = dstLayout;
}

void
Image::recordStagingImageBarrier(const vk::CommandBuffer& commandBuffer,
                                 vk::AccessFlagBits srcAccessMask,
                                 vk::AccessFlagBits dstAccessMask,
                                 vk::PipelineStageFlagBits srcStageMask,
                                 vk::PipelineStageFlagBits dstStageMask,
                                 vk::ImageLayout dstLayout)
{
    KP_LOG_DEBUG("Kompute Image recording STAGING image memory barrier");

    this->recordImageMemoryBarrier(commandBuffer,
                                   *this->mStagingImage,
                                   srcAccessMask,
                                   dstAccessMask,
                                   srcStageMask,
                                   dstStageMask,
                                   this->mStagingImageLayout,
                                   dstLayout);

    this->mStagingImageLayout = dstLayout;
}

void
Image::recordImageMemoryBarrier(const vk::CommandBuffer& commandBuffer,
                                const vk::Image& image,
                                vk::AccessFlagBits srcAccessMask,
                                vk::AccessFlagBits dstAccessMask,
                                vk::PipelineStageFlagBits srcStageMask,
                                vk::PipelineStageFlagBits dstStageMask,
                                vk::ImageLayout srcLayout,
                                vk::ImageLayout dstLayout)
{
    KP_LOG_DEBUG("Kompute Image recording image memory barrier");

    vk::ImageMemoryBarrier imageMemoryBarrier;
    imageMemoryBarrier.image = image;

    imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
    imageMemoryBarrier.subresourceRange.levelCount = 1;
    imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
    imageMemoryBarrier.subresourceRange.layerCount = 1;
    imageMemoryBarrier.subresourceRange.aspectMask =
      vk::ImageAspectFlagBits::eColor;

    imageMemoryBarrier.srcAccessMask = srcAccessMask;
    imageMemoryBarrier.dstAccessMask = dstAccessMask;
    imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    imageMemoryBarrier.oldLayout = srcLayout;
    imageMemoryBarrier.newLayout = dstLayout;

    commandBuffer.pipelineBarrier(srcStageMask,
                                  dstStageMask,
                                  vk::DependencyFlags(),
                                  nullptr,
                                  nullptr,
                                  imageMemoryBarrier);
}

vk::DescriptorImageInfo
Image::constructDescriptorImageInfo()
{
    KP_LOG_DEBUG("Kompute Image construct descriptor image info size {}",
                 this->memorySize());

    vk::ImageViewCreateInfo viewInfo;
    viewInfo.image = *this->mPrimaryImage;
    viewInfo.format = this->getFormat();
    viewInfo.flags = vk::ImageViewCreateFlags();
    viewInfo.viewType = vk::ImageViewType::e2D;

    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;
    viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;

    // This image object owns the image view
    if (!this->mImageView) {
        mImageView = std::make_shared<vk::ImageView>(
          this->mDevice->createImageView(viewInfo));
    }

    vk::DescriptorImageInfo descriptorInfo;

    descriptorInfo.imageView = *(mImageView.get());
    descriptorInfo.imageLayout = this->mPrimaryImageLayout;
    return descriptorInfo;
}

vk::WriteDescriptorSet
Image::constructDescriptorSet(vk::DescriptorSet descriptorSet, uint32_t binding)
{
    KP_LOG_DEBUG("Kompute Image construct descriptor set for binding {}",
                 binding);

    mDescriptorImageInfo = this->constructDescriptorImageInfo();

    return vk::WriteDescriptorSet(descriptorSet,
                                  binding, // Destination binding
                                  0,       // Destination array element
                                  1,       // Descriptor count
                                  vk::DescriptorType::eStorageImage,
                                  &mDescriptorImageInfo,
                                  nullptr); // Descriptor buffer info
}

vk::ImageUsageFlags
Image::getPrimaryImageUsageFlags()
{
    switch (this->mMemoryType) {
        case MemoryTypes::eDevice:
        case MemoryTypes::eHost:
        case MemoryTypes::eDeviceAndHost:
            return vk::ImageUsageFlagBits::eStorage |
                   vk::ImageUsageFlagBits::eTransferSrc |
                   vk::ImageUsageFlagBits::eTransferDst;
            break;
        case MemoryTypes::eStorage:
            return vk::ImageUsageFlagBits::eStorage |
                   // You can still copy images to/from storage memory
                   // so set the transfer usage flags here.
                   vk::ImageUsageFlagBits::eTransferSrc |
                   vk::ImageUsageFlagBits::eTransferDst;
            break;
        default:
            throw std::runtime_error("Kompute Image invalid image type");
    }
}

vk::ImageUsageFlags
Image::getStagingImageUsageFlags()
{
    switch (this->mMemoryType) {
        case MemoryTypes::eDevice:
            return vk::ImageUsageFlagBits::eTransferSrc |
                   vk::ImageUsageFlagBits::eTransferDst;
            break;
        default:
            throw std::runtime_error("Kompute Image invalid image type");
    }
}

std::shared_ptr<vk::Image>
Image::getPrimaryImage()
{
    return this->mPrimaryImage;
}

vk::ImageLayout
Image::getPrimaryImageLayout()
{
    return this->mPrimaryImageLayout;
}

uint32_t
Image::getNumChannels()
{
    return this->mNumChannels;
}

void
Image::allocateMemoryCreateGPUResources()
{
    KP_LOG_DEBUG("Kompute Image creating image");

    if (!this->mPhysicalDevice) {
        throw std::runtime_error("Kompute Image phyisical device is null");
    }
    if (!this->mDevice) {
        throw std::runtime_error("Kompute Image device is null");
    }

    KP_LOG_DEBUG("Kompute Image creating primary image and memory");

    this->mPrimaryImage = std::make_shared<vk::Image>();
    this->createImage(
      this->mPrimaryImage, this->getPrimaryImageUsageFlags(), this->mTiling);
    this->mFreePrimaryImage = true;
    this->mPrimaryMemory = std::make_shared<vk::DeviceMemory>();
    this->allocateBindMemory(this->mPrimaryImage,
                             this->mPrimaryMemory,
                             this->getPrimaryMemoryPropertyFlags());
    this->mFreePrimaryMemory = true;

    if (this->mMemoryType == MemoryTypes::eDevice) {
        KP_LOG_DEBUG("Kompute Image creating staging image and memory");

        this->mStagingImage = std::make_shared<vk::Image>();
        this->createImage(this->mStagingImage,
                          this->getStagingImageUsageFlags(),
                          vk::ImageTiling::eLinear);
        this->mFreeStagingImage = true;
        this->mStagingMemory = std::make_shared<vk::DeviceMemory>();
        this->allocateBindMemory(this->mStagingImage,
                                 this->mStagingMemory,
                                 this->getStagingMemoryPropertyFlags());
        this->mFreeStagingMemory = true;
    }

    KP_LOG_DEBUG("Kompute Image image & memory creation successful");
}

void
Image::createImage(std::shared_ptr<vk::Image> image,
                   vk::ImageUsageFlags imageUsageFlags,
                   vk::ImageTiling imageTiling)
{
    vk::DeviceSize imageSize = this->memorySize();

    if (imageSize < 1) {
        throw std::runtime_error(
          "Kompute Image attempted to create a zero-sized image");
    }

    KP_LOG_DEBUG("Kompute Image creating image with memory size: {}, and "
                 "usage flags: {}",
                 imageSize,
                 vk::to_string(imageUsageFlags));

    // TODO: Explore having concurrent sharing mode (with option)
    vk::ImageCreateInfo imageInfo;

    imageInfo.flags = vk::ImageCreateFlags();
    imageInfo.imageType = vk::ImageType::e2D;
    imageInfo.format = this->getFormat();
    imageInfo.extent = vk::Extent3D(this->getX(), this->getY(), 1);
    imageInfo.usage = imageUsageFlags;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.initialLayout = vk::ImageLayout::eUndefined;
    imageInfo.tiling = imageTiling;

    this->mDevice->createImage(&imageInfo, nullptr, image.get());
}

void
Image::allocateBindMemory(std::shared_ptr<vk::Image> image,
                          std::shared_ptr<vk::DeviceMemory> memory,
                          vk::MemoryPropertyFlags memoryPropertyFlags)
{

    KP_LOG_DEBUG("Kompute Image allocating and binding memory");

    vk::PhysicalDeviceMemoryProperties memoryProperties =
      this->mPhysicalDevice->getMemoryProperties();

    vk::MemoryRequirements memoryRequirements =
      this->mDevice->getImageMemoryRequirements(*image);

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
          "Memory type index for image creation not found");
    }

    KP_LOG_DEBUG(
      "Kompute Image allocating memory index: {}, size {}, flags: {}",
      memoryTypeIndex,
      memoryRequirements.size,
      vk::to_string(memoryPropertyFlags));

    vk::MemoryAllocateInfo memoryAllocateInfo(memoryRequirements.size,
                                              memoryTypeIndex);

    this->mDevice->allocateMemory(&memoryAllocateInfo, nullptr, memory.get());

    this->mDevice->bindImageMemory(*image, *memory, 0);
}

void
Image::destroy()
{
    KP_LOG_DEBUG("Kompute Image started destroy()");

    if (!this->mDevice) {
        KP_LOG_WARN(
          "Kompute Image destructor reached with null Device pointer");
        return;
    }

    if (this->mFreePrimaryImage) {
        if (!this->mPrimaryImage) {
            KP_LOG_WARN("Kompose Image expected to destroy primary image "
                        "but got null image");
        } else {
            KP_LOG_DEBUG("Kompose Image destroying primary image");
            this->mDevice->destroy(
              *this->mPrimaryImage,
              (vk::Optional<const vk::AllocationCallbacks>)nullptr);
            this->mPrimaryImage = nullptr;
            this->mFreePrimaryImage = false;
        }
    }

    if (this->mFreeStagingImage) {
        if (!this->mStagingImage) {
            KP_LOG_WARN("Kompose Image expected to destroy staging image "
                        "but got null image");
        } else {
            KP_LOG_DEBUG("Kompose Image destroying staging image");
            this->mDevice->destroy(
              *this->mStagingImage,
              (vk::Optional<const vk::AllocationCallbacks>)nullptr);
            this->mStagingImage = nullptr;
            this->mFreeStagingImage = false;
        }
    }

    if (this->mImageView) {
        KP_LOG_DEBUG("Kompose Image freeing image view");
        this->mDevice->destroyImageView(*this->mImageView);
        this->mImageView = nullptr;
    }

    Memory::destroy();

    KP_LOG_DEBUG("Kompute Image successful destroy()");
}

vk::Format
Image::getFormat()
{
    switch (this->mDataType) {
        case Memory::DataTypes::eChar: {
            switch (this->mNumChannels) {
                case 1:
                    return vk::Format::eR8Sint;
                case 2:
                    return vk::Format::eR8G8Sint;
                case 4:
                    return vk::Format::eR8G8B8A8Sint;
                default:
                    return vk::Format::eUndefined;
            }
        }
        case Memory::DataTypes::eUnsignedChar: {
            switch (this->mNumChannels) {
                case 1:
                    return vk::Format::eR8Uint;
                case 2:
                    return vk::Format::eR8G8Uint;
                case 4:
                    return vk::Format::eR8G8B8A8Uint;
                default:
                    return vk::Format::eUndefined;
            }
        }
        case Memory::DataTypes::eUnsignedShort: {
            switch (this->mNumChannels) {
                case 1:
                    return vk::Format::eR16Uint;
                case 2:
                    return vk::Format::eR16G16Uint;
                case 4:
                    return vk::Format::eR16G16B16A16Uint;
                default:
                    return vk::Format::eUndefined;
            }
        }
        case Memory::DataTypes::eShort: {
            switch (this->mNumChannels) {
                case 1:
                    return vk::Format::eR16Sint;
                case 2:
                    return vk::Format::eR16G16Sint;
                case 4:
                    return vk::Format::eR16G16B16A16Sint;
                default:
                    return vk::Format::eUndefined;
            }
        }
        case Memory::DataTypes::eUnsignedInt: {
            switch (this->mNumChannels) {
                case 1:
                    return vk::Format::eR32Uint;
                case 2:
                    return vk::Format::eR32G32Uint;
                case 4:
                    return vk::Format::eR32G32B32A32Uint;
                default:
                    return vk::Format::eUndefined;
            }
        }
        case Memory::DataTypes::eInt: {
            switch (this->mNumChannels) {
                case 1:
                    return vk::Format::eR32Sint;
                case 2:
                    return vk::Format::eR32G32Sint;
                case 4:
                    return vk::Format::eR32G32B32A32Sint;
                default:
                    return vk::Format::eUndefined;
            }
        }
        case Memory::DataTypes::eFloat: {
            switch (this->mNumChannels) {
                case 1:
                    return vk::Format::eR32Sfloat;
                case 2:
                    return vk::Format::eR32G32Sfloat;
                case 4:
                    return vk::Format::eR32G32B32A32Sfloat;
                default:
                    return vk::Format::eUndefined;
            }
        }
        default:
            return vk::Format::eUndefined;
    }
}
}
