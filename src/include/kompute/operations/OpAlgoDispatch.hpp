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

    OpAlgoDispatch(const std::shared_ptr<kp::Algorithm>& algorithm, bool skipAlgoCheck = false);

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
     */
    virtual void record(std::shared_ptr<vk::CommandBuffer> commandBuffer) override;

    /**
     * Does not perform any preEval commands.
     */
    virtual void preEval() override;

    /**
     * Executes after the recorded commands are submitted, and performs a copy
     * of the GPU Device memory into the staging buffer so the output data can
     * be retrieved.
     */
    virtual void postEval() override;

private:
    // -------------- ALWAYS OWNED RESOURCES
    std::shared_ptr<Algorithm> mAlgorithm;
};

} // End namespace kp

