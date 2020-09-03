
#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"

TEST(LogisticRegressionAlgorithm, TestMainLogisticRegression) {

    uint32_t ITERATIONS = 100;

    std::vector<float> wInVec = { 0.001, 0.001 };
    std::vector<float> bInVec = { 0 };

    std::shared_ptr<kp::Tensor> xI{ new kp::Tensor({ 0, 1, 1, 1, 1 })};
    std::shared_ptr<kp::Tensor> xJ{ new kp::Tensor({ 0, 0, 0, 1, 1 })};

    std::shared_ptr<kp::Tensor> y{ new kp::Tensor({ 0, 0, 0, 1, 1 })};

    std::shared_ptr<kp::Tensor> wIn{ 
        new kp::Tensor(wInVec, kp::Tensor::TensorTypes::eStaging)};
    std::shared_ptr<kp::Tensor> wOutI{ new kp::Tensor({ 0, 0, 0, 0, 0 })};
    std::shared_ptr<kp::Tensor> wOutJ{ new kp::Tensor({ 0, 0, 0, 0, 0 })};

    std::shared_ptr<kp::Tensor> bIn{ 
        new kp::Tensor(bInVec, kp::Tensor::TensorTypes::eStaging)};
    std::shared_ptr<kp::Tensor> bOut{ new kp::Tensor({ 0, 0, 0, 0, 0 })};

    std::vector<std::shared_ptr<kp::Tensor>> params = 
        {xI, xJ, y, wIn, wOutI, wOutJ, bIn, bOut};

    {
        kp::Manager mgr;

        if (std::shared_ptr<kp::Sequence> sq = 
                mgr.getOrCreateManagedSequence("createTensors").lock()) {

            sq->begin();

            sq->record<kp::OpCreateTensor>(params);

            sq->end();
            sq->eval();

            // Record op algo base
            sq->begin();

            sq->record<kp::OpAlgoBase<>>(
                    params, 
                    true, // Whether to copy output from device
                    "test/shaders/glsl/test_logistic_regression.comp");

            sq->end();

            // Iterate across all expected iterations
            for (size_t i = 0; i < ITERATIONS; i++) {

                sq->eval();

                for(size_t j = 0; j < bOut->size(); j++) {
                    wIn->data()[0] -= wOutI->data()[j];
                    wIn->data()[1] -= wOutJ->data()[j];
                    bIn->data()[0] -= bOut->data()[j];
                }
                wIn->mapDataIntoHostMemory();
                bIn->mapDataIntoHostMemory();
            }
        }
    }


    // Based on the inputs the outputs should be at least:
    // * wi < 0.01
    // * wj > 1.0
    // * b < 0
    EXPECT_LT(wIn->data()[0], 0.01);
    EXPECT_GT(wIn->data()[1], 1.0);
    EXPECT_LT(bIn->data()[0], 0.0);

    //SPDLOG_DEBUG("Result wIn: {}, bIn: {}", 
    //        wIn->data(), bIn->data());
}
