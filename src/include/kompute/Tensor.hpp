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

    /**
     *  Default constructor with data provided which would be used to create the
     * respective vulkan buffer and memory.
     *
     *  @param data Non-zero-sized vector of data that will be used by the
     * tensor
     *  @param tensorType Type for the tensor which is of type TensorTypes
     */
    Tensor(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
           std::shared_ptr<vk::Device> device,
           const std::vector<float>& data,
           const TensorTypes& tensorType = TensorTypes::eDevice);

    /**
     * Destructor which is in charge of freeing vulkan resources unless they
     * have been provided externally.
     */
    ~Tensor();

    /**
     * Initialiser which calls the initialisation for all the respective tensors
     * as well as creates the respective staging tensors. The staging tensors
     * would only be created for the tensors of type TensorType::eDevice as
     * otherwise there is no need to copy from host memory.
     */
    void rebuild(const std::vector<float>& data,
                 TensorTypes tensorType = TensorTypes::eDevice);

    /**
     * Destroys and frees the GPU resources which include the buffer and memory.
     */
    void destroy();

    bool isInit();

    /**
     * Returns the vector of data currently contained by the Tensor. It is
     * important to ensure that there is no out-of-sync data with the GPU
     * memory.
     *
     * @return Reference to vector of elements representing the data in the
     * tensor.
     */
    std::vector<float>& data();
    /**
     * Overrides the subscript operator to expose the underlying data's
     * subscript operator which in this case would be its underlying
     * vector's.
     *
     * @param i The index where the element will be returned from.
     * @return Returns the element in the position requested.
     */
    float& operator[](int index);
    /**
     * Returns the size/magnitude of the Tensor, which will be the total number
     * of elements across all dimensions
     *
     * @return Unsigned integer representing the total number of elements
     */
    uint32_t size();

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
    void setData(const std::vector<float>& data);

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
    /**
     * Maps data from the Host Visible GPU memory into the data vector. It
     * requires the Tensor to be of staging type for it to work.
     */
    void mapDataFromHostMemory();
    /**
     * Maps data from the data vector into the Host Visible GPU memory. It
     * requires the tensor to be of staging type for it to work.
     */
    void mapDataIntoHostMemory();

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
    std::vector<float> mData;

    TensorTypes mTensorType = TensorTypes::eDevice;

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
    uint64_t memorySize();
};

} // End namespace kp
