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
     *  Default constructor with parameters that provides the bare minimum
     * requirements for the operations to be able to create and manage their
     * sub-components.
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
    }

    /**
     * Default destructor for OpBase class. This OpBase destructor class should
     * always be called to destroy and free owned resources unless it is
     * intended to destroy the resources in the parent class. This can be done
     * by passing the mFreeTensors=false.
     */
    ~OpBase()
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

    virtual void init() = 0;

    virtual void record() = 0;

    virtual void postSubmit() = 0;

  protected:
    // Sometimes owned resources
    std::vector<std::shared_ptr<Tensor>> mTensors;
    bool mFreeTensors =
      false; // TODO: Provide granularity to specify which to free

    // Always external resources
    std::shared_ptr<vk::PhysicalDevice> mPhysicalDevice;
    std::shared_ptr<vk::Device> mDevice;
    std::shared_ptr<vk::CommandBuffer> mCommandBuffer;
};

} // End namespace kp
