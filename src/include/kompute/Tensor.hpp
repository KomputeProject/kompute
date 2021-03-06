#pragma once

#include "kompute/Core.hpp"

namespace kp {

/**
 * Structured data used in GPU operations.
 *
 * Tensors are the base building block in Kompute to perform operations across
 * GPUs. Each tensor would have a respective Vulkan memory and buffer, which
 * would be used to store their respective data. The tensors can be used for GPU
 * data storage or transfer.
 */
class Tensor
{
  public:
    /**
     * Type for tensors created: Device allows memory to be transferred from
     * staging buffers. Staging are host memory visible. Storage are device
     * visible but are not set up to transfer or receive data (only for shader
     * storage).
     */
    enum class TensorTypes
    {
        eDevice = 0,  ///< Type is device memory, source and destination
        eHost = 1,    ///< Type is host memory, source and destination
        eStorage = 2, ///< Type is Device memory (only)
    };
    enum class TensorDataTypes
    {
        eBool = 0,
        eInt = 1,
        eUnsignedInt = 2,
        eFloat = 3,
        eDouble = 4,
    };

    /**
     *  Constructor with data provided which would be used to create the
     * respective vulkan buffer and memory.
     *
     *  @param physicalDevice The physical device to use to fetch properties
     *  @param device The device to use to create the buffer and memory from
     *  @param data Non-zero-sized vector of data that will be used by the
     * tensor
     *  @param tensorTypes Type for the tensor which is of type TensorTypes
     */
    Tensor(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
           std::shared_ptr<vk::Device> device,
           void* data,
           uint32_t elementTotalCount,
           uint32_t elementMemorySize,
           const TensorDataTypes& dataType = TensorDataTypes::eFloat,
           const TensorTypes& tensorType = TensorTypes::eDevice);

    /**
     * Destructor which is in charge of freeing vulkan resources unless they
     * have been provided externally.
     */
    ~Tensor();

    /**
     * Returns the size/magnitude of the Tensor, which will be the total number
     * of elements across all dimensions
     *
     * @return Unsigned integer representing the total number of elements
     */
    // TODO: move to cpp
    virtual uint32_t size() {
        return this->mElementMemorySize;
    }

    // TODO: move to cpp
    virtual uint32_t memorySize() {
        return this->mSize * this->mElementMemorySize;
    }

    /**
     * Retrieve the underlying data type of the Tensor
     *
     * @return Data type of tensor of type kp::Tensor::TensorDataTypes
     */
    virtual TensorDataTypes dataType() {
        return this->mDataType;
    }

    /**
     * Maps data from the Host Visible GPU memory into the data vector. It
     * requires the Tensor to be of staging type for it to work.
     */
    virtual void mapDataFromHostMemory();
    /**
     * Maps data from the data vector into the Host Visible GPU memory. It
     * requires the tensor to be of staging type for it to work.
     */
    virtual void mapDataIntoHostMemory();

    // TODO: Decide whether this is one we prefer to have also overriden in the underlying tensorView
    // TODO: move to cpp
    void getRawData(void* data) {
        this->rawMapDataFromHostMemory(data);
    }

    /**
     * Function to trigger reinitialisation of the tensor buffer and memory with
     * new data as well as new potential device type.
     *
     * @param data Vector of data to use to initialise vector from
     * @param tensorType The type to use for the tensor
     */
    void rebuild(void* data,
                 uint32_t elementTotalCount,
                 uint32_t elementMemorySize,
                 const TensorDataTypes& dataType = TensorDataTypes::eFloat,
                 TensorTypes tensorType = TensorTypes::eDevice);

    /**
     * Destroys and frees the GPU resources which include the buffer and memory.
     */
    void destroy();

    /**
     * Check whether tensor is initialized based on the created gpu resources.
     *
     * @returns Boolean stating whether tensor is initialized
     */
    bool isInit();

    /**
     * Retrieve the tensor type of the Tensor
     *
     * @return Tensor type of tensor
     */
    TensorTypes tensorType();

    /**
     * Sets / resets the vector data of the tensor. This function does not
     * perform any copies into GPU memory and is only performed on the host.
     */
    void setRawData(void* data, uint32_t elementTotalCount, uint32_t elementMemorySize) {
        if (elementTotalCount * elementMemorySize != this->memorySize()) {
            throw std::runtime_error(
              "Kompute Tensor Cannot set data of different sizes");
        }
        this->mSize = elementTotalCount;
        this->mElementMemorySize = elementMemorySize;
        this->rawMapDataIntoHostMemory(data);
    }

