
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
        this->mTensors[i]->recordCopyFrom(
          commandBuffer, this->mTensors[0], false);
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

    // Copy the data from the first tensor into all the tensors
    for (size_t i = 1; i < this->mTensors.size(); i++) {
        this->mTensors[i]->setData(this->mTensors[0]->data());
    }
}

}
