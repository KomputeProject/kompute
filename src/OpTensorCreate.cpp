
#include "kompute/Tensor.hpp"

#include "kompute/operations/OpTensorCreate.hpp"

namespace kp {

OpTensorCreate::OpTensorCreate()
{
    SPDLOG_DEBUG("Kompute OpTensorCreate constructor base");
}

OpTensorCreate::OpTensorCreate(
  std::shared_ptr<vk::PhysicalDevice> physicalDevice,
  std::shared_ptr<vk::Device> device,
  std::shared_ptr<vk::CommandBuffer> commandBuffer,
  std::vector<std::shared_ptr<Tensor>> tensors)
  : OpBase(physicalDevice, device, commandBuffer, tensors, true)
{
    SPDLOG_DEBUG("Kompute OpTensorCreate constructor with params");
}

OpTensorCreate::~OpTensorCreate()
{
    SPDLOG_DEBUG("Kompute OpTensorCreate destructor started");
}

void
OpTensorCreate::init()
{
    SPDLOG_DEBUG("Kompute OpTensorCreate init called");

    if (this->mTensors.size() < 1) {
        throw std::runtime_error(
          "Kompute OpTensorCreate called with less than 1 tensor");
    }

    for (std::shared_ptr<Tensor> tensor : this->mTensors) {
        if (tensor->isInit()) {
            throw std::runtime_error(
              "Kompute OpTensorCreate: Tensor has already been initialized");
        }
        if (tensor->tensorType() != Tensor::TensorTypes::eStorage) {
            tensor->init(this->mPhysicalDevice, this->mDevice);

            tensor->mapDataIntoHostMemory();
        }
    }
}

void
OpTensorCreate::record()
{
    SPDLOG_DEBUG("Kompute OpTensorCreate record called");

    for (size_t i = 0; i < this->mTensors.size(); i++) {
        if (this->mTensors[i]->tensorType() == Tensor::TensorTypes::eDevice) {
            this->mTensors[i]->recordCopyFromStagingToDevice(
              this->mCommandBuffer, false);
        }
    }
}

void
OpTensorCreate::preEval()
{
    SPDLOG_DEBUG("Kompute OpTensorCreate preEval called");
}

void
OpTensorCreate::postEval()
{
    SPDLOG_DEBUG("Kompute OpTensorCreate postEval called");
}

}
