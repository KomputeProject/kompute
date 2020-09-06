
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
  std::vector<std::shared_ptr<Tensor>> tensors)
  : OpBase(physicalDevice, device, commandBuffer, tensors, true)
{
    SPDLOG_DEBUG("Kompute OpCreateTensor constructor with params");
}

OpCreateTensor::~OpCreateTensor()
{
    SPDLOG_DEBUG("Kompute OpCreateTensor destructor started");

    SPDLOG_DEBUG("Kompute OpCreateTensor destroying staging tensors");
    for (size_t i = 0; i < this->mStagingTensors.size(); i++) {
        if (this->mStagingTensors[i]) {
            this->mStagingTensors[i]->freeMemoryDestroyGPUResources();
        }
    }
}

void
OpCreateTensor::init()
{
    SPDLOG_DEBUG("Kompute OpCreateTensor init called");

    if (this->mTensors.size() < 1) {
        throw std::runtime_error(
          "Kompute OpCreateTensor called with less than 1 tensor");
    }

    for (std::shared_ptr<Tensor> tensor: this->mTensors) {
        if (tensor->isInit()) {
            throw std::runtime_error("Kompute OpCreateTensor: Tensor has already been initialized");
        }
        if (tensor->tensorType() == Tensor::TensorTypes::eDevice) {
            tensor->init(
              this->mPhysicalDevice, this->mDevice);

            std::shared_ptr<Tensor> stagingTensor = std::make_shared<Tensor>(
              tensor->data(), Tensor::TensorTypes::eStaging);

            stagingTensor->init(
              this->mPhysicalDevice, this->mDevice);

            stagingTensor->mapDataIntoHostMemory();

            this->mStagingTensors.push_back(stagingTensor);

        } else {

            tensor->init(
              this->mPhysicalDevice, this->mDevice);

            // We push a nullptr when no staging tensor is needed to match 
            // index number in array to have one to one mapping with tensors
            this->mStagingTensors.push_back(nullptr);
        }
    }
}

void
OpCreateTensor::record()
{
    SPDLOG_DEBUG("Kompute OpCreateTensor record called");

    for (size_t i = 0; i < this->mTensors.size(); i++) {
        if (this->mTensors[i]->tensorType() == Tensor::TensorTypes::eDevice) {
            this->mTensors[i]->recordCopyFrom(this->mCommandBuffer, this->mStagingTensors[i], false);
        } else if (this->mTensors[i]->tensorType() == Tensor::TensorTypes::eStaging) {
            this->mTensors[i]->mapDataIntoHostMemory();
        }
    }
}

void
OpCreateTensor::postSubmit()
{
    SPDLOG_DEBUG("Kompute OpCreateTensor postSubmit called");

    for (size_t i = 0; i < this->mTensors.size(); i++) {
        if (this->mTensors[i]->tensorType() == Tensor::TensorTypes::eDevice) {
            this->mStagingTensors[i]->mapDataFromHostMemory();

            this->mTensors[i]->setData(this->mStagingTensors[i]->data());
        }
    }
}

}
