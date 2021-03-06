#pragma once

#include <fstream>

#include "kompute/Core.hpp"

#include "kompute/shaders/shaderopmult.hpp"

#include "kompute/Algorithm.hpp"
#include "kompute/Tensor.hpp"

#include "kompute/operations/OpAlgoDispatch.hpp"

namespace kp {

/**
 * Operation that performs multiplication on two tensors and outpus on third
 * tensor.
 */
class OpMult : public OpAlgoDispatch
{
  public:

    /**
     * Default constructor with parameters that provides the bare minimum
     * requirements for the operations to be able to create and manage their
     * sub-components.
     *
     * @param tensors Tensors that are to be used in this operation
     * @param algorithm An algorithm that will be overridden with the OpMult
     * shader data and the tensors provided which are expected to be 3
     */
    OpMult(std::vector<std::shared_ptr<Tensor>> tensors, std::shared_ptr<Algorithm> algorithm)
        : OpAlgoDispatch(algorithm)
    {
        KP_LOG_DEBUG("Kompute OpMult constructor with params");

        if (tensors.size() != 3) {
            throw std::runtime_error("Kompute OpMult expected 3 tensors but got " + tensors.size());
        }

        std::vector<uint32_t> spirv(
          (uint32_t*)shader_data::shaders_glsl_opmult_comp_spv,
          (uint32_t*)(shader_data::shaders_glsl_opmult_comp_spv +
            kp::shader_data::shaders_glsl_opmult_comp_spv_len));

        algorithm->rebuild(tensors, spirv);
    }

    /**
     * Default destructor, which is in charge of destroying the algorithm
     * components but does not destroy the underlying tensors
     */
    virtual ~OpMult() override {
        KP_LOG_DEBUG("Kompute OpMult destructor started");
    }
};

} // End namespace kp
