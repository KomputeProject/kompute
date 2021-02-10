
#include "kompute/Tensor.hpp"

#include "kompute/operations/OpTensorSyncLocal.hpp"

namespace kp {

OpTensorSyncLocal::OpTensorSyncLocal()
{
    SPDLOG_DEBUG("Kompute OpTensorSyncLocal constructor base");
}

OpTensorSyncLocal::OpTensorSyncLocal(
  std::shared_ptr<vk::PhysicalDevice> physicalDevice,
  std::shared_ptr<vk::Device> device,
  std::shared_ptr<vk::CommandBuffer> commandBuffer,
  std::vector<std::shared_ptr<Tensor>> tensors)
  : OpBase(physicalDevice, device, commandBuffer, tensors)
{
    SPDLOG_DEBUG("Kompute OpTensorSyncLocal constructor with params");
}

OpTensorSyncLocal::~OpTensorSyncLocal()
{
    SPDLOG_DEBUG("Kompute OpTensorSyncLocal destructor started");
}

void
OpTensorSyncLocal::init()
{
    SPDLOG_DEBUG("Kompute OpTensorSyncLocal init called");

    if (this->mTensors.size() < 1) {
        throw std::runtime_error(
          "Kompute OpTensorSyncLocal called with less than 1 tensor");
    }

    for (std::shared_ptr<Tensor> tensor : this->mTensors) {
        if (!tensor->isInit()) {
            throw std::runtime_error(
              "Kompute OpTensorSyncLocal: Tensor has not been initialized");
        }
        if (tensor->tensorType() == Tensor::TensorTypes::eStorage) {
            SPDLOG_WARN(
              "Kompute OpTensorSyncLocal tensor parameter is of type "
              "TensorTypes::eStorage and hence cannot be used to receive or "
              "pass data.");
        }
    }
}

void
OpTensorSyncLocal::record()
{
    SPDLOG_DEBUG("Kompute OpTensorSyncLocal record called");

    for (size_t i = 0; i < this->mTensors.size(); i++) {
        if (this->mTensors[i]->tensorType() == Tensor::TensorTypes::eDevice) {
            this->mTensors[i]->recordCopyFromDeviceToStaging(
              this->mCommandBuffer, true);
        }
    }
}

void
OpTensorSyncLocal::preEval()
{
    SPDLOG_DEBUG("Kompute OpTensorSyncLocal preEval called");
}

void
OpTensorSyncLocal::postEval()
{
    SPDLOG_DEBUG("Kompute OpTensorSyncLocal postEval called");

    SPDLOG_DEBUG("Kompute OpTensorSyncLocal mapping data into tensor local");
    for (size_t i = 0; i < this->mTensors.size(); i++) {
        if (this->mTensors[i]->tensorType() != Tensor::TensorTypes::eStorage) {
            this->mTensors[i]->mapDataFromHostMemory();
        }
    }
}

}
