
#include "KomputeModelML.hpp"

KomputeModelML::KomputeModelML() {

}

KomputeModelML::~KomputeModelML() {

}

void KomputeModelML::train(std::vector<float> yData, std::vector<float> xIData, std::vector<float> xJData) {

    std::vector<float> zerosData;

    for (size_t i = 0; i < yData.size(); i++) {
        zerosData.push_back(0);
    }

    uint32_t ITERATIONS = 100;
    float learningRate = 0.1;

    std::shared_ptr<kp::Tensor> xI{ new kp::Tensor(xIData) };
    std::shared_ptr<kp::Tensor> xJ{ new kp::Tensor(xJData) };

    std::shared_ptr<kp::Tensor> y{ new kp::Tensor(yData) };

    std::shared_ptr<kp::Tensor> wIn{ new kp::Tensor({ 0.001, 0.001 }) };
    std::shared_ptr<kp::Tensor> wOutI{ new kp::Tensor(zerosData) };
    std::shared_ptr<kp::Tensor> wOutJ{ new kp::Tensor(zerosData) };

    std::shared_ptr<kp::Tensor> bIn{ new kp::Tensor({ 0 }) };
    std::shared_ptr<kp::Tensor> bOut{ new kp::Tensor(zerosData) };

    std::shared_ptr<kp::Tensor> lOut{ new kp::Tensor(zerosData) };

    std::vector<std::shared_ptr<kp::Tensor>> params = { xI,  xJ,    y,
                                                        wIn, wOutI, wOutJ,
                                                        bIn, bOut,  lOut };

    {
        kp::Manager mgr;

        if (std::shared_ptr<kp::Sequence> sq =
                    mgr.getOrCreateManagedSequence("createTensors").lock()) {

            sq->begin();

            sq->record<kp::OpTensorCreate>(params);

            sq->end();
            sq->eval();

            // Record op algo base
            sq->begin();

            sq->record<kp::OpTensorSyncDevice>({ wIn, bIn });

#ifdef KOMPUTE_ANDROID_SHADER_FROM_STRING
            // Newer versions of Android are able to use shaderc to read raw string
            sq->record<kp::OpAlgoBase<>>(
                    params, std::vector<char>(LR_SHADER.begin(), LR_SHADER.end()));
#else
            // Older versions of Android require the SPIRV binary directly
            sq->record<kp::OpAlgoBase<>>(
                    params, std::vector<char>(
                            kp::shader_data::shaders_glsl_logisticregression_comp_spv,
                            kp::shader_data::shaders_glsl_logisticregression_comp_spv
                                + kp::shader_data::shaders_glsl_logisticregression_comp_spv_len
                    ));
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
            }
        }
    }

    this->mWeights = kp::Tensor(wIn->data());
    this->mBias = kp::Tensor(bIn->data());
}

std::vector<float> KomputeModelML::predict(std::vector<float> xI, std::vector<float> xJ) {
    assert(xI.size() == xJ.size());

    std::vector<float> retVector;

    // We run the inference in the CPU for simplicity
    // BUt you can also implement the inference on GPU
    // GPU implementation would speed up minibatching
    for (size_t i = 0; i < xI.size(); i++) {
        float xIVal = xI[i];
        float xJVal = xJ[i];
        float result = (xIVal * this->mWeights.data()[0]
                        + xJVal * this->mWeights.data()[1]
                        + this->mBias.data()[0]);

        // Instead of using sigmoid we'll just return full numbers
        float var = result > 0 ? 1 : 0;
        retVector.push_back(var);
    }

    return retVector;
}

std::vector<float> KomputeModelML::get_params() {
    std::vector<float> retVector;

    if(this->mWeights.size() + this->mBias.size() == 0) {
        return retVector;
    }

    retVector.push_back(this->mWeights.data()[0]);
    retVector.push_back(this->mWeights.data()[1]);
    retVector.push_back(this->mBias.data()[0]);
    retVector.push_back(99.0);

    return retVector;
}
