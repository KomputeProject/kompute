
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

    SPDLOG_DEBUG("Kompute OpTensorCreate freeing staging tensors");
    for (std::shared_ptr<Tensor> tensor : this->mStagingTensors) {
        if (tensor && tensor->isInit()) {
            tensor->freeMemoryDestroyGPUResources();
        } else {
            SPDLOG_ERROR("Kompute OpTensorCreate expected to free "
                          "tensor but has already been freed.");
        }
    }
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
        if (tensor->tensorType() == Tensor::TensorTypes::eDevice) {
            tensor->init(this->mPhysicalDevice, this->mDevice);

            std::shared_ptr<Tensor> stagingTensor = std::make_shared<Tensor>(
              tensor->data(), Tensor::TensorTypes::eStaging);

            stagingTensor->init(this->mPhysicalDevice, this->mDevice);

            stagingTensor->mapDataIntoHostMemory();

            this->mStagingTensors.push_back(stagingTensor);

        } else {

            tensor->init(this->mPhysicalDevice, this->mDevice);

            tensor->mapDataIntoHostMemory();

            // We push a nullptr when no staging tensor is needed to match
            // index number in array to have one to one mapping with tensors
            this->mStagingTensors.push_back(nullptr);
        }
    }
}

void
OpTensorCreate::record()
{
    SPDLOG_DEBUG("Kompute OpTensorCreate record called");

    for (size_t i = 0; i < this->mTensors.size(); i++) {
        if (this->mTensors[i]->tensorType() == Tensor::TensorTypes::eDevice) {
            this->mTensors[i]->recordCopyFrom(
              this->mCommandBuffer, this->mStagingTensors[i], false);
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
