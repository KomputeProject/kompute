

#include "Tensor.hpp"

#include "OpMult.hpp"

namespace kp {

OpMult::OpMult() {
    SPDLOG_DEBUG("Kompute OpMult constructor base");

}

OpMult::OpMult(std::shared_ptr<vk::PhysicalDevice> physicalDevice, 
                               std::shared_ptr<vk::Device> device,
                               std::shared_ptr<vk::CommandBuffer> commandBuffer)
  : OpBase(physicalDevice, device, commandBuffer)
{
    SPDLOG_DEBUG("Kompute OpMult constructor with params");
}

OpMult::~OpMult() {
    SPDLOG_DEBUG("Kompute OpMult destructor started");
}

void
OpMult::init(std::vector<std::shared_ptr<Tensor>> tensors)
{
    SPDLOG_DEBUG("Kompute OpMult init called");

    if (tensors.size() < 2) {
        throw std::runtime_error("Kompute OpMult called with less than 1 tensor");
    } else if (tensors.size() > 2) {
        spdlog::warn("Kompute OpMult called with more than 2 tensor");
    }

}

void
OpMult::record()
{
    SPDLOG_DEBUG("Kompute OpMult record called");

}

}
