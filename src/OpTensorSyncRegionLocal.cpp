// SPDX-License-Identifier: Apache-2.0

#include "kompute/Tensor.hpp"

#include "kompute/operations/OpTensorSyncRegionLocal.hpp"

namespace kp {

OpTensorSyncRegionLocal::OpTensorSyncRegionLocal(
  const std::vector<TensorRegion>& regions)
{
    KP_LOG_DEBUG("Kompute OpTensorSyncRegionLocal constructor with params");

    if (regions.size() < 1) {
        throw std::runtime_error(
          "Kompute OpTensorSyncRegionLocal called with less than 1 region");
    }

    for (size_t i = 0; i < regions.size(); i++) {
        if (regions[i].elemCount == 0) {
            throw std::runtime_error(
            "Kompute OpTensorSyncRegionLocal called with elemCount == 0");
        }
        if (regions[i].srcIndex + regions[i].elemCount > regions[i].tensor->size()) {
            throw std::runtime_error(
            "Kompute OpTensorSyncRegionLocal called with out of bounds source region");
        }
        if (regions[i].dstIndex  + regions[i].elemCount > regions[i].tensor->size()) {
            throw std::runtime_error(
            "Kompute OpTensorSyncRegionLocal called with out of bounds destination region");
        }        
    }

    this->mRegions = regions;
}

OpTensorSyncRegionLocal::~OpTensorSyncRegionLocal()
{
    KP_LOG_DEBUG("Kompute OpTensorSyncRegionLocal destructor started");
}

void
OpTensorSyncRegionLocal::record(const vk::CommandBuffer& commandBuffer)
{
    KP_LOG_DEBUG("Kompute OpTensorSyncRegionLocal record called");

    for (size_t i = 0; i < this->mRegions.size(); i++) {
        if (this->mRegions[i].tensor->tensorType() == Tensor::TensorTypes::eDevice) {

            this->mRegions[i].tensor->recordPrimaryBufferMemoryBarrier(
              commandBuffer,
              vk::AccessFlagBits::eShaderWrite,
              vk::AccessFlagBits::eTransferRead,
              vk::PipelineStageFlagBits::eComputeShader,
              vk::PipelineStageFlagBits::eTransfer);

            const uint32_t dataTypeMemorySize = this->mRegions[i].tensor->dataTypeMemorySize();
            const vk::BufferCopy copy = {
                dataTypeMemorySize * this->mRegions[i].srcIndex,
                dataTypeMemorySize * this->mRegions[i].dstIndex,
                dataTypeMemorySize * this->mRegions[i].elemCount,
            };
            this->mRegions[i].tensor->recordCopyFromDeviceToStaging(commandBuffer, copy);

            this->mRegions[i].tensor->recordPrimaryBufferMemoryBarrier(
              commandBuffer,
              vk::AccessFlagBits::eTransferWrite,
              vk::AccessFlagBits::eHostRead,
              vk::PipelineStageFlagBits::eTransfer,
              vk::PipelineStageFlagBits::eHost);
        }
    }
}

void
OpTensorSyncRegionLocal::preEval(const vk::CommandBuffer& /*commandBuffer*/)
{
    KP_LOG_DEBUG("Kompute OpTensorSyncRegionLocal preEval called");
}

void
OpTensorSyncRegionLocal::postEval(const vk::CommandBuffer& /*commandBuffer*/)
{
    KP_LOG_DEBUG("Kompute OpTensorSyncRegionLocal postEval called");

    KP_LOG_DEBUG("Kompute OpTensorSyncRegionLocal mapping data into tensor local");
}

}
