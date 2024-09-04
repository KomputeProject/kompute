// SPDX-License-Identifier: Apache-2.0

#include "kompute/Tensor.hpp"

#include "kompute/operations/OpSyncLocal.hpp"

namespace kp {

OpSyncLocal::OpSyncLocal(const std::vector<std::shared_ptr<Memory>>& memObjects)
{
    KP_LOG_DEBUG("Kompute OpSyncLocal constructor with params");

    if (memObjects.size() < 1) {
        throw std::runtime_error(
          "Kompute OpSyncLocal called with less than 1 memory object");
    }

    this->mMemObjects = memObjects;
}

OpSyncLocal::~OpSyncLocal()
{
    KP_LOG_DEBUG("Kompute OpSyncLocal destructor started");
}

void
OpSyncLocal::record(const vk::CommandBuffer& commandBuffer)
{
    KP_LOG_DEBUG("Kompute OpSyncLocal record called");

    for (size_t i = 0; i < this->mMemObjects.size(); i++) {
        if (this->mMemObjects[i]->memoryType() ==
            Memory::MemoryTypes::eDevice) {

            this->mMemObjects[i]->recordPrimaryMemoryBarrier(
              commandBuffer,
              vk::AccessFlagBits::eShaderWrite,
              vk::AccessFlagBits::eTransferRead,
              vk::PipelineStageFlagBits::eComputeShader,
              vk::PipelineStageFlagBits::eTransfer);

            this->mMemObjects[i]->recordCopyFromDeviceToStaging(commandBuffer);

            this->mMemObjects[i]->recordPrimaryMemoryBarrier(
              commandBuffer,
              vk::AccessFlagBits::eTransferWrite,
              vk::AccessFlagBits::eHostRead,
              vk::PipelineStageFlagBits::eTransfer,
              vk::PipelineStageFlagBits::eHost);
        }
    }
}

void
OpSyncLocal::preEval(const vk::CommandBuffer& /*commandBuffer*/)
{
    KP_LOG_DEBUG("Kompute OpSyncLocal preEval called");
}

void
OpSyncLocal::postEval(const vk::CommandBuffer& /*commandBuffer*/)
{
    KP_LOG_DEBUG("Kompute OpSyncLocal postEval called");

    KP_LOG_DEBUG("Kompute OpSyncLocal mapping data into tensor local");
}

}
