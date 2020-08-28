
#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>

// SPDLOG_ACTIVE_LEVEL must be defined before spdlog.h import
#if DEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#endif

#include <spdlog/spdlog.h>

/*
    THIS FILE HAS BEEN AUTOMATICALLY GENERATED - DO NOT EDIT

    ---

    Copyright 2020 The Institute for Ethical AI & Machine Learning

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#ifndef SHADEROP_SHADEROPMULT_HPP
#define SHADEROP_SHADEROPMULT_HPP

namespace kp {
namespace shader_data {
static unsigned const char shaders_glsl_opmult_comp_spv[] = {
  0x03, 0x02, 0x23, 0x07, 0x00, 0x00, 0x01, 0x00, 0x08, 0x00, 0x08, 0x00,
  0x2a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x02, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x06, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x47, 0x4c, 0x53, 0x4c, 0x2e, 0x73, 0x74, 0x64, 0x2e, 0x34, 0x35, 0x30,
  0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x06, 0x00, 0x05, 0x00, 0x00, 0x00,
  0x04, 0x00, 0x00, 0x00, 0x6d, 0x61, 0x69, 0x6e, 0x00, 0x00, 0x00, 0x00,
  0x0b, 0x00, 0x00, 0x00, 0x10, 0x00, 0x06, 0x00, 0x04, 0x00, 0x00, 0x00,
  0x11, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 0x02, 0x00, 0x00, 0x00,
  0xc2, 0x01, 0x00, 0x00, 0x05, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00,
  0x6d, 0x61, 0x69, 0x6e, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x04, 0x00,
  0x08, 0x00, 0x00, 0x00, 0x69, 0x6e, 0x64, 0x65, 0x78, 0x00, 0x00, 0x00,
  0x05, 0x00, 0x08, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x67, 0x6c, 0x5f, 0x47,
  0x6c, 0x6f, 0x62, 0x61, 0x6c, 0x49, 0x6e, 0x76, 0x6f, 0x63, 0x61, 0x74,
  0x69, 0x6f, 0x6e, 0x49, 0x44, 0x00, 0x00, 0x00, 0x05, 0x00, 0x06, 0x00,
  0x11, 0x00, 0x00, 0x00, 0x74, 0x65, 0x6e, 0x73, 0x6f, 0x72, 0x4f, 0x75,
  0x74, 0x70, 0x75, 0x74, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x07, 0x00,
  0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x61, 0x6c, 0x75,
  0x65, 0x73, 0x4f, 0x75, 0x74, 0x70, 0x75, 0x74, 0x00, 0x00, 0x00, 0x00,
  0x05, 0x00, 0x03, 0x00, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x05, 0x00, 0x05, 0x00, 0x18, 0x00, 0x00, 0x00, 0x74, 0x65, 0x6e, 0x73,
  0x6f, 0x72, 0x4c, 0x68, 0x73, 0x00, 0x00, 0x00, 0x06, 0x00, 0x06, 0x00,
  0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x61, 0x6c, 0x75,
  0x65, 0x73, 0x4c, 0x68, 0x73, 0x00, 0x00, 0x00, 0x05, 0x00, 0x03, 0x00,
  0x1a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x05, 0x00,
  0x20, 0x00, 0x00, 0x00, 0x74, 0x65, 0x6e, 0x73, 0x6f, 0x72, 0x52, 0x68,
  0x73, 0x00, 0x00, 0x00, 0x06, 0x00, 0x06, 0x00, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x76, 0x61, 0x6c, 0x75, 0x65, 0x73, 0x52, 0x68,
  0x73, 0x00, 0x00, 0x00, 0x05, 0x00, 0x03, 0x00, 0x22, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x0b, 0x00, 0x00, 0x00,
  0x0b, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00,
  0x10, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
  0x48, 0x00, 0x05, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x03, 0x00,
  0x11, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00,
  0x13, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x47, 0x00, 0x04, 0x00, 0x13, 0x00, 0x00, 0x00, 0x21, 0x00, 0x00, 0x00,
  0x02, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x17, 0x00, 0x00, 0x00,
  0x06, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x48, 0x00, 0x05, 0x00,
  0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x03, 0x00, 0x18, 0x00, 0x00, 0x00,
  0x03, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x1a, 0x00, 0x00, 0x00,
  0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00,
  0x1a, 0x00, 0x00, 0x00, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x47, 0x00, 0x04, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00,
  0x04, 0x00, 0x00, 0x00, 0x48, 0x00, 0x05, 0x00, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x47, 0x00, 0x03, 0x00, 0x20, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
  0x47, 0x00, 0x04, 0x00, 0x22, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x22, 0x00, 0x00, 0x00,
  0x21, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00,
  0x29, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x19, 0x00, 0x00, 0x00,
  0x13, 0x00, 0x02, 0x00, 0x02, 0x00, 0x00, 0x00, 0x21, 0x00, 0x03, 0x00,
  0x03, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x15, 0x00, 0x04, 0x00,
  0x06, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x20, 0x00, 0x04, 0x00, 0x07, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00,
  0x06, 0x00, 0x00, 0x00, 0x17, 0x00, 0x04, 0x00, 0x09, 0x00, 0x00, 0x00,
  0x06, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00,
  0x0a, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00,
  0x3b, 0x00, 0x04, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x2b, 0x00, 0x04, 0x00, 0x06, 0x00, 0x00, 0x00,
  0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00,
  0x0d, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00,
  0x1d, 0x00, 0x03, 0x00, 0x10, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00,
  0x1e, 0x00, 0x03, 0x00, 0x11, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
  0x20, 0x00, 0x04, 0x00, 0x12, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
  0x11, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x12, 0x00, 0x00, 0x00,
  0x13, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x15, 0x00, 0x04, 0x00,
  0x14, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x2b, 0x00, 0x04, 0x00, 0x14, 0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x1d, 0x00, 0x03, 0x00, 0x17, 0x00, 0x00, 0x00,
  0x06, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x03, 0x00, 0x18, 0x00, 0x00, 0x00,
  0x17, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x19, 0x00, 0x00, 0x00,
  0x02, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00,
  0x19, 0x00, 0x00, 0x00, 0x1a, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
  0x20, 0x00, 0x04, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
  0x06, 0x00, 0x00, 0x00, 0x1d, 0x00, 0x03, 0x00, 0x1f, 0x00, 0x00, 0x00,
  0x06, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x03, 0x00, 0x20, 0x00, 0x00, 0x00,
  0x1f, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x21, 0x00, 0x00, 0x00,
  0x02, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00,
  0x21, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
  0x2b, 0x00, 0x04, 0x00, 0x06, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x2c, 0x00, 0x06, 0x00, 0x09, 0x00, 0x00, 0x00,
  0x29, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00,
  0x28, 0x00, 0x00, 0x00, 0x36, 0x00, 0x05, 0x00, 0x02, 0x00, 0x00, 0x00,
  0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
  0xf8, 0x00, 0x02, 0x00, 0x05, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00,
  0x07, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00,
  0x41, 0x00, 0x05, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00,
  0x0b, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00,
  0x06, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00,
  0x3e, 0x00, 0x03, 0x00, 0x08, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00,
  0x3d, 0x00, 0x04, 0x00, 0x06, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00,
  0x08, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x06, 0x00, 0x00, 0x00,
  0x1b, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x41, 0x00, 0x06, 0x00,
  0x1c, 0x00, 0x00, 0x00, 0x1d, 0x00, 0x00, 0x00, 0x1a, 0x00, 0x00, 0x00,
  0x15, 0x00, 0x00, 0x00, 0x1b, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00,
  0x06, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x1d, 0x00, 0x00, 0x00,
  0x3d, 0x00, 0x04, 0x00, 0x06, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00,
  0x08, 0x00, 0x00, 0x00, 0x41, 0x00, 0x06, 0x00, 0x1c, 0x00, 0x00, 0x00,
  0x24, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00,
  0x23, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x06, 0x00, 0x00, 0x00,
  0x25, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x84, 0x00, 0x05, 0x00,
  0x06, 0x00, 0x00, 0x00, 0x26, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00,
  0x25, 0x00, 0x00, 0x00, 0x41, 0x00, 0x06, 0x00, 0x1c, 0x00, 0x00, 0x00,
  0x27, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00,
  0x16, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x03, 0x00, 0x27, 0x00, 0x00, 0x00,
  0x26, 0x00, 0x00, 0x00, 0xfd, 0x00, 0x01, 0x00, 0x38, 0x00, 0x01, 0x00
};
static unsigned const int shaders_glsl_opmult_comp_spv_len = 1308;
}
}
#endif // define SHADEROP_SHADEROPMULT_HPP

#include <unordered_map>

#define KP_MAX_DIM_SIZE 1

namespace kp {

class Tensor
{
  public:
    enum class TensorTypes
    {
        eDevice = 0,
        eStaging = 1,
        eStorage = 2,
    };

    Tensor();

    Tensor(std::vector<uint32_t> data,
           TensorTypes tensorType = TensorTypes::eDevice);

    ~Tensor();

    void init(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
              std::shared_ptr<vk::Device> device,
              std::shared_ptr<vk::CommandBuffer> commandBuffer);

    // Create functions
    void createBuffer();

    // Destroy/Free functions
    void freeMemoryDestroyGPUResources();

    // Getter functions
    std::vector<uint32_t> data();
    uint32_t size();
    std::array<uint32_t, KP_MAX_DIM_SIZE> shape();
    TensorTypes tensorType();
    bool isInit();

    // Setters
    void setData(const std::vector<uint32_t>& data);

    // Record functions
    void recordCopyFrom(std::shared_ptr<Tensor> copyFromTensor);
    // TODO: Explore simplifying by infering pipeline stage flag bits from
    // access flag bits (as seems to be superset)
    void recordBufferMemoryBarrier(vk::AccessFlagBits srcAccessMask,
                                   vk::AccessFlagBits dstAccessMask,
                                   vk::PipelineStageFlagBits srcStageMask,
                                   vk::PipelineStageFlagBits dstStageMask);

    // Util functions
    vk::DescriptorBufferInfo constructDescriptorBufferInfo();
    void mapDataFromHostMemory();
    void mapDataIntoHostMemory();

  private:
    std::shared_ptr<vk::PhysicalDevice> mPhysicalDevice;
    std::shared_ptr<vk::Device> mDevice;
    std::shared_ptr<vk::CommandBuffer> mCommandBuffer;

    std::shared_ptr<vk::Buffer> mBuffer;
    bool mFreeBuffer;
    std::shared_ptr<vk::DeviceMemory> mMemory;
    bool mFreeMemory;

    std::vector<uint32_t> mData;

    TensorTypes mTensorType = TensorTypes::eDevice;

    std::array<uint32_t, KP_MAX_DIM_SIZE> mShape; // TODO: Only 1D supported
    bool mIsInit = false;
    // uint32_t mDataType;

    // Private util functions
    vk::BufferUsageFlags getBufferUsageFlags();
    vk::MemoryPropertyFlags getMemoryPropertyFlags();
    uint64_t memorySize();
};

} // End namespace kp

namespace kp {

/**
 *  Base Operation which provides the high level interface that Kompute
 *  operations implement in order to perform a set of actions in the GPU.
 *
 *  Operations can perform actions on tensors, and optionally can also own an
 *  Algorithm with respective parameters. kp::Operations with kp::Algorithms
 *  would inherit from kp::OpBaseAlgo.
 */
