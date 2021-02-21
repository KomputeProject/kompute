#pragma once

#include <fstream>

#include "kompute/Core.hpp"

#if RELEASE
#include "kompute/shaders/shaderopmult.hpp"
#endif

#include "kompute/Algorithm.hpp"
#include "kompute/Tensor.hpp"

#include "kompute/operations/OpAlgoBase.hpp"

namespace kp {

/**
 * Operation that performs multiplication on two tensors and outpus on third
 * tensor.
 */
class OpMult : public OpAlgoBase
{
  public:
    /**
     *  Base constructor, should not be used unless explicitly intended.
     */
    OpMult() {

    }

    /**
     * Default constructor with parameters that provides the bare minimum
     * requirements for the operations to be able to create and manage their
     * sub-components.
     *
     * @param physicalDevice Vulkan physical device used to find device queues
     * @param device Vulkan logical device for passing to Algorithm
     * @param commandBuffer Vulkan Command Buffer to record commands into
     * @param tensors Tensors that are to be used in this operation
     * @param komputeWorkgroup Optional parameter to specify the layout for processing
     */
    OpMult(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
           std::shared_ptr<vk::Device> device,
           std::shared_ptr<vk::CommandBuffer> commandBuffer,
           std::vector<std::shared_ptr<Tensor>> tensors,
           const Workgroup& komputeWorkgroup = {})
      : OpAlgoBase(physicalDevice, device, commandBuffer, tensors, "", komputeWorkgroup)
    {
        KP_LOG_DEBUG("Kompute OpMult constructor with params");

#ifndef RELEASE
        this->mShaderFilePath = "shaders/glsl/opmult.comp.spv";
#endif
    }

#if RELEASE
    /**
     * If RELEASE=1 it will be using the static version of the shader which is 
     * loaded using this file directly. Otherwise it should not override the function.
     */
    std::vector<uint32_t> fetchSpirvBinaryData() override
    {
        KP_LOG_WARN(
          "Kompute OpMult Running shaders directly from header");

        return std::vector<uint32_t>(
          (uint32_t*)shader_data::shaders_glsl_opmult_comp_spv,
          (uint32_t*)(shader_data::shaders_glsl_opmult_comp_spv +
            kp::shader_data::shaders_glsl_opmult_comp_spv_len));

    }
#endif

    /**
     * Default destructor, which is in charge of destroying the algorithm
     * components but does not destroy the underlying tensors
     */
    ~OpMult() override {
        KP_LOG_DEBUG("Kompute OpMult destructor started");
    }

};

} // End namespace kp
