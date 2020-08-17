
#include "OpCreateTensor.hpp"

namespace kp {

OpCreateTensor::OpCreateTensor() {

}

OpCreateTensor::OpCreateTensor(std::shared_ptr<vk::CommandBuffer> commandBuffer) {
    this->mCommandBuffer = commandBuffer;
}

OpCreateTensor::~OpCreateTensor() {

}

}
