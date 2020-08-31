
#include "catch2/catch.hpp"

#include "kompute/Kompute.hpp"

#include <fmt/ranges.h>

TEST_CASE("test_logistic_regression") {

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
                mgr.getOrCreateManagedSequence("testSq").lock()) {
            sq->begin();

            sq->record<kp::OpCreateTensor>(params);

            sq->record<kp::OpAlgoBase<>>(
                    params, 
                    true, // Whether to copy output from device
                    "test/shaders/glsl/test_logistic_regression.comp");

            sq->end();

            // Iterate across all expected iterations
            for (size_t i = 0; i < ITERATIONS; i++) {
                sq->eval();

                // TODO: Reference of data instead of full value copy every time
                for(size_t j = 0; j < bOut->size(); j++) {
                    wInVec[0] -= wOutI->data()[j];
                    wInVec[1] -= wOutJ->data()[j];
                    bInVec[0] -= bOut->data()[j];
                }
                wIn->setData(wInVec);
                bIn->setData(bInVec);

                wIn->mapDataIntoHostMemory();
                bIn->mapDataIntoHostMemory();
            }
        }

        wIn->mapDataFromHostMemory();
        bIn->mapDataFromHostMemory();
    }


    // Based on the inputs the outputs should be at least:
    // * wi < 0.01
    // * wj > 1.0
    // * b < 0
    REQUIRE(wIn->data()[0] < 0.01);
    REQUIRE(wIn->data()[1] > 1.0);
    REQUIRE(bIn->data()[0] < 0.0);
}
