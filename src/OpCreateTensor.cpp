
#include "kompute/Tensor.hpp"

#include "kompute/operations/OpCreateTensor.hpp"

namespace kp {

OpCreateTensor::OpCreateTensor()
{
    SPDLOG_DEBUG("Kompute OpCreateTensor constructor base");
}

OpCreateTensor::OpCreateTensor(
  std::shared_ptr<vk::PhysicalDevice> physicalDevice,
  std::shared_ptr<vk::Device> device,
  std::shared_ptr<vk::CommandBuffer> commandBuffer,
  std::vector<std::shared_ptr<Tensor>>& tensors,
  bool freeTensors)
  : OpBase(physicalDevice, device, commandBuffer, tensors, freeTensors)
{
    SPDLOG_DEBUG("Kompute OpCreateTensor constructor with params");
}

OpCreateTensor::~OpCreateTensor()
{
    SPDLOG_DEBUG("Kompute OpCreateTensor destructor started");
}

void
OpCreateTensor::init()
{
    SPDLOG_DEBUG("Kompute OpCreateTensor init called");

    if (this->mTensors.size() < 1) {
        throw std::runtime_error(
          "Kompute OpCreateTensor called with less than 1 tensor");
    } else if (this->mTensors.size() > 1) {
        spdlog::warn("Kompute OpCreateTensor called with more than 1 tensor");
    }

    this->mPrimaryTensor = this->mTensors[0];

    if (this->mPrimaryTensor->tensorType() == Tensor::TensorTypes::eDevice) {
        this->mPrimaryTensor->init(
          this->mPhysicalDevice, this->mDevice, this->mCommandBuffer);

        this->mStagingTensor = std::make_shared<Tensor>(
          this->mPrimaryTensor->data(), Tensor::TensorTypes::eStaging);

        this->mStagingTensor->init(
          this->mPhysicalDevice, this->mDevice, this->mCommandBuffer);

        this->mStagingTensor->mapDataIntoHostMemory();

        // Adding to the OpBase owned resource so they are freed
        this->mTensors.push_back(this->mStagingTensor);

    } else {
        this->mPrimaryTensor->init(
          this->mPhysicalDevice, this->mDevice, this->mCommandBuffer);
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

void
OpCreateTensor::postSubmit()
{
    SPDLOG_DEBUG("Kompute OpCreateTensor postSubmit called");

    this->mStagingTensor->mapDataFromHostMemory();

    this->mPrimaryTensor->setData(this->mStagingTensor->data());
}

}
