// SPDX-License-Identifier: Apache-2.0

#include "kompute/operations/OpSyncDevice.hpp"

namespace kp {

OpSyncDevice::OpSyncDevice(
  const std::vector<std::shared_ptr<Memory>>& memObjects)
{
    KP_LOG_DEBUG("Kompute OpSyncDevice constructor with params");

    if (memObjects.size() < 1) {
        throw std::runtime_error(
          "Kompute OpSyncDevice called with less than 1 memory object");
    }

    // Store both shared_ptr for ownership and raw pointers for performance
    this->mMemObjectsShared = memObjects;
    this->mMemObjects.reserve(memObjects.size());
    for (const auto& memObj : memObjects) {
        this->mMemObjects.push_back(memObj.get());
    }
}

OpSyncDevice::~OpSyncDevice() noexcept
{
    KP_LOG_DEBUG("Kompute OpSyncDevice destructor started");

    // Clear both vectors - no explicit cleanup needed as we don't own the Memory objects
    this->mMemObjects.clear();
    this->mMemObjectsShared.clear();
}

void
OpSyncDevice::record(const vk::CommandBuffer& commandBuffer)
{
    KP_LOG_DEBUG("Kompute OpSyncDevice record called");

    // Using raw pointers for performance optimization
    for (size_t i = 0; i < this->mMemObjects.size(); i++) {
        if (this->mMemObjects[i]->memoryType() ==
            Tensor::MemoryTypes::eDevice) {
            this->mMemObjects[i]->recordCopyFromStagingToDevice(commandBuffer);
        }
    }
}

void
OpSyncDevice::preEval(const vk::CommandBuffer& /*commandBuffer*/)
{
    KP_LOG_DEBUG("Kompute OpSyncDevice preEval called");
}

void
OpSyncDevice::postEval(const vk::CommandBuffer& /*commandBuffer*/)
{
    KP_LOG_DEBUG("Kompute OpSyncDevice postEval called");
}

}