class OpBase
{
  public:
    /**
     *  Base constructor, should not be used unless explicitly intended.
     */
    OpBase() { SPDLOG_DEBUG("Compute OpBase base constructor"); }

    /**
     *  Default constructor with parameters that provides the bare minimum
     * requirements for the operations to be able to create and manage their
     * sub-components.
     */
    OpBase(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
           std::shared_ptr<vk::Device> device,
           std::shared_ptr<vk::CommandBuffer> commandBuffer,
           std::vector<std::shared_ptr<Tensor>>& tensors,
           bool freeTensors)
    {
        SPDLOG_DEBUG("Compute OpBase constructor with params");

        this->mPhysicalDevice = physicalDevice;
        this->mDevice = device;
        this->mCommandBuffer = commandBuffer;
        this->mTensors = tensors;
    }

    /**
     * Default destructor for OpBase class. This OpBase destructor class should
     * always be called to destroy and free owned resources unless it is
     * intended to destroy the resources in the parent class. This can be done
     * by passing the mFreeTensors=false.
     */
    ~OpBase()
    {
        SPDLOG_DEBUG("Kompute OpBase destructor started");

        if (!this->mDevice) {
            spdlog::warn("Kompute OpBase destructor called with empty device");
            return;
        }

        if (this->mFreeTensors) {
            SPDLOG_DEBUG("Kompute OpBase freeing tensors");
            for (std::shared_ptr<Tensor> tensor : this->mTensors) {
                if (tensor && tensor->isInit()) {
                    tensor->freeMemoryDestroyGPUResources();
                } else {
                    spdlog::error("Kompute OpBase expected to free "
                                  "tensor but has already been freed.");
                }
            }
        }
    }

