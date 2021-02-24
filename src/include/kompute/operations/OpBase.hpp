#pragma once

#include "kompute/Core.hpp"

#include "kompute/Tensor.hpp"
#include "kompute/Algorithm.hpp"

namespace kp {

/**
 *  Base Operation which provides the high level interface that Kompute
 *  operations implement in order to perform a set of actions in the GPU.
 *
 *  Operations can perform actions on tensors, and optionally can also own an
 *  Algorithm with respective parameters. kp::Operations with kp::Algorithms
 *  would inherit from kp::OpBaseAlgo.
 */
class OpBase
{
  public:

    /**
     * Default constructor with parameters that provides the bare minimum
     * requirements for the operations to be able to create and manage their
     * sub-components.
     *
     * @param physicalDevice Vulkan physical device used to find device queues
     * @param device Vulkan logical device for passing to Algorithm
     * @param commandBuffer Vulkan Command Buffer to record commands into
     * @param tensors Tensors that are to be used in this operation
     */
    OpBase(std::vector<std::shared_ptr<Tensor>>& tensors,
            std::shared_ptr<Algorithm> algorithm)
    {
        KP_LOG_DEBUG("Compute OpBase constructor with params");
        this->mTensors = tensors;
        this->mAlgorithm = algorithm;
        this->mIsInit = false;
    }

    /**
     * Default destructor for OpBase class. This OpBase destructor class should
     * always be called to destroy and free owned resources unless it is
     * intended to destroy the resources in the parent class.
     */
    virtual ~OpBase()
    {
        KP_LOG_DEBUG("Kompute OpBase destructor started");
        this->destroy();
    }

    virtual std::shared_ptr<kp::Algorithm> algorithm() {
        return this->mAlgorithm;
    }

    virtual std::vector<std::shared_ptr<kp::Tensor>> tensors() {
        return this->mTensors;
    }

    virtual bool isInit() {
        return this->mIsInit;
    }

    /**
     * The init function is responsible for setting up all the resources and
     * should be called after the Operation has been created.
     */
    // TODO: Potentially remove physicalDevice in favour of memoryProperties (for tensor)
    virtual void init(
            std::shared_ptr<vk::PhysicalDevice> physicalDevice,
            std::shared_ptr<vk::Device> device) {

        if (this->mTensors.size() < 1) {
            throw std::runtime_error("Kompute OpBase init called with 0 tensors");
        }

        if (this->mManagesTensors) {
            for (std::shared_ptr<Tensor> tensor : this->mTensors) {
                if (tensor->isInit()) {
                    // TODO: Evaluate whether throwing runtime error or just writing error log
                    throw std::runtime_error(
                      "Kompute OpTensorCreate: Tensor has already been initialized");
                }
                else {
                    tensor->init(physicalDevice, device);
                }
            }
        }

        if (this->mManagesAlgorithm) {
            this->mAlgorithm->init(device, this->mTensors);
        }
    }

    virtual void destroy() {
        if (!this->mIsInit) {
            KP_LOG_WARN("Kompute OpBase destroy called but not initialised");
        }

        if (this->mManagesTensors) {
            for (const std::shared_ptr<Tensor>& tensor : this->mTensors) {
                if (!tensor->isInit()) {
                    KP_LOG_WARN("Kompute OpBase attempted to free managed tensor "
                                 "but tensor is not initialised");
                } else {
                    KP_LOG_DEBUG("Kompute OpBase freeing tensor");
                    tensor->freeMemoryDestroyGPUResources();
                }
            }
            this->mTensors.clear();
        }

        if (this->mManagesAlgorithm) {
            if (this->mAlgorithm && this->mAlgorithm->isInit()) {
                KP_LOG_DEBUG("Kompute OpBase freeing tensor");
                this->mAlgorithm->freeMemoryDestroyGPUResources();
            } else {
                KP_LOG_WARN("Kompute OpBase attempted to free managed algorithm"
                             "but algorithm is not initialised");
            }
        }

        this->mIsInit = false;
    }

    /**
     * The record function is intended to only send a record command or run
     * commands that are expected to record operations that are to be submitted
     * as a batch into the GPU.
     */
    virtual void record(std::shared_ptr<vk::CommandBuffer> commandBuffer) = 0;

    /**
     * Pre eval is called before the Sequence has called eval and submitted the commands to
     * the GPU for processing, and can be used to perform any per-eval setup steps
     * required as the computation iteration begins. It's worth noting that 
     * there are situations where eval can be called multiple times, so the 
     * resources that are created should be idempotent in case it's called multiple
     * times in a row.
     */
    virtual void preEval() = 0;

    /**
     * Post eval is called after the Sequence has called eval and submitted the commands to
     * the GPU for processing, and can be used to perform any tear-down steps
     * required as the computation iteration finishes. It's worth noting that 
     * there are situations where eval can be called multiple times, so the 
     * resources that are destroyed should not require a re-init unless explicitly
     * provided by the user.
     */
    virtual void postEval() = 0;

  protected:
    // -------------- OPTIONALLY OWNED RESOURCES
    std::vector<std::shared_ptr<Tensor>> mTensors; 
    bool mManagesTensors = false;
    std::shared_ptr<kp::Algorithm> mAlgorithm;
    bool mManagesAlgorithm = false;

    // -------------- ALWAYS OWNED RESOURCES
    bool mIsInit;
};

} // End namespace kp
