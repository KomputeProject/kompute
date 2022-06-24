
#include <iostream>
#include <memory>
#include <vector>

#include "kompute/Tensor.hpp"
#include "my_shader.hpp"
#include <kompute/Kompute.hpp>

int
main()
{
    uint32_t ITERATIONS = 100;
    float learningRate = 0.1;

    kp::Manager mgr;

    std::shared_ptr<kp::TensorT<float>> xI = mgr.tensor({ 0, 1, 1, 1, 1 });
    std::shared_ptr<kp::TensorT<float>> xJ = mgr.tensor({ 0, 0, 0, 1, 1 });

    std::shared_ptr<kp::TensorT<float>> y = mgr.tensor({ 0, 0, 0, 1, 1 });

    std::shared_ptr<kp::TensorT<float>> wIn = mgr.tensor({ 0.001, 0.001 });
    std::shared_ptr<kp::TensorT<float>> wOutI = mgr.tensor({ 0, 0, 0, 0, 0 });
    std::shared_ptr<kp::TensorT<float>> wOutJ = mgr.tensor({ 0, 0, 0, 0, 0 });

    std::shared_ptr<kp::TensorT<float>> bIn = mgr.tensor({ 0 });
    std::shared_ptr<kp::TensorT<float>> bOut = mgr.tensor({ 0, 0, 0, 0, 0 });

    std::shared_ptr<kp::TensorT<float>> lOut = mgr.tensor({ 0, 0, 0, 0, 0 });

    const std::vector<std::shared_ptr<kp::Tensor>> params = {
        xI, xJ, y, wIn, wOutI, wOutJ, bIn, bOut, lOut
    };

    const std::vector<uint32_t> shader = std::vector<uint32_t>(
      shader::MY_SHADER_COMP_SPV.begin(), shader::MY_SHADER_COMP_SPV.end());

    std::shared_ptr<kp::Algorithm> algo = mgr.algorithm(
      params, shader, kp::Workgroup({ 5 }), std::vector<float>({ 5.0 }));

    mgr.sequence()->eval<kp::OpTensorSyncDevice>(params);

    std::shared_ptr<kp::Sequence> sq =
      mgr.sequence()
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