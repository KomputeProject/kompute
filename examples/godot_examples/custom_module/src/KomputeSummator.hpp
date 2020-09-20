#pragma once

#include <memory>

#include "kompute/Kompute.hpp"

#include "scene/main/node_2d.h"

class KomputeSummator : public Node2D {
    GDCLASS(KomputeSummator, Node2D);

public:
    KomputeSummator();

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

