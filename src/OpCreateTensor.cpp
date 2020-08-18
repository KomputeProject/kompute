
#include "Tensor.hpp"

#include "OpCreateTensor.hpp"

namespace kp {

OpCreateTensor::OpCreateTensor() {}

OpCreateTensor::OpCreateTensor(std::shared_ptr<vk::PhysicalDevice> physicalDevice, 
                               std::shared_ptr<vk::Device> device,
                               std::shared_ptr<vk::CommandBuffer> commandBuffer)
  : BaseOp(physicalDevice, device, commandBuffer)
{

}

OpCreateTensor::~OpCreateTensor() {

}

void
OpCreateTensor::init(std::shared_ptr<Tensor> tensor, std::vector<uint32_t> data)
{
    this->mPrimaryTensor = tensor;

    if (tensor->tensorType() == Tensor::TensorTypes::eDevice) {
        tensor->init(this->mPhysicalDevice, this->mDevice, this->mCommandBuffer);

        this->mStagingTensor = std::make_unique<Tensor>(tensor->shape(), Tensor::TensorTypes::eStaging);

        this->mStagingTensor->init(this->mPhysicalDevice, this->mDevice, this->mCommandBuffer, data);

    }
    else {
        tensor->init(this->mPhysicalDevice, this->mDevice, this->mCommandBuffer, data);
    }
}

void
OpCreateTensor::record()
{
    if (this->mPrimaryTensor->tensorType() == Tensor::TensorTypes::eDevice) {
        this->mPrimaryTensor->recordCopyFrom(this->mStagingTensor);
    }
}

}