    virtual void init() = 0;

    virtual void record() = 0;

    virtual void postSubmit() = 0;

  protected:
    // Sometimes owned resources
    std::vector<std::shared_ptr<Tensor>> mTensors;
    bool mFreeTensors =
      false; // TODO: Provide granularity to specify which to free

    // Always external resources
    std::shared_ptr<vk::PhysicalDevice> mPhysicalDevice;
    std::shared_ptr<vk::Device> mDevice;
    std::shared_ptr<vk::CommandBuffer> mCommandBuffer;
};

} // End namespace kp

namespace kp {

/**
    Container of operations that can be sent to GPU as batch
*/
class Sequence
{
  public:
    /**
        Constructor
    */
    Sequence();
    Sequence(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
             std::shared_ptr<vk::Device> device,
             std::shared_ptr<vk::Queue> computeQueue,
             uint32_t queueIndex);
    ~Sequence();

    // Initialiser
    void init();

    // Record command functions
    void begin();
    void end();
    void eval();

    // TODO: Explore design without template using just top level class
    template<typename T, typename... TArgs>
    void record(std::vector<std::shared_ptr<Tensor>> tensors)
    {
        static_assert(std::is_base_of<OpBase, T>::value,
                      "Template only valid with OpBase derived classes");

        SPDLOG_DEBUG("Kompute Sequence record function started");

        SPDLOG_DEBUG("Kompute Sequence creating OpBase derived class instance");
        T* op = new T(
          this->mPhysicalDevice, this->mDevice, this->mCommandBuffer, tensors);
        OpBase* baseOp = dynamic_cast<OpBase*>(op);

        std::unique_ptr<OpBase> baseOpPtr{ baseOp };

        SPDLOG_DEBUG(
          "Kompute Sequence running init on OpBase derived class instance");
        baseOpPtr->init();

        SPDLOG_DEBUG(
          "Kompute Sequence running record on OpBase derived class instance");
        baseOpPtr->record();

        mOperations.push_back(std::move(baseOpPtr));
    }

