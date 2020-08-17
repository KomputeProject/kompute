
// SPDLOG_ACTIVE_LEVEL must be defined before spdlog.h import
#if DEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#endif

#include <spdlog/spdlog.h>

#include "BaseOp.hpp"

namespace kp {

BaseOp::BaseOp() {}

BaseOp::BaseOp(std::shared_ptr<vk::CommandBuffer> commandBuffer)
{
    SPDLOG_DEBUG("Compute BaseOp constructor started");
    this->mCommandBuffer = commandBuffer;
}

BaseOp::~BaseOp()
{
    SPDLOG_DEBUG("Compute BaseOp destructor started");
}

void
BaseOp::init(std::string one, std::string two)
{
    SPDLOG_DEBUG("Compute BaseOp init started");
}

void
BaseOp::record()
{
    SPDLOG_DEBUG("Compute BaseOp record started");
}

}
