
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

    std::shared_ptr<kp::Tensor> xI{ new kp::Tensor({ 0, 1, 1, 1, 1 }) };
    std::shared_ptr<kp::Tensor> xJ{ new kp::Tensor({ 0, 0, 0, 1, 1 }) };

    std::shared_ptr<kp::Tensor> y{ new kp::Tensor({ 0, 0, 0, 1, 1 }) };

    std::shared_ptr<kp::Tensor> wIn{ new kp::Tensor({ 0.001, 0.001 }) };
    std::shared_ptr<kp::Tensor> wOutI{ new kp::Tensor({ 0, 0, 0, 0, 0 }) };
    std::shared_ptr<kp::Tensor> wOutJ{ new kp::Tensor({ 0, 0, 0, 0, 0 }) };

    std::shared_ptr<kp::Tensor> bIn{ new kp::Tensor({ 0 }) };
    std::shared_ptr<kp::Tensor> bOut{ new kp::Tensor({ 0, 0, 0, 0, 0 }) };

    std::shared_ptr<kp::Tensor> lOut{ new kp::Tensor({ 0, 0, 0, 0, 0 }) };

    std::vector<std::shared_ptr<kp::Tensor>> params = { xI,  xJ,    y,
                                                        wIn, wOutI, wOutJ,
                                                        bIn, bOut,  lOut };

    kp::Manager mgr;

    std::weak_ptr<kp::Sequence> sqWeakPtr = mgr.getOrCreateManagedSequence("createTensors");
    std::shared_ptr<kp::Sequence> sq = sqWeakPtr.lock();

    sq->begin();

    sq->record<kp::OpTensorCreate>(params);

    sq->end();
    sq->eval();

    // Record op algo base
    sq->begin();

    sq->record<kp::OpTensorSyncDevice>({ wIn, bIn });

    sq->record<kp::OpAlgoBase<>>(
      params, "shaders/glsl/logistic_regression.comp");

    sq->record<kp::OpTensorSyncLocal>({ wOutI, wOutJ, bOut, lOut });

    sq->end();

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

