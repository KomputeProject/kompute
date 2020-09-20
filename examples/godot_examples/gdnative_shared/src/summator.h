#pragma once

#include <Godot.hpp>
#include <Node2D.hpp>

#include <memory>

#include "kompute/Kompute.hpp"

namespace godot {
class Summator : public Node2D {
private:
    GODOT_CLASS(Summator, Node2D);

public:
    Summator();

    void add(float value);
    void reset();
    float get_total() const;

    void _process(float delta);
    void _init();

    static void _register_methods();

protected:
    //static void _bind_methods();

private:
    kp::Manager mManager;
    std::weak_ptr<kp::Sequence> mSequence;
    std::shared_ptr<kp::Tensor> mPrimaryTensor;
    std::shared_ptr<kp::Tensor> mSecondaryTensor;
};

}
