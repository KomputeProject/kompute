
#include "kompute/operations/OpTensorCopy.hpp"

namespace kp {

OpTensorCopy::OpTensorCopy(const std::vector<std::shared_ptr<Tensor>>& tensors)
{
    KP_LOG_DEBUG("Kompute OpTensorCopy constructor with params");

    this->mTensors = tensors;

    if (this->mTensors.size() < 2) {
        throw std::runtime_error(
          "Kompute OpTensorCopy called with less than 2 tensor");
    }

    kp::Tensor::TensorDataTypes dataType = this->mTensors[0]->dataType();
    uint32_t size = this->mTensors[0]->size();
    for (const std::shared_ptr<Tensor>& tensor : tensors) {
        if (tensor->dataType() != dataType) {
            throw std::runtime_error(fmt::format(
              "Attempting to copy tensors of different types from {} to {}",
              dataType,
              tensor->dataType()));
        }
        if (tensor->size() != size) {
            throw std::runtime_error(fmt::format(
              "Attempting to copy tensors of different sizes from {} to {}",
              size,
              tensor->size()));
        }
    }
}

OpTensorCopy::~OpTensorCopy()
{
    KP_LOG_DEBUG("Kompute OpTensorCopy destructor started");
}

void
OpTensorCopy::record(const vk::CommandBuffer& commandBuffer)
{
    KP_LOG_DEBUG("Kompute OpTensorCopy record called");

    // We iterate from the second tensor onwards and record a copy to all
    for (size_t i = 1; i < this->mTensors.size(); i++) {
        this->mTensors[i]->recordCopyFrom(commandBuffer, this->mTensors[0]);
    }
}

void
OpTensorCopy::preEval(const vk::CommandBuffer& commandBuffer)
{
    KP_LOG_DEBUG("Kompute OpTensorCopy preEval called");
}

void
OpTensorCopy::postEval(const vk::CommandBuffer& commandBuffer)
{
    KP_LOG_DEBUG("Kompute OpTensorCopy postEval called");

    void* data = this->mTensors[0]->rawData();

    // Copy the data from the first tensor into all the tensors
    for (size_t i = 1; i < this->mTensors.size(); i++) {
        this->mTensors[i]->setRawData(data);
    }
}

}
