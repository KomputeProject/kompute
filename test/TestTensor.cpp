// SPDX-License-Identifier: Apache-2.0

#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"
#include "kompute/logger/Logger.hpp"

// Introducing custom struct that can be used for tensors
struct TestStruct
{
    float x;
    uint32_t y;
    int32_t z;

    // Creating an == operator overload for the comparison below
    bool operator==(const TestStruct rhs) const
    {
        return this->x == rhs.x && this->y == rhs.y && this->z == rhs.z;
    }
};
// Custom struct needs to be mapped the eCustom datatype
template<>
kp::Tensor::TensorDataTypes
kp::TensorT<TestStruct>::dataType()
{
    return Tensor::TensorDataTypes::eCustom;
}

TEST(TestTensor, ConstructorData)
{
    kp::Manager mgr;
    std::vector<float> vec{ 0, 1, 2 };
    std::shared_ptr<kp::TensorT<float>> tensor = mgr.tensor(vec);
    EXPECT_EQ(tensor->size(), vec.size());
    EXPECT_EQ(tensor->dataTypeMemorySize(), sizeof(float));
    EXPECT_EQ(tensor->vector(), vec);
}

TEST(TestTensor, ReserveData)
{
    kp::Manager mgr;
    std::shared_ptr<kp::Tensor> tensor = mgr.tensor(
      nullptr, 3, sizeof(float), kp::Tensor::TensorDataTypes::eFloat);
    EXPECT_EQ(tensor->size(), 3);
    EXPECT_EQ(tensor->dataTypeMemorySize(), sizeof(float));

    std::shared_ptr<kp::Tensor> tensor2 =
      mgr.tensor(3, sizeof(float), kp::Tensor::TensorDataTypes::eFloat);
    EXPECT_EQ(tensor2->size(), 3);
    EXPECT_EQ(tensor2->dataTypeMemorySize(), sizeof(float));

    std::shared_ptr<kp::TensorT<float>> tensor3 = mgr.tensorT<float>(3);
    EXPECT_EQ(tensor3->size(), 3);
    EXPECT_EQ(tensor3->dataTypeMemorySize(), sizeof(float));

    std::shared_ptr<kp::TensorT<TestStruct>> tensor4 = mgr.tensorT<TestStruct>(3);
    EXPECT_EQ(tensor3->size(), 3);
    EXPECT_EQ(tensor3->dataTypeMemorySize(), sizeof(TestStruct));
}

TEST(TestTensor, DataTypes)
{
    kp::Manager mgr;

    {
        std::vector<float> vec{ 0, 1, 2 };
        std::shared_ptr<kp::TensorT<float>> tensor = mgr.tensor(vec);
        EXPECT_EQ(tensor->dataType(), kp::Tensor::TensorDataTypes::eFloat);
    }

    {
        std::vector<int32_t> vec{ 0, 1, 2 };
        std::shared_ptr<kp::TensorT<int32_t>> tensor = mgr.tensorT(vec);
        EXPECT_EQ(tensor->dataType(), kp::Tensor::TensorDataTypes::eInt);
    }

    {
        std::vector<uint32_t> vec{ 0, 1, 2 };
        std::shared_ptr<kp::TensorT<uint32_t>> tensor = mgr.tensorT(vec);
        EXPECT_EQ(tensor->dataType(),
                  kp::Tensor::TensorDataTypes::eUnsignedInt);
    }

    {
        std::vector<double> vec{ 0, 1, 2 };
        std::shared_ptr<kp::TensorT<double>> tensor = mgr.tensorT(vec);
        EXPECT_EQ(tensor->dataType(), kp::Tensor::TensorDataTypes::eDouble);
    }
}
