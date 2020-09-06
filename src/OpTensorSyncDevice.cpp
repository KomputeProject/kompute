
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

    SPDLOG_DEBUG("Kompute OpTensorSyncDevice destroying staging tensors");
}

void
OpTensorSyncDevice::init()
{
    SPDLOG_DEBUG("Kompute OpTensorSyncDevice init called");

    if (this->mTensors.size() < 2) {
        throw std::runtime_error(
          "Kompute OpTensorSyncDevice called with less than 2 tensor");
    }

    for (std::shared_ptr<Tensor> tensor: this->mTensors) {
        if (!tensor->isInit()) {
            throw std::runtime_error("Kompute OpTensorSyncDevice tensor parameter has not been initialized");
        }
        if (tensor->tensorType() == Tensor::TensorTypes::eStorage) {
            throw std::runtime_error("Kompute OpTensorSyncDevice tensor parameter is of type storage and hence cannot be used to receive or pass data.");
        }
    }
}

void
OpTensorSyncDevice::record()
{
    SPDLOG_DEBUG("Kompute OpTensorSyncDevice record called");

    // We iterate from the second tensor onwards and record a copy to all
    for (size_t i = 1; i < this->mTensors.size(); i++) {
        this->mTensors[i]->recordCopyFrom(this->mCommandBuffer, this->mTensors[0], false);
    }
}

void
OpTensorSyncDevice::postSubmit()
{
    SPDLOG_DEBUG("Kompute OpTensorSyncDevice postSubmit called");

    // Copy the data from the first tensor into all the tensors
    for (size_t i = 1; i < this->mTensors.size(); i++) {
        this->mTensors[i]->setData(this->mTensors[0]->data());
    }
}

}