  private:
    std::shared_ptr<vk::PhysicalDevice> mPhysicalDevice = nullptr;
    std::shared_ptr<vk::Device> mDevice = nullptr;
    std::shared_ptr<vk::Queue> mComputeQueue = nullptr;
    uint32_t mQueueIndex = -1;
    std::shared_ptr<vk::CommandPool> mCommandPool = nullptr;
    bool mFreeCommandPool = false;
    std::shared_ptr<vk::CommandBuffer> mCommandBuffer = nullptr;
    bool mFreeCommandBuffer = false;

    // Base op objects
    std::vector<std::unique_ptr<OpBase>> mOperations;

    // Record state
    bool mRecording = false;

    // Create functions
    void createCommandPool();
    void createCommandBuffer();
};

} // End namespace kp

#define KP_DEFAULT_SESSION "DEFAULT"

namespace kp {

/**
    Base orchestrator which creates and manages device and child components
*/
class Manager
{
  private:
  public:
    /**
        Constructor
    */
    Manager();

    Manager(std::shared_ptr<vk::Instance> instance,
            std::shared_ptr<vk::Device>,
            uint32_t queueIndex);

    ~Manager();

    std::weak_ptr<Sequence> getOrCreateManagedSequence(std::string sessionName);

    template<typename T, typename... TArgs>
    void evalOp(std::vector<std::shared_ptr<Tensor>> tensors, std::string sessionName = KP_DEFAULT_SESSION)
    {
        SPDLOG_DEBUG("Kompute Manager evalOp triggered");
        std::weak_ptr<Sequence> sqWeakPtr = 
            this->getOrCreateManagedSequence(sessionName);

        if (std::shared_ptr<kp::Sequence> sq = sqWeakPtr.lock()) 
        {
            SPDLOG_DEBUG("Kompute Manager evalOp running sequence BEGIN");
            sq->begin();

            SPDLOG_DEBUG("Kompute Manager evalOp running sequence RECORD");
            sq->record<T>(tensors);

            SPDLOG_DEBUG("Kompute Manager evalOp running sequence END");
            sq->end();

            SPDLOG_DEBUG("Kompute Manager evalOp running sequence EVAL");
            sq->eval();
        }
        SPDLOG_DEBUG("Kompute Manager evalOp running sequence SUCCESS");
    }

