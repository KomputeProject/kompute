// SPDX-License-Identifier: Apache-2.0

#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"
#include "kompute/logger/Logger.hpp"

TEST(TestManager, EndToEndOpMultEvalFlow)
{
    kp::Manager mgr;

    std::shared_ptr<kp::TensorT<float>> tensorLHS = mgr.tensor({ 0, 1, 2 });
    std::shared_ptr<kp::TensorT<float>> tensorRHS = mgr.tensor({ 2, 4, 6 });
    std::shared_ptr<kp::TensorT<float>> tensorOutput = mgr.tensor({ 0, 0, 0 });

    std::vector<std::shared_ptr<kp::Memory>> params = { tensorLHS,
                                                        tensorRHS,
                                                        tensorOutput };

    mgr.sequence()
      ->eval<kp::OpSyncDevice>(params)
      ->eval<kp::OpMult>(params, mgr.algorithm())
      ->eval<kp::OpSyncLocal>(params);

    EXPECT_EQ(tensorOutput->vector(), std::vector<float>({ 0, 4, 12 }));
}

TEST(TestManager, EndToEndOpMultSeqFlow)
{
    kp::Manager mgr;

    std::shared_ptr<kp::TensorT<float>> tensorLHS = mgr.tensor({ 0, 1, 2 });
    std::shared_ptr<kp::TensorT<float>> tensorRHS = mgr.tensor({ 2, 4, 6 });
    std::shared_ptr<kp::TensorT<float>> tensorOutput = mgr.tensor({ 0, 0, 0 });

    std::vector<std::shared_ptr<kp::Memory>> params = { tensorLHS,
                                                        tensorRHS,
                                                        tensorOutput };

    mgr.sequence()
      ->record<kp::OpSyncDevice>(params)
      ->record<kp::OpMult>(params, mgr.algorithm())
      ->record<kp::OpSyncLocal>(params)
      ->eval();

    EXPECT_EQ(tensorOutput->vector(), std::vector<float>({ 0, 4, 12 }));
}

TEST(TestManager, OpMultInvalidArgs)
{
    kp::Manager mgr;

    std::shared_ptr<kp::TensorT<float>> tensorLHS = mgr.tensor({ 0, 1, 2 });
    std::shared_ptr<kp::TensorT<float>> tensorRHS = mgr.tensor({ 2, 4, 6 });
    std::shared_ptr<kp::TensorT<float>> tensorOutput = mgr.tensor({ 0, 0, 0 });

    // Only give two paramters instead of three.
    std::vector<std::shared_ptr<kp::Memory>> params = { tensorLHS, tensorRHS };

    EXPECT_THROW(
      mgr.sequence()->record<kp::OpMult>(params, mgr.algorithm())->eval(),
      std::runtime_error);
}

TEST(TestManager, TestMultipleSequences)
{
    kp::Manager mgr;

    std::shared_ptr<kp::TensorT<float>> tensorLHS = mgr.tensor({ 0, 1, 2 });
    std::shared_ptr<kp::TensorT<float>> tensorRHS = mgr.tensor({ 2, 4, 6 });
    std::shared_ptr<kp::TensorT<float>> tensorOutput = mgr.tensor({ 0, 0, 0 });

    std::vector<std::shared_ptr<kp::Memory>> params = { tensorLHS,
                                                        tensorRHS,
                                                        tensorOutput };

    mgr.sequence()->eval<kp::OpSyncDevice>(params);
    mgr.sequence()->eval<kp::OpMult>(params, mgr.algorithm());
    mgr.sequence()->eval<kp::OpSyncLocal>(params);

    EXPECT_EQ(tensorOutput->vector(), std::vector<float>({ 0, 4, 12 }));
}

TEST(TestManager, TestDeviceProperties)
{
    kp::Manager mgr;
    const vk::PhysicalDeviceProperties properties = mgr.getDeviceProperties();
    EXPECT_GT(properties.deviceName.size(), 0);
}

TEST(TestManager, TestListDevices)
{
    kp::Manager mgr;
    const std::vector<vk::PhysicalDevice> devices = mgr.listDevices();
    EXPECT_GT(devices.size(), 0);
    EXPECT_GT(devices[0].getProperties().deviceName.size(), 0);
}

TEST(TestManager, TestClearDestroy)
{
    kp::Manager mgr;

    // Running within scope to run clear
    {
        std::shared_ptr<kp::TensorT<float>> tensorLHS = mgr.tensor({ 0, 1, 2 });
        std::shared_ptr<kp::TensorT<float>> tensorRHS = mgr.tensor({ 2, 4, 6 });
        std::shared_ptr<kp::TensorT<float>> tensorOutput =
          mgr.tensor({ 0, 0, 0 });

        std::vector<std::shared_ptr<kp::Memory>> params = { tensorLHS,
                                                            tensorRHS,
                                                            tensorOutput };

        mgr.sequence()->eval<kp::OpSyncDevice>(params);
        mgr.sequence()->eval<kp::OpMult>(params, mgr.algorithm());
        mgr.sequence()->eval<kp::OpSyncLocal>(params);

        EXPECT_EQ(tensorOutput->vector(), std::vector<float>({ 0, 4, 12 }));
    }

    mgr.clear();

    mgr.destroy();
}
