
#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"

TEST(TestManager, EndToEndOpMultEvalFlow)
{
    kp::Manager mgr;

    std::shared_ptr<kp::TensorT<float>> tensorLHS = mgr.tensor({ 0, 1, 2 });
    std::shared_ptr<kp::TensorT<float>> tensorRHS = mgr.tensor({ 2, 4, 6 });
    std::shared_ptr<kp::TensorT<float>> tensorOutput = mgr.tensor({ 0, 0, 0 });

    std::vector<std::shared_ptr<kp::Tensor>> params = { tensorLHS,
                                                        tensorRHS,
                                                        tensorOutput };

    mgr.sequence()
      ->eval<kp::OpTensorSyncDevice>(params)
      ->eval<kp::OpMult>(params, mgr.algorithm())
      ->eval<kp::OpTensorSyncLocal>(params);

    EXPECT_EQ(tensorOutput->vector(), std::vector<float>({ 0, 4, 12 }));
}

TEST(TestManager, EndToEndOpMultSeqFlow)
{
    kp::Manager mgr;

    std::shared_ptr<kp::TensorT<float>> tensorLHS = mgr.tensor({ 0, 1, 2 });
    std::shared_ptr<kp::TensorT<float>> tensorRHS = mgr.tensor({ 2, 4, 6 });
    std::shared_ptr<kp::TensorT<float>> tensorOutput = mgr.tensor({ 0, 0, 0 });

    std::vector<std::shared_ptr<kp::Tensor>> params = { tensorLHS,
                                                        tensorRHS,
                                                        tensorOutput };

    mgr.sequence()
      ->record<kp::OpTensorSyncDevice>(params)
      ->record<kp::OpMult>(params, mgr.algorithm())
      ->record<kp::OpTensorSyncLocal>(params)
      ->eval();

    EXPECT_EQ(tensorOutput->vector(), std::vector<float>({ 0, 4, 12 }));
}

TEST(TestManager, TestMultipleSequences)
{
    kp::Manager mgr;

    std::shared_ptr<kp::TensorT<float>> tensorLHS = mgr.tensor({ 0, 1, 2 });
    std::shared_ptr<kp::TensorT<float>> tensorRHS = mgr.tensor({ 2, 4, 6 });
    std::shared_ptr<kp::TensorT<float>> tensorOutput = mgr.tensor({ 0, 0, 0 });

    std::vector<std::shared_ptr<kp::Tensor>> params = { tensorLHS,
                                                        tensorRHS,
                                                        tensorOutput };

    mgr.sequence()->eval<kp::OpTensorSyncDevice>(params);
    mgr.sequence()->eval<kp::OpMult>(params, mgr.algorithm());
    mgr.sequence()->eval<kp::OpTensorSyncLocal>(params);

    EXPECT_EQ(tensorOutput->vector(), std::vector<float>({ 0, 4, 12 }));
}

TEST(TestManager, TestDeviceProperties)
{
    kp::Manager mgr;
    const auto properties = mgr.getDeviceProperties();
    EXPECT_GT(properties.deviceName.size(), 0);
}
