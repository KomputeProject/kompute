// SPDX-License-Identifier: Apache-2.0

#include "kompute/operations/OpTensorSyncRegionDevice.hpp"

namespace kp {

OpTensorSyncRegionDevice::OpTensorSyncRegionDevice(
  const std::vector<TensorRegion>& regions)
{
    KP_LOG_DEBUG("Kompute OpTensorSyncRegionDevice constructor with params");

    if (regions.size() < 1) {
        throw std::runtime_error(
          "Kompute OpTensorSyncRegionDevice called with less than 1 tensor region");
    }

    for (size_t i = 0; i < regions.size(); i++) {
        if (regions[i].elemCount == 0) {
            throw std::runtime_error(
            "Kompute OpTensorSyncRegionDevice called with elemCount == 0");
        }
        if (regions[i].srcIndex + regions[i].elemCount > regions[i].tensor->size()) {
            throw std::runtime_error(
            "Kompute OpTensorSyncRegionDevice called with out of bounds source region");
        }
        if (regions[i].dstIndex  + regions[i].elemCount > regions[i].tensor->size()) {
            throw std::runtime_error(
            "Kompute OpTensorSyncRegionDevice called with out of bounds destination region");
        }        
    }

    this->mRegions = regions;
}

OpTensorSyncRegionDevice::~OpTensorSyncRegionDevice()
{
    KP_LOG_DEBUG("Kompute OpTensorSyncRegionDevice destructor started");

    this->mRegions.clear();
}

void
OpTensorSyncRegionDevice::record(const vk::CommandBuffer& commandBuffer)
{
    KP_LOG_DEBUG("Kompute OpTensorSyncRegionDevice record called");

    for (size_t i = 0; i < this->mRegions.size(); i++) {
        if (this->mRegions[i].tensor->tensorType() == Tensor::TensorTypes::eDevice) {
            const uint32_t dataTypeMemorySize = this->mRegions[i].tensor->dataTypeMemorySize();
            const vk::BufferCopy copy = {
                dataTypeMemorySize * this->mRegions[i].srcIndex,
                dataTypeMemorySize * this->mRegions[i].dstIndex,
                dataTypeMemorySize * this->mRegions[i].elemCount,
            };
            this->mRegions[i].tensor->recordCopyFromStagingToDevice(commandBuffer, copy);
        }
    }
}

void
OpTensorSyncRegionDevice::preEval(const vk::CommandBuffer& /*commandBuffer*/)
{
    KP_LOG_DEBUG("Kompute OpTensorSyncRegionDevice preEval called");
}

void
OpTensorSyncRegionDevice::postEval(const vk::CommandBuffer& /*commandBuffer*/)
{
    KP_LOG_DEBUG("Kompute OpTensorSyncRegionDevice postEval called");
}

}
