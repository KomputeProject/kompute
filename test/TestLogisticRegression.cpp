
#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"

#include "kompute_test/shaders/shadertest_logistic_regression.hpp"

#include "TestUtils.cpp"

TEST(TestLogisticRegression, TestMainLogisticRegression)
{

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

    std::string shaderStr = R"(
#version 450

layout (constant_id = 0) const float m = 0;

layout (local_size_x = 1) in;

layout(set = 0, binding = 0) buffer bxi { float xi[]; };
layout(set = 0, binding = 1) buffer bxj { float xj[]; };
layout(set = 0, binding = 2) buffer by { float y[]; };
layout(set = 0, binding = 3) buffer bwin { float win[]; };
layout(set = 0, binding = 4) buffer bwouti { float wouti[]; };
layout(set = 0, binding = 5) buffer bwoutj { float woutj[]; };
layout(set = 0, binding = 6) buffer bbin { float bin[]; };
layout(set = 0, binding = 7) buffer bbout { float bout[]; };
layout(set = 0, binding = 8) buffer blout { float lout[]; };

float sigmoid(float z) {
    return 1.0 / (1.0 + exp(-z));
}

float inference(vec2 x, vec2 w, float b) {
    // Compute the linear mapping function
    float z = dot(w, x) + b;
    // Calculate the y-hat with sigmoid 
    float yHat = sigmoid(z);
    return yHat;
}

float calculateLoss(float yHat, float y) {
    return -(y * log(yHat)  +  (1.0 - y) * log(1.0 - yHat));
}

void main() {
    uint idx = gl_GlobalInvocationID.x;

    vec2 wCurr = vec2(win[0], win[1]);
    float bCurr = bin[0];

    vec2 xCurr = vec2(xi[idx], xj[idx]);
    float yCurr = y[idx];

    float yHat = inference(xCurr, wCurr, bCurr);

    float dZ = yHat - yCurr;
    vec2 dW = (1. / m) * xCurr * dZ;
    float dB = (1. / m) * dZ;
    wouti[idx] = dW.x;
    woutj[idx] = dW.y;
    bout[idx] = dB;

    lout[idx] = calculateLoss(yHat, yCurr);
}
    )";

    std::vector<char> shaderSpirv = spirv_from_string(shaderStr);

    std::vector<char> shaderData = std::vector<char>(
            kp::shader_data::shaders_glsl_logisticregression_comp_spv,
            kp::shader_data::shaders_glsl_logisticregression_comp_spv +
              kp::shader_data::shaders_glsl_logisticregression_comp_spv_len);

    {
        kp::Manager mgr;

        mgr.rebuild(params);

        std::shared_ptr<kp::Sequence> sq = mgr.sequence();

        // Record op algo base
        sq->begin();

        sq->record<kp::OpTensorSyncDevice>({ wIn, bIn });

        sq->record<kp::OpAlgoBase>(
          params,
          shaderSpirv,
          kp::Workgroup(), kp::Constants({5.0}));

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
    }

    // Based on the inputs the outputs should be at least:
    // * wi < 0.01
    // * wj > 1.0
    // * b < 0
    // TODO: Add EXPECT_DOUBLE_EQ instead
    EXPECT_LT(wIn->data()[0], 0.01);
    EXPECT_GT(wIn->data()[1], 1.0);
    EXPECT_LT(bIn->data()[0], 0.0);

    SPDLOG_WARN("Result wIn i: {}, wIn j: {}, bIn: {}",
                wIn->data()[0],
                wIn->data()[1],
                bIn->data()[0]);
}

TEST(TestLogisticRegressionAlgorithm, TestMainLogisticRegressionManualCopy)
{

    uint32_t ITERATIONS = 100;
    float learningRate = 0.1;

    kp::Constants wInVec = { 0.001, 0.001 };
    std::vector<float> bInVec = { 0 };

    std::shared_ptr<kp::Tensor> xI{ new kp::Tensor({ 0, 1, 1, 1, 1 }) };
    std::shared_ptr<kp::Tensor> xJ{ new kp::Tensor({ 0, 0, 0, 1, 1 }) };

    std::shared_ptr<kp::Tensor> y{ new kp::Tensor({ 0, 0, 0, 1, 1 }) };

    std::shared_ptr<kp::Tensor> wIn{ new kp::Tensor(
      wInVec, kp::Tensor::TensorTypes::eHost) };
    std::shared_ptr<kp::Tensor> wOutI{ new kp::Tensor({ 0, 0, 0, 0, 0 }) };
    std::shared_ptr<kp::Tensor> wOutJ{ new kp::Tensor({ 0, 0, 0, 0, 0 }) };

    std::shared_ptr<kp::Tensor> bIn{ new kp::Tensor(
      bInVec, kp::Tensor::TensorTypes::eHost) };
    std::shared_ptr<kp::Tensor> bOut{ new kp::Tensor({ 0, 0, 0, 0, 0 }) };

    std::shared_ptr<kp::Tensor> lOut{ new kp::Tensor({ 0, 0, 0, 0, 0 }) };

    std::vector<std::shared_ptr<kp::Tensor>> params = { xI,  xJ,    y,
                                                        wIn, wOutI, wOutJ,
                                                        bIn, bOut,  lOut };

    {
        kp::Manager mgr;

        mgr.rebuild(params);

        std::shared_ptr<kp::Sequence> sq = mgr.sequence();

        // Record op algo base
        sq->begin();

#ifdef KOMPUTE_SHADER_FROM_STRING
        sq->record<kp::OpAlgoBase>(
          params, "test/shaders/glsl/test_logistic_regression.comp.spv",
           kp::Workgroup(), kp::Algorithm::SpecializationContainer{{(uint32_t)5}});
#else
        sq->record<kp::OpAlgoBase>(
          params,
          std::vector<char>(
            kp::shader_data::shaders_glsl_logisticregression_comp_spv,
            kp::shader_data::shaders_glsl_logisticregression_comp_spv +
              kp::shader_data::shaders_glsl_logisticregression_comp_spv_len),
           kp::Workgroup(), kp::Constants({5.0}));
#endif

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
            wIn->mapDataIntoHostMemory();
            bIn->mapDataIntoHostMemory();
        }
    }

    // Based on the inputs the outputs should be at least:
    // * wi < 0.01
    // * wj > 1.0
    // * b < 0
    // TODO: Add EXPECT_DOUBLE_EQ instead
    EXPECT_LT(wIn->data()[0], 0.01);
    EXPECT_GT(wIn->data()[1], 1.0);
    EXPECT_LT(bIn->data()[0], 0.0);

    SPDLOG_WARN("Result wIn i: {}, wIn j: {}, bIn: {}",
                wIn->data()[0],
                wIn->data()[1],
                bIn->data()[0]);
}
