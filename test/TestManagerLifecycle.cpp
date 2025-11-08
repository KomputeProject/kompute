// SPDX-License-Identifier: Apache-2.0

#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"
#include "kompute/logger/Logger.hpp"

TEST(TestManagerLifecycle, ManagerDestructionMultipleObjectTypes)
{
    std::shared_ptr<kp::TensorT<float>> tensor;
    std::shared_ptr<kp::TensorT<int32_t>> intTensor;
    std::shared_ptr<kp::Algorithm> algorithm;
    std::shared_ptr<kp::Sequence> sequence;

    {
        kp::Manager mgr;

        tensor = mgr.tensor({1.0f, 2.0f, 3.0f});
        intTensor = mgr.tensorT<int32_t>({4, 5, 6});

        std::vector<std::shared_ptr<kp::Memory>> params = {tensor, intTensor};
        algorithm = mgr.algorithm(params);
        sequence = mgr.sequence();

        EXPECT_TRUE(tensor->isInit());
        EXPECT_TRUE(intTensor->isInit());
        EXPECT_TRUE(sequence->isInit());
    }

    EXPECT_FALSE(tensor->isInit());
    EXPECT_FALSE(intTensor->isInit());
    EXPECT_FALSE(sequence->isInit());
}

TEST(TestManagerLifecycle, MultipleManagerIndependence)
{
    std::shared_ptr<kp::TensorT<float>> tensor1, tensor2;
    std::shared_ptr<kp::Sequence> seq1, seq2;

    {
        kp::Manager mgr1;
        tensor1 = mgr1.tensor({1.0f, 2.0f});
        seq1 = mgr1.sequence();

        EXPECT_TRUE(tensor1->isInit());
        EXPECT_TRUE(seq1->isInit());

        {
            kp::Manager mgr2;
            tensor2 = mgr2.tensor({3.0f, 4.0f});
            seq2 = mgr2.sequence();

            EXPECT_TRUE(tensor2->isInit());
            EXPECT_TRUE(seq2->isInit());

            EXPECT_EQ(tensor1->vector()[0], 1.0f);
            EXPECT_EQ(tensor2->vector()[0], 3.0f);
        }

        EXPECT_TRUE(tensor1->isInit());
        EXPECT_TRUE(seq1->isInit());
        EXPECT_FALSE(tensor2->isInit());
        EXPECT_FALSE(seq2->isInit());
    }

    EXPECT_FALSE(tensor1->isInit());
    EXPECT_FALSE(seq1->isInit());
}

TEST(TestManagerLifecycle, MemoryTypeCombinations)
{
    std::shared_ptr<kp::TensorT<float>> deviceTensor, hostTensor, storageTensor;

    {
        kp::Manager mgr;

        std::vector<float> data = {1.0f, 2.0f, 3.0f};

        deviceTensor = mgr.tensor(data, kp::Memory::MemoryTypes::eDevice);
        hostTensor = mgr.tensor(data, kp::Memory::MemoryTypes::eHost);
        storageTensor = mgr.tensor(data, kp::Memory::MemoryTypes::eStorage);

        EXPECT_TRUE(deviceTensor->isInit());
        EXPECT_TRUE(hostTensor->isInit());
        EXPECT_TRUE(storageTensor->isInit());

        EXPECT_EQ(deviceTensor->memoryType(), kp::Memory::MemoryTypes::eDevice);
        EXPECT_EQ(hostTensor->memoryType(), kp::Memory::MemoryTypes::eHost);
        EXPECT_EQ(storageTensor->memoryType(), kp::Memory::MemoryTypes::eStorage);

        EXPECT_EQ(hostTensor->vector(), data);

        std::vector<std::shared_ptr<kp::Memory>> mixedParams = {deviceTensor, hostTensor};
        auto sequence = mgr.sequence();

        sequence->record<kp::OpSyncDevice>(mixedParams);
        sequence->eval();
    }

    EXPECT_FALSE(deviceTensor->isInit());
    EXPECT_FALSE(hostTensor->isInit());
    EXPECT_FALSE(storageTensor->isInit());
}

TEST(TestManagerLifecycle, ResourceTrackingValidation)
{
    {
        kp::Manager mgr;

        auto tensor1 = mgr.tensorT<float>(1);
        auto tensor2 = mgr.tensorT<int32_t>(1);
        auto tensor3 = mgr.tensorT<double>(1);

        auto sequence1 = mgr.sequence();
        auto sequence2 = mgr.sequence();

        std::vector<std::shared_ptr<kp::Memory>> params = {tensor1, tensor2};
        auto algorithm = mgr.algorithm(params);

        EXPECT_TRUE(tensor1->isInit());
        EXPECT_TRUE(tensor2->isInit());
        EXPECT_TRUE(tensor3->isInit());
        EXPECT_TRUE(sequence1->isInit());
        EXPECT_TRUE(sequence2->isInit());

        sequence1->record<kp::OpSyncDevice>(params);
        sequence1->eval();

        tensor3->destroy();
        sequence2->destroy();

        EXPECT_FALSE(tensor3->isInit());
        EXPECT_FALSE(sequence2->isInit());
        EXPECT_TRUE(tensor1->isInit());
    }
}