  private:

    std::shared_ptr<vk::Instance> mInstance = nullptr;
    bool mFreeInstance = false;
    std::shared_ptr<vk::PhysicalDevice> mPhysicalDevice = nullptr;
    uint32_t mPhysicalDeviceIndex = -1;
    std::shared_ptr<vk::Device> mDevice = nullptr;
    bool mFreeDevice = false;
    uint32_t mComputeQueueFamilyIndex = -1;
    std::shared_ptr<vk::Queue> mComputeQueue = nullptr;

    // Always owned resources
    std::unordered_map<std::string, std::shared_ptr<Sequence>> mManagedSequences;

#if DEBUG
    vk::DebugReportCallbackEXT mDebugReportCallback;
    vk::DispatchLoaderDynamic mDebugDispatcher;
#endif

    // Create functions
    void createInstance();
    void createDevice();
};

} // End namespace kp

#include <fstream>

namespace kp {

class Algorithm
{
  public:
    Algorithm();

    Algorithm(std::shared_ptr<vk::Device> device,
              std::shared_ptr<vk::CommandBuffer> commandBuffer);

    // TODO: Add specialisation data
    // TODO: Explore other ways of passing shader (ie raw bytes)
    void init(const std::vector<char>& shaderFileData,
              std::vector<std::shared_ptr<Tensor>> tensorParams);

    ~Algorithm();

    // Record commands
    void recordDispatch(uint32_t x = 1, uint32_t y = 1, uint32_t z = 1);

  private:
    // Never Owned Resources
    std::shared_ptr<vk::Device> mDevice;
    std::shared_ptr<vk::CommandBuffer> mCommandBuffer;

    // Optionally owned resources
    std::shared_ptr<vk::DescriptorSetLayout> mDescriptorSetLayout;
    bool mFreeDescriptorSetLayout = false;
    std::shared_ptr<vk::DescriptorPool> mDescriptorPool;
    bool mFreeDescriptorPool = false;

    // TODO: Explore design for multiple descriptor sets
    std::shared_ptr<vk::DescriptorSet> mDescriptorSet;
    bool mFreeDescriptorSet = false;
    std::shared_ptr<vk::ShaderModule> mShaderModule;
    bool mFreeShaderModule = false;
    std::shared_ptr<vk::PipelineLayout> mPipelineLayout;
    bool mFreePipelineLayout = false;
    std::shared_ptr<vk::PipelineCache> mPipelineCache;
    bool mFreePipelineCache = false;
    std::shared_ptr<vk::Pipeline> mPipeline;
    bool mFreePipeline = false;

    // Create util functions
    void createShaderModule(const std::vector<char>& shaderFileData);
    void createPipeline();

    // Parameters
    void createParameters(std::vector<std::shared_ptr<Tensor>>& tensorParams);
    void createDescriptorPool();
};

} // End namespace kp

namespace kp {

/**
    Base algorithm based operation
*/
template<uint32_t tX = 0, uint32_t tY = 0, uint32_t tZ = 0>
class OpMult : public OpBase
{
  public:
    /**
        Constructor
    */
    OpMult();

    OpMult(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
           std::shared_ptr<vk::Device> device,
           std::shared_ptr<vk::CommandBuffer> commandBuffer,
           std::vector<std::shared_ptr<Tensor>>& tensors,
           bool freeTensors = false);

    ~OpMult();

    void init() override;

    void record() override;

    void postSubmit() override;

  private:
    // Always owned resources
    std::shared_ptr<Tensor> mTensorOutputStaging;

    // Optionally owned resources
    std::shared_ptr<Algorithm> mAlgorithm;
    bool mFreeAlgorithm = false;