    /**
     * Records a copy from the memory of the tensor provided to the current
     * thensor. This is intended to pass memory into a processing, to perform
     * a staging buffer transfer, or to gather output (between others).
     *
     * @param commandBuffer Vulkan Command Buffer to record the commands into
     * @param copyFromTensor Tensor to copy the data from
     * @param createBarrier Whether to create a barrier that ensures the data is
     * copied before further operations. Default is true.
     */
    void recordCopyFrom(const vk::CommandBuffer& commandBuffer,
                        std::shared_ptr<Tensor> copyFromTensor,
                        bool createBarrier);

    /**
     * Records a copy from the internal staging memory to the device memory
     * using an optional barrier to wait for the operation. This function would
     * only be relevant for kp::Tensors of type eDevice.
     *
     * @param commandBuffer Vulkan Command Buffer to record the commands into
     * @param createBarrier Whether to create a barrier that ensures the data is
     * copied before further operations. Default is true.
     */
    void recordCopyFromStagingToDevice(const vk::CommandBuffer& commandBuffer,
                                       bool createBarrier);

    /**
     * Records a copy from the internal device memory to the staging memory
     * using an optional barrier to wait for the operation. This function would
     * only be relevant for kp::Tensors of type eDevice.
     *
     * @param commandBuffer Vulkan Command Buffer to record the commands into
     * @param createBarrier Whether to create a barrier that ensures the data is
     * copied before further operations. Default is true.
     */
    void recordCopyFromDeviceToStaging(const vk::CommandBuffer& commandBuffer,
                                       bool createBarrier);

    /**
     * Records the buffer memory barrier into the command buffer which
     * ensures that relevant data transfers are carried out correctly.
     *
     * @param commandBuffer Vulkan Command Buffer to record the commands into
     * @param srcAccessMask Access flags for source access mask
     * @param dstAccessMask Access flags for destination access mask
     * @param scrStageMask Pipeline stage flags for source stage mask
     * @param dstStageMask Pipeline stage flags for destination stage mask
     */
    void recordBufferMemoryBarrier(const vk::CommandBuffer& commandBuffer,
                                   vk::AccessFlagBits srcAccessMask,
                                   vk::AccessFlagBits dstAccessMask,
                                   vk::PipelineStageFlagBits srcStageMask,
                                   vk::PipelineStageFlagBits dstStageMask);

    /**
     * Constructs a vulkan descriptor buffer info which can be used to specify
     * and reference the underlying buffer component of the tensor without
     * exposing it.
     *
     * @return Descriptor buffer info with own buffer
     */
    vk::DescriptorBufferInfo constructDescriptorBufferInfo();

  private:
    // -------------- NEVER OWNED RESOURCES
    std::shared_ptr<vk::PhysicalDevice> mPhysicalDevice;
    std::shared_ptr<vk::Device> mDevice;

    // -------------- OPTIONALLY OWNED RESOURCES
    std::shared_ptr<vk::Buffer> mPrimaryBuffer;
    bool mFreePrimaryBuffer = false;
    std::shared_ptr<vk::Buffer> mStagingBuffer;
    bool mFreeStagingBuffer = false;
    std::shared_ptr<vk::DeviceMemory> mPrimaryMemory;
    bool mFreePrimaryMemory = false;
    std::shared_ptr<vk::DeviceMemory> mStagingMemory;
    bool mFreeStagingMemory = false;

    // -------------- ALWAYS OWNED RESOURCES
    TensorTypes mTensorType;
    TensorDataTypes mDataType;
    uint32_t mSize;
    uint32_t mElementMemorySize;

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
                          vk::BufferCopy copyRegion,
                          bool createBarrier);

    // Private util functions
    vk::BufferUsageFlags getPrimaryBufferUsageFlags();
    vk::MemoryPropertyFlags getPrimaryMemoryPropertyFlags();
    vk::BufferUsageFlags getStagingBufferUsageFlags();
    vk::MemoryPropertyFlags getStagingMemoryPropertyFlags();

