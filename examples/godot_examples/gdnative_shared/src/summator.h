/* summator.h */

#ifndef SUMMATOR_H
#define SUMMATOR_H

#include <memory>

#include "kompute/Kompute.hpp"

#include "scene/main/node.h"

class Summator : public Node {
    GDCLASS(Summator, Node);

public:
    Summator();

    void add(float value);
    void reset();
    float get_total() const;

protected:
    static void _bind_methods();

private:
    kp::Manager mManager;
    std::weak_ptr<kp::Sequence> mSequence;
    std::shared_ptr<kp::Tensor> mPrimaryTensor;
    std::shared_ptr<kp::Tensor> mSecondaryTensor;
};

#endif // SUMMATOR_H
