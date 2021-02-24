
#include "kompute/Tensor.hpp"

#include "kompute/operations/OpTensorSyncLocal.hpp"

namespace kp {

OpTensorSyncLocal::OpTensorSyncLocal(
  std::vector<std::shared_ptr<Tensor>> tensors)
  : OpBase(tensors, nullptr)
{
    KP_LOG_DEBUG("Kompute OpTensorSyncLocal constructor with params");

    this->mManagesTensors = false;
    this->mManagesAlgorithm = false;
}

OpTensorSyncLocal::~OpTensorSyncLocal()
{
    KP_LOG_DEBUG("Kompute OpTensorSyncLocal destructor started");
}

void
OpTensorSyncLocal::init(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
            std::shared_ptr<vk::Device> device)
{
    KP_LOG_DEBUG("Kompute OpTensorSyncLocal init called");

    if (this->mTensors.size() < 1) {
        throw std::runtime_error(
          "Kompute OpTensorSyncLocal called with less than 1 tensor");
    }

    for (std::shared_ptr<Tensor> tensor : this->mTensors) {
        if (!tensor->isInit()) {
            throw std::runtime_error(
              "Kompute OpTensorSyncLocal: Tensor has not been initialized");
        }
    }
}

void
OpTensorSyncLocal::record(std::shared_ptr<vk::CommandBuffer> commandBuffer)
{
    KP_LOG_DEBUG("Kompute OpTensorSyncLocal record called");

    for (size_t i = 0; i < this->mTensors.size(); i++) {
        if (this->mTensors[i]->tensorType() == Tensor::TensorTypes::eDevice) {
            this->mTensors[i]->recordCopyFromDeviceToStaging(
              commandBuffer, true);
        }
    }
}

void
OpTensorSyncLocal::preEval()
{
    KP_LOG_DEBUG("Kompute OpTensorSyncLocal preEval called");
}

void
OpTensorSyncLocal::postEval()
{
    KP_LOG_DEBUG("Kompute OpTensorSyncLocal postEval called");

    KP_LOG_DEBUG("Kompute OpTensorSyncLocal mapping data into tensor local");
    for (size_t i = 0; i < this->mTensors.size(); i++) {
        if (this->mTensors[i]->tensorType() != Tensor::TensorTypes::eStorage) {
            this->mTensors[i]->mapDataFromHostMemory();
        }
    }
}

}
