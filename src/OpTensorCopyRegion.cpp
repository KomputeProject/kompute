// SPDX-License-Identifier: Apache-2.0

#include "kompute/operations/OpTensorCopyRegion.hpp"
#include "kompute/Tensor.hpp"

namespace kp {

OpTensorCopyRegion::OpTensorCopyRegion(const TensorCopyRegions regions)
{
    KP_LOG_DEBUG("Kompute OpTensorCopyRegion constructor with params");

    if (regions.dstRegions.size() < 1) {
        throw std::runtime_error(
          "Kompute OpTensorCopyRegion called with no destination region");
    }

    kp::Tensor::TensorDataTypes dataType = regions.srcTensor->dataType();
    for (const TensorRegion& region : regions.dstRegions) {
        if (region.tensor->dataType() != dataType) {
            throw std::runtime_error(fmt::format(
              "Kompute OpTensorCopyRegion called with different types from {} to {}",
              Tensor::toString(dataType),
              Tensor::toString(region.tensor->dataType())));
        }
        if (region.elemCount == 0) {
            throw std::runtime_error(
                "Kompute OpTensorCopyRegion called with elemCount == 0");
        }
        if (region.srcIndex + region.elemCount > regions.srcTensor->size()) {
            throw std::runtime_error(
                "Kompute OpTensorCopyRegion called with out of bounds source region");
        }
        if (region.dstIndex  + region.elemCount > region.tensor->size()) {
            throw std::runtime_error(
                "Kompute OpTensorCopyRegion called with out of bounds destination region");
        }     
    }

    this->mRegions = regions;
}

OpTensorCopyRegion::~OpTensorCopyRegion()
{
    KP_LOG_DEBUG("Kompute OpTensorCopyRegion destructor started");
}

void
OpTensorCopyRegion::record(const vk::CommandBuffer& commandBuffer)
{
    KP_LOG_DEBUG("Kompute OpTensorCopyRegion record called");

    for (size_t i = 0; i < this->mRegions.dstRegions.size(); i++) {
        const uint32_t dataTypeMemorySize = this->mRegions.dstRegions[i].tensor->dataTypeMemorySize();
        const vk::BufferCopy copy = {
            dataTypeMemorySize * this->mRegions.dstRegions[i].srcIndex,
            dataTypeMemorySize * this->mRegions.dstRegions[i].dstIndex,
            dataTypeMemorySize * this->mRegions.dstRegions[i].elemCount,
        };
        this->mRegions.dstRegions[i].tensor->recordCopyFrom(commandBuffer, this->mRegions.srcTensor, copy);
    }
}

void
OpTensorCopyRegion::preEval(const vk::CommandBuffer& /*commandBuffer*/)
{
    KP_LOG_DEBUG("Kompute OpTensorCopyRegion preEval called");
}

void
OpTensorCopyRegion::postEval(const vk::CommandBuffer& /*commandBuffer*/)
{
    KP_LOG_DEBUG("Kompute OpTensorCopyRegion postEval called");
}

}
