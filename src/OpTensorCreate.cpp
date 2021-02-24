
#include "kompute/operations/OpTensorCreate.hpp"

namespace kp {

OpTensorCreate::OpTensorCreate(
    std::vector<std::shared_ptr<Tensor>>& tensors)
    : OpBase(tensors, nullptr)
{
    KP_LOG_DEBUG("Compute OpTensorCreate constructor with params");
    this->mManagesTensors = true;
}

OpTensorCreate::~OpTensorCreate()
{
    KP_LOG_DEBUG("Kompute OpTensorCreate destructor started");
}

void
OpTensorCreate::init(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
            std::shared_ptr<vk::Device> device)
{
    KP_LOG_DEBUG("Kompute OpTensorCreate init called");

    OpBase::init(physicalDevice, device);
}

void
OpTensorCreate::record(std::shared_ptr<vk::CommandBuffer> commandBuffer)
{
    KP_LOG_DEBUG("Kompute OpTensorCreate record called");
}

void
OpTensorCreate::preEval()
{
    KP_LOG_DEBUG("Kompute OpTensorCreate preEval called");
}

void
OpTensorCreate::postEval()
{
    KP_LOG_DEBUG("Kompute OpTensorCreate postEval called");
}

}
