/* summator.cpp */

#include <vector>

#include "KomputeModelMLNode.h"

KomputeModelMLNode::KomputeModelMLNode() {
    this->_init();
}

void KomputeModelMLNode::add(float value) {
    // Set the new data in the local device
    this->mSecondaryTensor->setData({value});
    // Execute recorded sequence
    if (std::shared_ptr<kp::Sequence> sq = this->mSequence.lock()) {
        sq->eval();
    }
    else {
        throw std::runtime_error("Sequence pointer no longer available");
    }
}

void KomputeModelMLNode::reset() {
}

float KomputeModelMLNode::get_total() const {
    return this->mPrimaryTensor->data()[0];
}

void KomputeModelMLNode::_init() {
    std::cout << "CALLING INIT" << std::endl;
    this->mPrimaryTensor = this->mManager.buildTensor({ 0.0 });
    this->mSecondaryTensor = this->mManager.buildTensor({ 0.0 });
    this->mSequence = this->mManager.getOrCreateManagedSequence("AdditionSeq");

    // We now record the steps in the sequence
    if (std::shared_ptr<kp::Sequence> sq = this->mSequence.lock())
    {

        std::string shader(R"(
            #version 450

            layout (local_size_x = 1) in;

            layout(set = 0, binding = 0) buffer a { float pa[]; };
            layout(set = 0, binding = 1) buffer b { float pb[]; };

            void main() {
                uint index = gl_GlobalInvocationID.x;
                pa[index] = pb[index] + pa[index];
            }
        )");

        sq->begin();

        // First we ensure secondary tensor loads to GPU
        // No need to sync the primary tensor as it should not be changed
        sq->record<kp::OpTensorSyncDevice>(
                { this->mSecondaryTensor });

        // Then we run the operation with both tensors
        sq->record<kp::OpAlgoBase<>>(
            { this->mPrimaryTensor, this->mSecondaryTensor }, 
            std::vector<char>(shader.begin(), shader.end()));

        // We map the result back to local 
        sq->record<kp::OpTensorSyncLocal>(
                { this->mPrimaryTensor });

        sq->end();
    }
    else {
        throw std::runtime_error("Sequence pointer no longer available");
    }
}

void KomputeModelMLNode::_process(float delta) {

}

void KomputeModelMLNode::_bind_methods() {
    ClassDB::bind_method(D_METHOD("_process", "delta"), &KomputeModelMLNode::_process);
    ClassDB::bind_method(D_METHOD("_init"), &KomputeModelMLNode::_init);

    ClassDB::bind_method(D_METHOD("add", "value"), &KomputeModelMLNode::add);
    ClassDB::bind_method(D_METHOD("reset"), &KomputeModelMLNode::reset);
    ClassDB::bind_method(D_METHOD("get_total"), &KomputeModelMLNode::get_total);
}

