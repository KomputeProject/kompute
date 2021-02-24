#pragma once

#include "kompute/operations/OpAlgoCreate.hpp"

namespace kp {

OpAlgoCreate::OpAlgoCreate(std::vector<std::shared_ptr<Tensor>> tensors,
            std::shared_ptr<Algorithm> algorithm)
  : OpBase(tensors, algorithm)
{
    KP_LOG_DEBUG("Kompute OpAlgoCreate constructor");

    this->mManagesAlgorithm = true;
    this->mManagesTensors = false;
}

OpAlgoCreate::~OpAlgoCreate()
{
    KP_LOG_DEBUG("Kompute OpAlgoCreate destructor started");
}

void
OpAlgoCreate::init(
        std::shared_ptr<vk::PhysicalDevice> physicalDevice,
        std::shared_ptr<vk::Device> device) {

    KP_LOG_DEBUG("Kompute OpAlgoCreate init started");

    // Explicitly calling top level function to create algo
    OpBase::init(physicalDevice, device);
}

void
OpAlgoCreate::record(std::shared_ptr<vk::CommandBuffer> commandBuffer)
{
    KP_LOG_DEBUG("Kompute OpAlgoCreate record called");
}

void
OpAlgoCreate::preEval()
{
    KP_LOG_DEBUG("Kompute OpAlgoCreate preEval called");
}

void
OpAlgoCreate::postEval()
{
    KP_LOG_DEBUG("Kompute OpAlgoCreate postSubmit called");
}

}
