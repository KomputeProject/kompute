
#include "Tensor.hpp"

#include "OpCreateTensor.hpp"

namespace kp {

OpCreateTensor::OpCreateTensor() {
    SPDLOG_DEBUG("Kompute OpCreateTensor constructor base");

}

OpCreateTensor::OpCreateTensor(std::shared_ptr<vk::PhysicalDevice> physicalDevice, 
                               std::shared_ptr<vk::Device> device,
                               std::shared_ptr<vk::CommandBuffer> commandBuffer)
  : OpBase(physicalDevice, device, commandBuffer)
{
    SPDLOG_DEBUG("Kompute OpCreateTensor constructor with params");
}

OpCreateTensor::~OpCreateTensor() {
    SPDLOG_DEBUG("Kompute OpCreateTensor destructor started");
}

void
OpCreateTensor::init(std::shared_ptr<Tensor> tensor, ...)
{
    SPDLOG_DEBUG("Kompute OpCreateTensor init called");

    this->mPrimaryTensor = tensor;
    std::vector<uint32_t> data = this->mPrimaryTensor->data();

    if (tensor->tensorType() == Tensor::TensorTypes::eDevice) {
        tensor->init(this->mPhysicalDevice, this->mDevice, this->mCommandBuffer);

        this->mStagingTensor = std::make_shared<Tensor>(tensor->data(), Tensor::TensorTypes::eStaging);

        this->mStagingTensor->init(this->mPhysicalDevice, this->mDevice, this->mCommandBuffer, data);

    }
    else {
        tensor->init(this->mPhysicalDevice, this->mDevice, this->mCommandBuffer, data);
    }
}

void
OpCreateTensor::record()
{
    SPDLOG_DEBUG("Kompute OpCreateTensor record called");

    if (this->mPrimaryTensor->tensorType() == Tensor::TensorTypes::eDevice) {
        this->mPrimaryTensor->recordCopyFrom(this->mStagingTensor);
    }
}

}
