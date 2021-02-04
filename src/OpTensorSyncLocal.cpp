
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
  : OpBase(physicalDevice, device, commandBuffer, tensors, false)
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
            throw std::runtime_error(
              "Kompute OpTensorSyncLocal tensor parameter is of type "
              "TensorTypes::eStorage and hence cannot be used to receive or "
              "pass data.");
        }
        if (tensor->tensorType() == Tensor::TensorTypes::eDevice) {

            std::shared_ptr<Tensor> stagingTensor = std::make_shared<Tensor>(
              tensor->data_sp(), Tensor::TensorTypes::eStaging);

            stagingTensor->init(this->mPhysicalDevice, this->mDevice);

            this->mStagingTensors.push_back(stagingTensor);

        } else {

            // We push a nullptr when no staging tensor is needed to match
            // index number in array to have one to one mapping with tensors
            this->mStagingTensors.push_back(nullptr);
        }
    }
}

void
OpTensorSyncLocal::record()
{
    SPDLOG_DEBUG("Kompute OpTensorSyncLocal record called");

    for (size_t i = 0; i < this->mTensors.size(); i++) {
        if (this->mTensors[i]->tensorType() == Tensor::TensorTypes::eDevice) {
            this->mStagingTensors[i]->recordCopyFrom(
              this->mCommandBuffer, this->mTensors[i], true);
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
        if (this->mTensors[i]->tensorType() == Tensor::TensorTypes::eDevice) {
            this->mStagingTensors[i]->mapDataFromHostMemory();
            this->mTensors[i]->setData(this->mStagingTensors[i]->data());
        } else {
            this->mTensors[i]->mapDataFromHostMemory();
        }
    }
}

}