    void rawMapDataFromHostMemory(void* data) {

        KP_LOG_DEBUG("Kompute Tensor mapping data from host buffer");

        std::shared_ptr<vk::DeviceMemory> hostVisibleMemory = nullptr;

        if (this->mTensorType == TensorTypes::eHost) {
            hostVisibleMemory = this->mPrimaryMemory;
        } else if (this->mTensorType == TensorTypes::eDevice) {
            hostVisibleMemory = this->mStagingMemory;
        } else {
            KP_LOG_WARN(
              "Kompute Tensor mapping data not supported on storage tensor");
            return;
        }

        vk::DeviceSize bufferSize = this->memorySize();
        void* mapped = this->mDevice->mapMemory(
          *hostVisibleMemory, 0, bufferSize, vk::MemoryMapFlags());
        vk::MappedMemoryRange mappedMemoryRange(*hostVisibleMemory, 0, bufferSize);
        this->mDevice->invalidateMappedMemoryRanges(mappedMemoryRange);
        memcpy(data, mapped, bufferSize);
        this->mDevice->unmapMemory(*hostVisibleMemory);
    }

    void rawMapDataIntoHostMemory(void* data) {
        KP_LOG_DEBUG("Kompute Tensor mapping data into host buffer");

        std::shared_ptr<vk::DeviceMemory> hostVisibleMemory = nullptr;

        if (this->mTensorType == TensorTypes::eHost) {
            hostVisibleMemory = this->mPrimaryMemory;
        } else if (this->mTensorType == TensorTypes::eDevice) {
            hostVisibleMemory = this->mStagingMemory;
        } else {
            KP_LOG_WARN(
              "Kompute Tensor mapping data not supported on storage tensor");
            return;
        }

        vk::DeviceSize bufferSize = this->memorySize();

        void* mapped = this->mDevice->mapMemory(
          *hostVisibleMemory, 0, bufferSize, vk::MemoryMapFlags());
        memcpy(mapped, data, bufferSize);
        vk::MappedMemoryRange mappedRange(*hostVisibleMemory, 0, bufferSize);
        this->mDevice->flushMappedMemoryRanges(1, &mappedRange);
        this->mDevice->unmapMemory(*hostVisibleMemory);
    }
};

// TODO: Limit T to be only float, bool, double, etc
template <typename T>
class TensorView: public Tensor
{
  public:
    TensorView(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
           std::shared_ptr<vk::Device> device,
           const std::vector<T>& data,
           const TensorTypes& tensorType = TensorTypes::eDevice);

    ~TensorView();

    void rebuild(const std::vector<T>& data,
            TensorTypes tensorType = TensorTypes::eDevice) {

        this->mData = data;
        Tensor::rebuild(data.data(), data.size(), sizeof(T), this->dataType(), tensorType);
    }

    std::vector<T>& data() {
        return this->mData;
    }

    T& operator[](int index) {
        return this->mData[index];
    }

    void setData(const std::vector<T>& data) {

        if (data.size() != this->mData.size()) {
            throw std::runtime_error(
              "Kompute TensorView Cannot set data of different sizes");
        }

        this->mData = data;

        this->setRawData(this->mData.data(), this->mData.size(), sizeof(T), this->dataType());
    }

    TensorDataTypes dataType() override;

    uint32_t size() override {
        return this->mData->size();
    }

    uint32_t memorySize() override {
        return this->mData->size() * sizeof(T);
    }

    /**
     * Maps data from the Host Visible GPU memory into the data vector. It
     * requires the Tensor to be of staging type for it to work.
     */
    void mapDataFromHostMemory() override {
        KP_LOG_DEBUG("Kompute TensorView mapDataFromHostMemory copying data");

        this->rawMapDataFromHostMemory(this->mData.data());
    }
    /**
     * Maps data from the data vector into the Host Visible GPU memory. It
     * requires the tensor to be of staging type for it to work.
     */
    void mapDataIntoHostMemory() override {
        KP_LOG_DEBUG("Kompute TensorView mapDataIntoHostMemory copying data");

        this->rawMapDataIntoHostMemory(this->mData.data());
    }

  private:
    // -------------- ALWAYS OWNED RESOURCES
    std::vector<T> mData;

};

template<>
Tensor::TensorDataTypes
TensorView<bool>::dataType() {
    return Tensor::TensorDataTypes::eBool;
}

template<>
Tensor::TensorDataTypes
TensorView<int32_t>::dataType() {
    return Tensor::TensorDataTypes::eInt;
}

template<>
Tensor::TensorDataTypes
TensorView<uint32_t>::dataType() {
    return Tensor::TensorDataTypes::eUnsignedInt;
}

template<>
Tensor::TensorDataTypes
TensorView<float>::dataType() {
    return Tensor::TensorDataTypes::eFloat;
}

template<>
Tensor::TensorDataTypes
TensorView<double>::dataType() {
    return Tensor::TensorDataTypes::eDouble;
}

} // End namespace kp
