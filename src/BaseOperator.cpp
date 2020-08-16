
#include <spdlog/spdlog.h>

#include "BaseOperator.hpp"

namespace kp {

BaseOperator::BaseOperator() {

}

BaseOperator::BaseOperator(vk::CommandBuffer* commandBuffer) {
    SPDLOG_DEBUG("Compute BaseOperator constructor started");
    this->mCommandBuffer = commandBuffer;
}

BaseOperator::~BaseOperator() {
    SPDLOG_DEBUG("Compute BaseOperator destructor started");

}

void BaseOperator::init(std::string one, std::string two) {
    SPDLOG_DEBUG("Compute BaseOperator init started");

}

void BaseOperator::record() {
    SPDLOG_DEBUG("Compute BaseOperator record started");

}

}

