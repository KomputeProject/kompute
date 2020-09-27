#pragma once

#include <memory>

#include "kompute/Kompute.hpp"

#include "scene/main/node.h"

class KomputeModelMLNode : public Node {
    GDCLASS(KomputeModelMLNode, Node);

public:
    KomputeModelMLNode();

    void add(float value);
    void reset();
    float get_total() const;

    void _process(float delta);
    void _init();

protected:
    static void _bind_methods();

private:
    kp::Manager mManager;
    std::weak_ptr<kp::Sequence> mSequence;
    std::shared_ptr<kp::Tensor> mPrimaryTensor;
    std::shared_ptr<kp::Tensor> mSecondaryTensor;
};

