
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

    {
        kp::Manager mgr;

        std::shared_ptr<kp::Tensor> xI = mgr.tensor(xIData);
        std::shared_ptr<kp::Tensor> xJ = mgr.tensor(xJData);

        std::shared_ptr<kp::Tensor> y = mgr.tensor(yData);

        std::shared_ptr<kp::Tensor> wIn = mgr.tensor({ 0.001, 0.001 });
        std::shared_ptr<kp::Tensor> wOutI = mgr.tensor(zerosData);
        std::shared_ptr<kp::Tensor> wOutJ = mgr.tensor(zerosData);

        std::shared_ptr<kp::Tensor> bIn = mgr.tensor({ 0 });
        std::shared_ptr<kp::Tensor> bOut = mgr.tensor(zerosData);

        std::shared_ptr<kp::Tensor> lOut = mgr.tensor(zerosData);

        std::vector<std::shared_ptr<kp::Tensor>> params = { xI,  xJ,    y,
                                                            wIn, wOutI, wOutJ,
                                                            bIn, bOut,  lOut };

        std::vector<uint32_t> spirv(
                    (uint32_t*)kp::shader_data::shaders_glsl_logisticregression_comp_spv,
                    (uint32_t*)(kp::shader_data::shaders_glsl_logisticregression_comp_spv
                        + kp::shader_data::shaders_glsl_logisticregression_comp_spv_len));

        std::shared_ptr<kp::Algorithm> algo =
                mgr.algorithm(params, spirv, kp::Workgroup({ 5 }), kp::Constants({ 5.0 }));

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

        KP_LOG_INFO("RESULT: <<<<<<<<<<<<<<<<<<<");
        KP_LOG_INFO("{}", wIn->data()[0]);
        KP_LOG_INFO("{}", wIn->data()[1]);
        KP_LOG_INFO("{}", bIn->data()[0]);

        this->mWeights = wIn;
        this->mBias = bIn;
    }
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
        float result = (xIVal * this->mWeights->data()[0]
                        + xJVal * this->mWeights->data()[1]
                        + this->mBias->data()[0]);

        // Instead of using sigmoid we'll just return full numbers
        float var = result > 0 ? 1 : 0;
        retVector.push_back(var);
    }

    return retVector;
}

std::vector<float> KomputeModelML::get_params() {
    std::vector<float> retVector;

    if(this->mWeights->size() + this->mBias->size() == 0) {
        return retVector;
    }

    retVector.push_back(this->mWeights->data()[0]);
    retVector.push_back(this->mWeights->data()[1]);
    retVector.push_back(this->mBias->data()[0]);
    retVector.push_back(99.0);

    return retVector;
}
