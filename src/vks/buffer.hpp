/*
* Vulkan buffer class
*
* Encapsulates a Vulkan buffer
*
* Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#pragma once

#include "allocation.hpp"

namespace vks {
/**
    * @brief Encapsulates access to a Vulkan buffer backed up by device memory
    * @note To be filled by an external source like the VulkanDevice
    */
struct Buffer : public Allocation {
private:
    using Parent = Allocation;

public:
    vk::Buffer buffer;
    /** @brief Usage flags to be filled by external source at buffer creation (to query at some later point) */
    vk::BufferUsageFlags usageFlags;
    vk::DescriptorBufferInfo descriptor;

    operator bool() const { return buffer.operator bool(); }

    /** 
        * Attach the allocated memory block to the buffer
        * 
        * @param offset (Optional) Byte offset (from the beginning) for the memory region to bind
        * 
        * @return VkResult of the bindBufferMemory call
        */
    void bind(vk::DeviceSize offset = 0) { return device.bindBufferMemory(buffer, memory, offset); }

    /**
        * Setup the default descriptor for this buffer
        *
        * @param size (Optional) Size of the memory range of the descriptor
        * @param offset (Optional) Byte offset from beginning
        *
        */
    void setupDescriptor(vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0) {
        descriptor.offset = offset;
        descriptor.buffer = buffer;
        descriptor.range = size;
    }

    /**
        * Copies the specified data to the mapped buffer
        * 
        * @param data Pointer to the data to copy
        * @param size Size of the data to copy in machine units
        *
        */
    void copyTo(void* data, vk::DeviceSize size) {
        assert(mapped);
        memcpy(mapped, data, size);
    }

    /** 
        * Release all Vulkan resources held by this buffer
        */
    void destroy() override {
        if (buffer) {
            device.destroy(buffer);
            buffer = vk::Buffer{};
        }
        Parent::destroy();
    }
};
}  // namespace vks
