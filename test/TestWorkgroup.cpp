
#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"

#include "kompute_test/shaders/shadertest_workgroup.hpp"


TEST(TestWorkgroup, TestSimpleWorkgroup)
{
    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor(std::vector<float>(16 * 8)) };
    std::shared_ptr<kp::Tensor> tensorB{ new kp::Tensor(std::vector<float>(16 * 8)) };

    {
        std::shared_ptr<kp::Sequence> sq = nullptr;

        {
            kp::Manager mgr;

            mgr.rebuild({ tensorA, tensorB });

            kp::Workgroup workgroup = {16, 8, 1};

            sq = mgr.sequence();
            sq->begin();
            sq->record<kp::OpAlgoCreate>(
              { tensorA, tensorB },
              std::vector<uint32_t>(
                (uint32_t*)kp::shader_data::test_shaders_glsl_test_workgroup_comp_spv,
                (uint32_t*)(kp::shader_data::test_shaders_glsl_test_workgroup_comp_spv +
                  kp::shader_data::test_shaders_glsl_test_workgroup_comp_spv_len)),
                workgroup);
            sq->end();

            sq->eval();

            mgr.evalOpDefault<kp::OpTensorSyncLocal>({ tensorA, tensorB });
        }
    }

    std::vector<float> expectedA = { 0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15};

    std::vector<float> expectedB = { 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7 };

    EXPECT_EQ(tensorA->data(), expectedA);
    EXPECT_EQ(tensorB->data(), expectedB);
}

