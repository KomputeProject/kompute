// SPDX-License-Identifier: Apache-2.0

#include "kompute/operations/OpCopy.hpp"
#include "kompute/Image.hpp"
#include "kompute/Tensor.hpp"

namespace kp {

OpCopy::OpCopy(const std::vector<std::shared_ptr<Memory>>& memObjects)
{
    KP_LOG_DEBUG("Kompute OpCopy constructor with params");

    if (memObjects.size() < 2) {
        throw std::runtime_error(
          "Kompute OpCopy called with less than 2 memory objects");
    }

    this->mMemObjects = memObjects;
}

OpCopy::~OpCopy()
{
    KP_LOG_DEBUG("Kompute OpCopy destructor started");
}

void
OpCopy::record(const vk::CommandBuffer& commandBuffer)
{
    KP_LOG_DEBUG("Kompute OpCopy record called");

    // We iterate from the second memory object onwards and record a copy to all
    for (size_t i = 1; i < this->mMemObjects.size(); i++) {
        this->mMemObjects[i]->recordCopyFrom(commandBuffer,
                                             this->mMemObjects[0]);
    }
}

void
OpCopy::preEval(const vk::CommandBuffer& /*commandBuffer*/)
{
    KP_LOG_DEBUG("Kompute OpCopy preEval called");
}

void
OpCopy::postEval(const vk::CommandBuffer& /*commandBuffer*/)
{
    KP_LOG_DEBUG("Kompute OpCopy postEval called");

    // Do not copy on CPU side if source is storage memory
    if (this->mMemObjects[0]->memoryType() ==
        kp::Memory::MemoryTypes::eStorage) {
        KP_LOG_DEBUG("Kompute OpCopy not copying tensor source given "
                     "it's of eStorage type");
        return;
    }

    // Copy the data from the first memory object into all the memory objects
    for (size_t i = 1; i < this->mMemObjects.size(); i++) {
        if (this->mMemObjects[i]->memoryType() ==
            kp::Memory::MemoryTypes::eStorage) {
            KP_LOG_DEBUG("Kompute OpCopy not copying to tensor dest "
                         "given it's of eStorage type");
            continue;
        }
        this->mMemObjects[i]->setData(this->mMemObjects[0]->rawData(),
                                      this->mMemObjects[0]->memorySize());
    }
}

}
