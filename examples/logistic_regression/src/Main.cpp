
#include <iostream>
#include <memory>
#include <vector>

#include "kompute/Kompute.hpp"

int main()
{
#if KOMPUTE_ENABLE_SPDLOG
    spdlog::set_level(
      static_cast<spdlog::level::level_enum>(SPDLOG_ACTIVE_LEVEL));
#endif

    uint32_t ITERATIONS = 100;
    float learningRate = 0.1;

    kp::Manager mgr;

    auto xI = mgr.tensor({ 0, 1, 1, 1, 1 });
    auto xJ = mgr.tensor({ 0, 0, 0, 1, 1 });

    auto y = mgr.tensor({ 0, 0, 0, 1, 1 });

    auto wIn = mgr.tensor({ 0.001, 0.001 });
    auto wOutI = mgr.tensor({ 0, 0, 0, 0, 0 });
    auto wOutJ = mgr.tensor({ 0, 0, 0, 0, 0 });

    auto bIn = mgr.tensor({ 0 });
    auto bOut = mgr.tensor({ 0, 0, 0, 0, 0 });

    auto lOut = mgr.tensor({ 0, 0, 0, 0, 0 });

    std::vector<std::shared_ptr<kp::Tensor>> params = { xI,  xJ,    y,
                                                        wIn, wOutI, wOutJ,
                                                        bIn, bOut,  lOut };

    std::vector<uint32_t> spirv(
                (uint32_t*)kp::shader_data::shaders_glsl_logisticregression_comp_spv,
                (uint32_t*)(kp::shader_data::shaders_glsl_logisticregression_comp_spv
                    + kp::shader_data::shaders_glsl_logisticregression_comp_spv_len));

    std::shared_ptr<kp::Algorithm> algo = mgr.algorithm(
            params, spirv, kp::Workgroup({ 5 }), kp::Constants({ 5.0 }));

    mgr.sequence()->eval<kp::OpTensorSyncDevice>(params);

    std::shared_ptr<kp::Sequence> sq = mgr.sequence()
        ->record<kp::OpTensorSyncDevice>({ wIn, bIn })
        ->record<kp::OpAlgoDispatch>(algo)
        ->record<kp::OpTensorSyncLocal>({ wOutI, wOutJ, bOut, lOut });

    // Iterate across all expected iterations
    for (size_t i = 0; i < ITERATIONS; i++) {

        sq->eval();

        for (size_t j = 0; j < bOut->size(); j++) {
            wIn->data()[0] -= learningRate * wOutI->data()[j];
            wIn->data()[1] -= learningRate * wOutJ->data()[j];
            bIn->data()[0] -= learningRate * bOut->data()[j];
        }
    }

    std::cout << "RESULTS" << std::endl;
    std::cout << "w1: " << wIn->data()[0] << std::endl;
    std::cout << "w2: " << wIn->data()[1] << std::endl;
    std::cout << "b: " << bIn->data()[0] << std::endl;
}

