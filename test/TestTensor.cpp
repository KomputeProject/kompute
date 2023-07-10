// SPDX-License-Identifier: Apache-2.0

#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"
#include "kompute/logger/Logger.hpp"
#include "kompute/TypeContainer.hpp"

TEST(TestTensor, ConstructorData)
{
    kp::Manager mgr;
    std::vector<float> vec{ 0, 1, 2 };
    std::shared_ptr<kp::TensorT<float>> tensor = mgr.tensor(vec);
    EXPECT_EQ(tensor->size(), vec.size());
    EXPECT_EQ(tensor->dataTypeMemorySize(), sizeof(float));
    EXPECT_EQ(tensor->vector(), vec);
}

TEST(TestTensor, DataTypes)
{
    kp::Manager mgr;

    {
        std::vector<float> vec{ 0, 1, 2 };
        std::shared_ptr<kp::TensorT<float>> tensor = mgr.tensor(vec);
        EXPECT_EQ(TypeContainer<float>(), *tensor->dataType());

        EXPECT_EQ((*tensor->dataType()).name(), typeid(float).name());
    }

    {
        std::vector<int32_t> vec{ 0, 1, 2 };
        std::shared_ptr<kp::TensorT<int32_t>> tensor = mgr.tensor(vec);
        EXPECT_EQ(TypeContainer<int32_t>(), *tensor->dataType());

        EXPECT_EQ((*tensor->dataType()).name(), typeid(int32_t).name());
    }

    {
        std::vector<uint32_t> vec{ 0, 1, 2 };
        std::shared_ptr<kp::TensorT<uint32_t>> tensor = mgr.tensor(vec);
        EXPECT_EQ(TypeContainer<uint32_t>(), *tensor->dataType());

        EXPECT_EQ((*tensor->dataType()).name(), typeid(uint32_t).name());
    }

    {
        std::vector<double> vec{ 0, 1, 2 };
        std::shared_ptr<kp::TensorT<double>> tensor = mgr.tensor(vec);
        EXPECT_EQ(TypeContainer<double>(), *tensor->dataType());

        EXPECT_EQ((*tensor->dataType()).name(), typeid(double).name());
    }
}
