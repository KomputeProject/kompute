#pragma once

#include "kompute/Core.hpp"

#define KP_MAX_DIM_SIZE 1

namespace kp {

/**
 * Structured data used in GPU operations.
 *
 * Tensors are the base building block in Kompute to perform operations across
 * GPUs. Each tensor would have a respective Vulkan memory and buffer, which
 * woudl be used to store their respective data. The tensors can be used for GPU
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
        eDevice = 0,
        eStaging = 1,
        eStorage = 2,
    };

    /**
     *  Base constructor, should not be used unless explicitly intended.
     */
    Tensor();

    /**
     *  Default constructor with data provided which would be used to create the
     * respective vulkan buffer and memory.
     *
     *  @param data Vector of data that will be used by the tensor
     *  @param tensorType Type for the tensor which is of type TensorTypes
     */
    Tensor(std::vector<uint32_t> data,
           TensorTypes tensorType = TensorTypes::eDevice);

    /**
     * Destructor which is in charge of freeing vulkan resources unless they
     * have been provided externally.
     */
    ~Tensor();

    /**
     * Initialiser creates the buffer and GPU memory.
     */
    void init(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
              std::shared_ptr<vk::Device> device,
              std::shared_ptr<vk::CommandBuffer> commandBuffer);

    /**
     * Destroys and frees the GPU resources which include the buffer and memory.
     */
    void freeMemoryDestroyGPUResources();

    /**
     * Returns the vector of data currently contained by the Tensor. It is
     * important to ensure that there is no out-of-sync data with the GPU
     * memory.
     *
     * @return Vector of elements representing the data in the tensor.
     */
    std::vector<uint32_t> data();
    /**
     * Returns the size/magnitude of the Tensor, which will be the total number
     * of elements across all dimensions
     *
     * @return Unsigned integer representing the total number of elements
     */
    uint32_t size();
    /**
     * Returns the shape of the tensor, which includes the number of dimensions
     * and the size per dimension.
     *
     * @return Array containing the sizes for each dimension. Zero means
     * respective dimension is not active.
     */
    std::array<uint32_t, KP_MAX_DIM_SIZE> shape();
    /**
     * Retrieve the tensor type of the Tensor
     *
     * @return Tensor type of tensor
     */
    TensorTypes tensorType();
    /**
     * Returns true if the tensor initialisation function has been carried out
     * successful, which would mean that the buffer and memory will have been
     * provisioned.
     */
    bool isInit();

    /**
     * Sets / resets the vector data of the tensor. This function does not
     * perform any copies into GPU memory and is only performed on the host.
     */
    void setData(const std::vector<uint32_t>& data);

    /**
     * Records a copy from the memory of the tensor provided to the current
     * thensor. This is intended to pass memory into a processing, to perform
     * a staging buffer transfer, or to gather output (between others).
     *
     * @param copyFromTensor Tensor to copy the data from
     * @param createBarrier Whether to create a barrier that ensures the data is
     * copied before further operations. Default is true.
     */
    void recordCopyFrom(std::shared_ptr<Tensor> copyFromTensor,
                        bool createBarrier = true);

    /**
     * Records the buffer memory barrier into the command buffer which
     * ensures that relevant data transfers are carried out correctly.
     *
     * @param srcAccessMask Access flags for source access mask
     * @param dstAccessMask Access flags for destination access mask
     * @param scrStageMask Pipeline stage flags for source stage mask
     * @param dstStageMask Pipeline stage flags for destination stage mask
     */
    void recordBufferMemoryBarrier(vk::AccessFlagBits srcAccessMask,
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
    std::shared_ptr<vk::CommandBuffer> mCommandBuffer;

    // -------------- OPTIONALLY OWNED RESOURCES
    std::shared_ptr<vk::Buffer> mBuffer;
    bool mFreeBuffer;
    std::shared_ptr<vk::DeviceMemory> mMemory;
    bool mFreeMemory;

    // -------------- ALWAYS OWNED RESOURCES
    std::vector<uint32_t> mData;

    TensorTypes mTensorType = TensorTypes::eDevice;

    std::array<uint32_t, KP_MAX_DIM_SIZE> mShape;
    bool mIsInit = false;

    void createBuffer(); // Creates the vulkan buffer

    // Private util functions
    vk::BufferUsageFlags getBufferUsageFlags();
    vk::MemoryPropertyFlags getMemoryPropertyFlags();
    uint64_t memorySize();
};

} // End namespace kp
