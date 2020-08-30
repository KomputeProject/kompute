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
 * tensor. The template parameters specify the processing GPU layout number of
 * iterations for each x, y, z parameter. More specifically, this will be the
 * input to ".dispatch(uint32_t tX, uint32_t tY, uint32_t, tZ)"
 */
template<uint32_t tX = 0, uint32_t tY = 0, uint32_t tZ = 0>
class OpMult : public OpAlgoBase<tX, tY, tZ>
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
     * @param freeTensors Whether operation manages the memory of the Tensors
     */
    OpMult(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
           std::shared_ptr<vk::Device> device,
           std::shared_ptr<vk::CommandBuffer> commandBuffer,
           std::vector<std::shared_ptr<Tensor>> tensors)
      : OpAlgoBase<tX, tY, tZ>(physicalDevice, device, commandBuffer, tensors, true)
    {
        SPDLOG_DEBUG("Kompute OpMult constructor with params");

#ifndef RELEASE
        this->mOptSpirvBinPath = "shaders/glsl/opmult.comp.spv";
#endif
    }

#if RELEASE
    /**
     * If release it will be using the static version of the shader which is 
     * loaded using this file directly.
     *
     * @param physicalDevice Vulkan physical device used to find device queues
     * @param device Vulkan logical device for passing to Algorithm
     * @param commandBuffer Vulkan Command Buffer to record commands into
     * @param tensors Tensors that are to be used in this operation
     * @param freeTensors Whether operation manages the memory of the Tensors
     */
    std::vector<char> fetchSpirvBinaryData() override
    {
        SPDLOG_WARN(
          "Kompute OpMult Running shaders directly from header");

        return std::vector<char>(
          shader_data::shaders_glsl_opmult_comp_spv,
          shader_data::shaders_glsl_opmult_comp_spv +
            kp::shader_data::shaders_glsl_opmult_comp_spv_len);

    }
#endif

    /**
     * Default destructor, which is in charge of destroying the algorithm
     * components but does not destroy the underlying tensors
     */
    ~OpMult() override {
        SPDLOG_DEBUG("Kompute OpMult destructor started");
    }

};

} // End namespace kp
