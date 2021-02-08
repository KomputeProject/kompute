
#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"

TEST(TestTensor, ConstructorData)
{
    std::vector<float> vec{ 0, 1, 2 };
    kp::Tensor tensor(vec);
    EXPECT_EQ(tensor.size(), vec.size());
    EXPECT_EQ(tensor.data(), vec);
}

TEST(TestTensor, CopyFromHostData)
{
    std::vector<float> vecA{ 0, 1, 2 };
    std::vector<float> vecB{ 0, 0, 0 };

    std::shared_ptr<kp::Tensor> tensorA =
      std::make_shared<kp::Tensor>(vecA, kp::Tensor::TensorTypes::eHost);
    std::shared_ptr<kp::Tensor> tensorB =
      std::make_shared<kp::Tensor>(vecB, kp::Tensor::TensorTypes::eHost);

    kp::Manager mgr;

    mgr.rebuildTensors({ tensorA, tensorB });
    mgr.evalOpDefault<kp::OpTensorSyncDevice>({ tensorA, tensorB });

    if (std::shared_ptr<kp::Sequence> sq =
          mgr.getOrCreateManagedSequence("new")) {
        sq->begin();

        sq->record<kp::OpTensorCopy>({ tensorA, tensorB });

        sq->end();

        sq->eval();
    }

    EXPECT_EQ(tensorA->data(), tensorB->data());
}
