
#include "kompute/Tensor.hpp"

#include "kompute/operations/OpTensorSyncDevice.hpp"

namespace kp {

OpTensorSyncDevice::OpTensorSyncDevice()
{
    SPDLOG_DEBUG("Kompute OpTensorSyncDevice constructor base");
}

OpTensorSyncDevice::OpTensorSyncDevice(
  std::shared_ptr<vk::PhysicalDevice> physicalDevice,
  std::shared_ptr<vk::Device> device,
  std::shared_ptr<vk::CommandBuffer> commandBuffer,
  std::vector<std::shared_ptr<Tensor>> tensors)
  : OpBase(physicalDevice, device, commandBuffer, tensors, false)
{
    SPDLOG_DEBUG("Kompute OpTensorSyncDevice constructor with params");
}

OpTensorSyncDevice::~OpTensorSyncDevice()
{
    SPDLOG_DEBUG("Kompute OpTensorSyncDevice destructor started");
}

void
OpTensorSyncDevice::init()
{
    SPDLOG_DEBUG("Kompute OpTensorSyncDevice init called");

    if (this->mTensors.size() < 1) {
        throw std::runtime_error(
          "Kompute OpTensorSyncDevice called with less than 1 tensor");
    }

    for (std::shared_ptr<Tensor> tensor: this->mTensors) {
        if (!tensor->isInit()) {
            throw std::runtime_error("Kompute OpTensorSyncDevice: Tensor param has not been initialized");
        }
        if (tensor->tensorType() == Tensor::TensorTypes::eStorage) {
            throw std::runtime_error("Kompute OpTensorSyncLocal tensor parameter is of type TensorTypes::eStorage and hence cannot be used to receive or pass data.");
        }
        if (tensor->tensorType() == Tensor::TensorTypes::eDevice) {

            std::shared_ptr<Tensor> stagingTensor = std::make_shared<Tensor>(
              tensor->data(), Tensor::TensorTypes::eStaging);

            stagingTensor->init(
              this->mPhysicalDevice, this->mDevice);

            this->mStagingTensors.push_back(stagingTensor);

        } else {
            // We push a nullptr when no staging tensor is needed to match 
            // index number in array to have one to one mapping with tensors
            this->mStagingTensors.push_back(nullptr);
        }
    }
}

void
OpTensorSyncDevice::record()
{
    SPDLOG_DEBUG("Kompute OpTensorSyncDevice record called");

    for (size_t i = 0; i < this->mTensors.size(); i++) {
        if (this->mTensors[i]->tensorType() == Tensor::TensorTypes::eDevice) {
            this->mTensors[i]->recordCopyFrom(this->mCommandBuffer, this->mStagingTensors[i], false);
        }
    }
}

void
OpTensorSyncDevice::preEval()
{
    SPDLOG_DEBUG("Kompute OpTensorSyncDevice preEval called");

    // Performing sync of data as eval can be called multiple times with same op
    for (size_t i = 0; i < this->mTensors.size(); i++) {
        if (this->mTensors[i]->tensorType() == Tensor::TensorTypes::eDevice) {
            this->mStagingTensors[i]->setData(this->mTensors[i]->data());
            this->mStagingTensors[i]->mapDataIntoHostMemory();
        } else {
            this->mTensors[i]->mapDataFromHostMemory();
        }
    }
}

void
OpTensorSyncDevice::postEval()
{
    SPDLOG_DEBUG("Kompute OpTensorSyncDevice postEval called");
}

}
