
#include "Tensor.hpp"

#include "OpCreateTensor.hpp"

namespace kp {

OpCreateTensor::OpCreateTensor()
{
    SPDLOG_DEBUG("Kompute OpCreateTensor constructor base");
}

OpCreateTensor::OpCreateTensor(
  std::shared_ptr<vk::PhysicalDevice> physicalDevice,
  std::shared_ptr<vk::Device> device,
  std::shared_ptr<vk::CommandBuffer> commandBuffer)
  : OpBase(physicalDevice, device, commandBuffer)
{
    SPDLOG_DEBUG("Kompute OpCreateTensor constructor with params");
}

OpCreateTensor::~OpCreateTensor()
{
    SPDLOG_DEBUG("Kompute OpCreateTensor destructor started");
}

void
OpCreateTensor::init(std::vector<std::shared_ptr<Tensor>> tensors)
{
    SPDLOG_DEBUG("Kompute OpCreateTensor init called");

    if (tensors.size() < 1) {
        throw std::runtime_error(
          "Kompute OpCreateTensor called with less than 1 tensor");
    } else if (tensors.size() > 1) {
        spdlog::warn("Kompute OpCreateTensor called with more than 1 tensor");
    }

    this->mPrimaryTensor = tensors[0];
    std::vector<uint32_t> data = this->mPrimaryTensor->data();

    if (this->mPrimaryTensor->tensorType() == Tensor::TensorTypes::eDevice) {
        this->mPrimaryTensor->init(
          this->mPhysicalDevice, this->mDevice, this->mCommandBuffer);

        this->mStagingTensor = std::make_shared<Tensor>(
          this->mPrimaryTensor->data(), Tensor::TensorTypes::eStaging);

        this->mStagingTensor->init(
          this->mPhysicalDevice, this->mDevice, this->mCommandBuffer, data);

    } else {
        this->mPrimaryTensor->init(
          this->mPhysicalDevice, this->mDevice, this->mCommandBuffer, data);
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
