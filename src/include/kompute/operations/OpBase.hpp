#pragma once

#include "kompute/Core.hpp"

#include "kompute/Tensor.hpp"

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
     *  Base constructor, should not be used unless explicitly intended.
     */
    OpBase() { SPDLOG_DEBUG("Compute OpBase base constructor"); }

    /**
     * Default constructor with parameters that provides the bare minimum
     * requirements for the operations to be able to create and manage their
     * sub-components.
     *
     * @param physicalDevice Vulkan physical device used to find device queues
     * @param device Vulkan logical device for passing to Algorithm
     * @param commandBuffer Vulkan Command Buffer to record commands into
     * @param tensors Tensors that are to be used in this operation
     * @param freeTensors Whether operation manages the memory of the Tensors
     */
    OpBase(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
           std::shared_ptr<vk::Device> device,
           std::shared_ptr<vk::CommandBuffer> commandBuffer,
           std::vector<std::shared_ptr<Tensor>>& tensors,
           bool freeTensors)
    {
        SPDLOG_DEBUG("Compute OpBase constructor with params");

        this->mPhysicalDevice = physicalDevice;
        this->mDevice = device;
        this->mCommandBuffer = commandBuffer;
        this->mTensors = tensors;
        this->mFreeTensors = freeTensors;
    }

    /**
     * Default destructor for OpBase class. This OpBase destructor class should
     * always be called to destroy and free owned resources unless it is
     * intended to destroy the resources in the parent class. This can be done
     * by passing the mFreeTensors=false.
     */
    virtual ~OpBase()
    {
        SPDLOG_DEBUG("Kompute OpBase destructor started");

        if (!this->mDevice) {
            spdlog::warn("Kompute OpBase destructor called with empty device");
            return;
        }

        if (this->mFreeTensors) {
            SPDLOG_DEBUG("Kompute OpBase freeing tensors");
            for (std::shared_ptr<Tensor> tensor : this->mTensors) {
                if (tensor && tensor->isInit()) {
                    tensor->freeMemoryDestroyGPUResources();
                } else {
                    spdlog::error("Kompute OpBase expected to free "
                                  "tensor but has already been freed.");
                }
            }
        }
    }

    /**
     * The init function is responsible for setting up all the resources and
     * should be called after the Operation has been created.
     */
    virtual void init() = 0;

    /**
     * The record function is intended to only send a record command or run
     * commands that are expected to record operations that are to be submitted
     * as a batch into the GPU.
     */
    virtual void record() = 0;

    /**
     * Post submit is called after the Sequence has submitted the commands to
     * the GPU for processing, and can be used to perform any tear-down steps
     * required as the computation iteration finishes.
     */
    virtual void postSubmit() = 0;

  protected:
    // -------------- NEVER OWNED RESOURCES
    std::shared_ptr<vk::PhysicalDevice>
      mPhysicalDevice;                   ///< Vulkan Physical Device
    std::shared_ptr<vk::Device> mDevice; ///< Vulkan Logical Device
    std::shared_ptr<vk::CommandBuffer>
      mCommandBuffer; ///< Vulkan Command Buffer

    // -------------- OPTIONALLY OWNED RESOURCES
    std::vector<std::shared_ptr<Tensor>>
      mTensors; ///< Tensors referenced by operation that can be managed
                ///< optionally by operation
    bool mFreeTensors = false; ///< Explicit boolean that specifies whether the
                               ///< tensors are freed (if they are managed)
};

} // End namespace kp
