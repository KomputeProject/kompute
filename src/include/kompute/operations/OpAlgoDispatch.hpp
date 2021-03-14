#pragma once

#include "kompute/Core.hpp"
#include "kompute/Algorithm.hpp"
#include "kompute/Tensor.hpp"
#include "kompute/operations/OpBase.hpp"

namespace kp {

/**
 * Operation that provides a general abstraction that simplifies the use of 
 * algorithm and parameter components which can be used with shaders.
 * By default it enables the user to provide a dynamic number of tensors
 * which are then passed as inputs.
 */
class OpAlgoDispatch : public OpBase
{
  public:

    /**
     * Constructor that stores the algorithm to use as well as the relevant
     * push constants to override when recording.
     *
     * @param algorithm The algorithm object to use for dispatch
     * @param pushConstants The push constants to use for override
     */
    OpAlgoDispatch(const std::shared_ptr<kp::Algorithm>& algorithm,
            const kp::Constants& pushConstants = {});

    /**
     * Default destructor, which is in charge of destroying the algorithm
     * components but does not destroy the underlying tensors
     */
    virtual ~OpAlgoDispatch() override;

    /**
     * This records the commands that are to be sent to the GPU. This includes
     * the barriers that ensure the memory has been copied before going in and
     * out of the shader, as well as the dispatch operation that sends the
     * shader processing to the gpu. This function also records the GPU memory
     * copy of the output data for the staging buffer so it can be read by the
     * host.
     *
     * @param commandBuffer The command buffer to record the command into.
     */
    virtual void record(const vk::CommandBuffer& commandBuffer) override;

    /**
     * Does not perform any preEval commands.
     *
     * @param commandBuffer The command buffer to record the command into.
     */
    virtual void preEval(const vk::CommandBuffer& commandBuffer) override;

    /**
     * Does not perform any postEval commands.
     *
     * @param commandBuffer The command buffer to record the command into.
     */
    virtual void postEval(const vk::CommandBuffer& commandBuffer) override;

private:
    // -------------- ALWAYS OWNED RESOURCES
    std::shared_ptr<Algorithm> mAlgorithm;
    Constants mPushConstants;
};

} // End namespace kp