    // Never owned resources
    std::shared_ptr<Tensor> mTensorLHS;
    std::shared_ptr<Tensor> mTensorRHS;
    std::shared_ptr<Tensor> mTensorOutput;

    uint32_t mX;
    uint32_t mY;
    uint32_t mZ;
};

} // End namespace kp

// Including implemenation for template class
#ifndef OPMULT_CPP
#define OPMULT_CPP

namespace kp {

template<uint32_t tX, uint32_t tY, uint32_t tZ>
OpMult<tX, tY, tZ>::OpMult()
{
    SPDLOG_DEBUG("Kompute OpMult constructor base");
}

// TODO: Remove physicalDevice from main initialiser
template<uint32_t tX, uint32_t tY, uint32_t tZ>
OpMult<tX, tY, tZ>::OpMult(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
                           std::shared_ptr<vk::Device> device,
                           std::shared_ptr<vk::CommandBuffer> commandBuffer,
                           std::vector<std::shared_ptr<Tensor>>& tensors,
                           bool freeTensors)
  : OpBase(physicalDevice, device, commandBuffer, tensors, freeTensors)
{
    SPDLOG_DEBUG("Kompute OpMult constructor with params");

    this->mAlgorithm = std::make_shared<Algorithm>(device, commandBuffer);
}

template<uint32_t tX, uint32_t tY, uint32_t tZ>
OpMult<tX, tY, tZ>::~OpMult()
{
    SPDLOG_DEBUG("Kompute OpMult destructor started");
}

template<uint32_t tX, uint32_t tY, uint32_t tZ>
void
OpMult<tX, tY, tZ>::init()
{
    SPDLOG_DEBUG("Kompute OpMult init called");

    if (this->mTensors.size() < 3) {
        throw std::runtime_error(
          "Kompute OpMult called with less than 1 tensor");
    } else if (this->mTensors.size() > 3) {
        spdlog::warn("Kompute OpMult called with more than 3 this->mTensors");
    }

    this->mTensorLHS = this->mTensors[0];
    this->mTensorRHS = this->mTensors[1];
    this->mTensorOutput = this->mTensors[2];

    // The dispatch size is set up based on either explicitly provided template
    // parameters or by default it would take the shape and size of the tensors
    if (tX > 0) {
        // If at least the x value is provided we use mainly the parameters
        // provided
        this->mX = tX;
        this->mY = tY > 0 ? tY : 1;
        this->mZ = tZ > 0 ? tZ : 1;
    } else {
        // TODO: Fully support the full size dispatch using size for the shape
        this->mX = this->mTensorLHS->size();
        this->mY = 1;
        this->mZ = 1;
    }
    spdlog::info("Kompute OpMult dispatch size X: {}, Y: {}, Z: {}",
                 this->mX,
                 this->mY,
                 this->mZ);

    // TODO: Explore adding a validate function
    if (!(this->mTensorLHS->isInit() && this->mTensorRHS->isInit() &&
          this->mTensorOutput->isInit())) {
        throw std::runtime_error(
          "Kompute OpMult all tensor parameters must be initialised. LHS: " +
          std::to_string(this->mTensorLHS->isInit()) +
          " RHS: " + std::to_string(this->mTensorRHS->isInit()) +
          " Output: " + std::to_string(this->mTensorOutput->isInit()));
    }

    // TODO: Explore use-cases where tensors shouldn't be the same size, and how
    // to deal with those situations
    if (!(this->mTensorLHS->size() == this->mTensorRHS->size() &&
          this->mTensorRHS->size() == this->mTensorOutput->size())) {
        throw std::runtime_error(
          "Kompute OpMult all tensor parameters must be the same size LHS: " +
          std::to_string(this->mTensorLHS->size()) +
          " RHS: " + std::to_string(this->mTensorRHS->size()) +
          " Output: " + std::to_string(this->mTensorOutput->size()));
    }

    this->mTensorOutputStaging = std::make_shared<Tensor>(
      this->mTensorOutput->data(), Tensor::TensorTypes::eStaging);

    this->mTensorOutputStaging->init(
      this->mPhysicalDevice, this->mDevice, this->mCommandBuffer);

#if RELEASE
    std::vector<char> shaderFileData(
      shader_data::shaders_glsl_opmult_comp_spv,
      shader_data::shaders_glsl_opmult_comp_spv +
        kp::shader_data::shaders_glsl_opmult_comp_spv_len);
#else
    SPDLOG_DEBUG(
      "Kompute OpMult Running debug loading shaders directly from spirv file");

    // TODO: Move to utility function
    std::string shaderFilePath = "shaders/glsl/opmult.comp.spv";
    std::ifstream fileStream(shaderFilePath,
                             std::ios::binary | std::ios::in | std::ios::ate);

    size_t shaderFileSize = fileStream.tellg();
    fileStream.seekg(0, std::ios::beg);
    char* shaderDataRaw = new char[shaderFileSize];
    fileStream.read(shaderDataRaw, shaderFileSize);
    fileStream.close();

    std::vector<char> shaderFileData(shaderDataRaw,
                                     shaderDataRaw + shaderFileSize);
#endif

    SPDLOG_DEBUG("Kompute OpMult Initialising algorithm component");

    this->mAlgorithm->init(shaderFileData, this->mTensors);
}

template<uint32_t tX, uint32_t tY, uint32_t tZ>
void
OpMult<tX, tY, tZ>::record()
{
    SPDLOG_DEBUG("Kompute OpMult record called");

    // Barrier to ensure the data is finished writing to buffer memory
    this->mTensorLHS->recordBufferMemoryBarrier(
      vk::AccessFlagBits::eHostWrite,
      vk::AccessFlagBits::eShaderRead,
      vk::PipelineStageFlagBits::eHost,
      vk::PipelineStageFlagBits::eComputeShader);
    this->mTensorRHS->recordBufferMemoryBarrier(
      vk::AccessFlagBits::eHostWrite,
      vk::AccessFlagBits::eShaderRead,
      vk::PipelineStageFlagBits::eHost,
      vk::PipelineStageFlagBits::eComputeShader);

    this->mAlgorithm->recordDispatch(this->mX, this->mY, this->mZ);

    // Barrier to ensure the shader code is executed before buffer read
    this->mTensorLHS->recordBufferMemoryBarrier(
      vk::AccessFlagBits::eShaderWrite,
      vk::AccessFlagBits::eTransferRead,
      vk::PipelineStageFlagBits::eComputeShader,
      vk::PipelineStageFlagBits::eTransfer);
    this->mTensorOutput->recordBufferMemoryBarrier(
      vk::AccessFlagBits::eShaderWrite,
      vk::AccessFlagBits::eTransferRead,
      vk::PipelineStageFlagBits::eComputeShader,
      vk::PipelineStageFlagBits::eTransfer);

    this->mTensorOutputStaging->recordCopyFrom(this->mTensorOutput);

    // Buffer to ensure wait until data is copied to staging buffer
    this->mTensorLHS->recordBufferMemoryBarrier(
      vk::AccessFlagBits::eTransferWrite,
      vk::AccessFlagBits::eHostRead,
      vk::PipelineStageFlagBits::eTransfer,
      vk::PipelineStageFlagBits::eHost);
    this->mTensorOutput->recordBufferMemoryBarrier(
      vk::AccessFlagBits::eTransferWrite,
      vk::AccessFlagBits::eHostRead,
      vk::PipelineStageFlagBits::eTransfer,
      vk::PipelineStageFlagBits::eHost);
}

template<uint32_t tX, uint32_t tY, uint32_t tZ>
void
OpMult<tX, tY, tZ>::postSubmit()
{
    SPDLOG_DEBUG("Kompute OpMult postSubmit called");

    this->mTensorOutputStaging->mapDataFromHostMemory();

    this->mTensorOutput->setData(this->mTensorOutputStaging->data());
}

}

#endif // #ifndef OPMULT_CPP

namespace kp {

class OpCreateTensor : public OpBase
{
  public:
    OpCreateTensor();

    OpCreateTensor(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
                   std::shared_ptr<vk::Device> device,
                   std::shared_ptr<vk::CommandBuffer> commandBuffer,
                   std::vector<std::shared_ptr<Tensor>>& tensors,
                   bool freeTensors = true);

    ~OpCreateTensor();

    void init() override;

    void record() override;

    void postSubmit() override;

  private:
    // Never owned resources
    std::shared_ptr<Tensor> mPrimaryTensor;
    std::shared_ptr<Tensor> mStagingTensor;
};

} // End namespace kp
