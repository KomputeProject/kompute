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
class OpAlgoFactory : public OpBase, public std::enable_shared_from_this<OpAlgoFactory>
{
  public:

    enum class AlgoTypes
    {
        eMult = 0,
    };

    OpAlgoFactory(
            const AlgoTypes& algoType,
            const std::shared_ptr<kp::Algorithm>& algorithm,
            const std::vector<std::shared_ptr<kp::Tensor>>& tensors,
            const Workgroup& workgroup = {},
            const Constants& specializationConstants = {},
            const kp::Constants& pushConstants = {});

    /**
     * Default destructor, which is in charge of destroying the algorithm
     * components but does not destroy the underlying tensors
     */
    virtual ~OpAlgoFactory() override;

    /**
     * This records the commands that are to be sent to the GPU. This includes
     * the barriers that ensure the memory has been copied before going in and
     * out of the shader, as well as the dispatch operation that sends the
     * shader processing to the gpu. This function also records the GPU memory
     * copy of the output data for the staging buffer so it can be read by the
     * host.
     */
    virtual void record(const vk::CommandBuffer& commandBuffer) override;

    /**
     * Does not perform any preEval commands.
     */
    virtual void preEval(const vk::CommandBuffer& commandBuffer) override;

    /**
     * Executes after the recorded commands are submitted, and performs a copy
     * of the GPU Device memory into the staging buffer so the output data can
     * be retrieved.
     */
    virtual void postEval(const vk::CommandBuffer& commandBuffer) override;

    std::shared_ptr<kp::OpAlgoFactory> push(const Constants& pushConstants);

private:

    void rebuildAlgorithmFromFactory();

    AlgoTypes mAlgoType;
    std::vector<std::shared_ptr<kp::Tensor>> mTensors;
    std::shared_ptr<Algorithm> mAlgorithm;
    Constants mSpecializationConstants;
    Workgroup mWorkgroup;
    Constants mPushConstants;
};

} // End namespace kp

