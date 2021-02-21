
#include "kompute/Tensor.hpp"

#include "kompute/operations/OpTensorSyncDevice.hpp"

namespace kp {

OpTensorSyncDevice::OpTensorSyncDevice()
{
    KP_LOG_DEBUG("Kompute OpTensorSyncDevice constructor base");
}

OpTensorSyncDevice::OpTensorSyncDevice(
  std::shared_ptr<vk::PhysicalDevice> physicalDevice,
  std::shared_ptr<vk::Device> device,
  std::shared_ptr<vk::CommandBuffer> commandBuffer,
  std::vector<std::shared_ptr<Tensor>> tensors)
  : OpBase(physicalDevice, device, commandBuffer, tensors)
{
    KP_LOG_DEBUG("Kompute OpTensorSyncDevice constructor with params");
}

OpTensorSyncDevice::~OpTensorSyncDevice()
{
    KP_LOG_DEBUG("Kompute OpTensorSyncDevice destructor started");
}

void
OpTensorSyncDevice::init()
{
    KP_LOG_DEBUG("Kompute OpTensorSyncDevice init called");

    if (this->mTensors.size() < 1) {
        throw std::runtime_error(
          "Kompute OpTensorSyncDevice called with less than 1 tensor");
    }

    for (std::shared_ptr<Tensor> tensor : this->mTensors) {
        if (!tensor->isInit()) {
            throw std::runtime_error("Kompute OpTensorSyncDevice: Tensor param "
                                     "has not been initialized");
        }
        if (tensor->tensorType() == Tensor::TensorTypes::eStorage) {
            KP_LOG_WARN(
              "Kompute OpTensorSyncLocal tensor parameter is of type "
              "TensorTypes::eStorage and hence cannot be used to receive or "
              "pass data.");
        }
    }
}

void
OpTensorSyncDevice::record()
{
    KP_LOG_DEBUG("Kompute OpTensorSyncDevice record called");

    for (size_t i = 0; i < this->mTensors.size(); i++) {
        if (this->mTensors[i]->tensorType() == Tensor::TensorTypes::eDevice) {
            this->mTensors[i]->recordCopyFromStagingToDevice(
              this->mCommandBuffer, false);
        }
    }
}

void
OpTensorSyncDevice::preEval()
{
    KP_LOG_DEBUG("Kompute OpTensorSyncDevice preEval called");

    // Performing sync of data as eval can be called multiple times with same op
    for (size_t i = 0; i < this->mTensors.size(); i++) {
        if (this->mTensors[i]->tensorType() != Tensor::TensorTypes::eStorage) {
            this->mTensors[i]->mapDataIntoHostMemory();
        }
    }
}

void
OpTensorSyncDevice::postEval()
{
    KP_LOG_DEBUG("Kompute OpTensorSyncDevice postEval called");
}

}
