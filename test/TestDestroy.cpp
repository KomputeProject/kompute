
#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"

TEST(TestDestroy, TestDestroyTensorSingle)
{
    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor({ 0, 0, 0 }) };

    std::string shader(R"(
      #version 450
      layout (local_size_x = 1) in;
      layout(set = 0, binding = 0) buffer a { float pa[]; };
      void main() {
          uint index = gl_GlobalInvocationID.x;
          pa[index] = pa[index] + 1;
      })");

    {
        std::shared_ptr<kp::Sequence> sq = nullptr;

        {
            kp::Manager mgr;

            mgr.rebuild({ tensorA });

            sq = mgr.sequence();

            sq->begin();
            sq->record<kp::OpAlgoBase>(
              { tensorA }, kp::Shader::compile_source(shader));
            sq->end();

            sq->eval();

            mgr.evalOpDefault<kp::OpTensorSyncLocal>({ tensorA });

            mgr.destroy(tensorA);

            EXPECT_FALSE(tensorA->isInit());
        }
    }
    EXPECT_EQ(tensorA->data(), std::vector<float>({ 1, 1, 1 }));
}

TEST(TestDestroy, TestDestroyTensorVector)
{
    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor({ 1, 1, 1 }) };
    std::shared_ptr<kp::Tensor> tensorB{ new kp::Tensor({ 1, 1, 1 }) };

    std::string shader(R"(
      #version 450
      layout (local_size_x = 1) in;
      layout(set = 0, binding = 0) buffer a { float pa[]; };
      layout(set = 0, binding = 1) buffer b { float pb[]; };
      void main() {
          uint index = gl_GlobalInvocationID.x;
          pa[index] = pa[index] + 1;
          pb[index] = pb[index] + 2;
      })");

    {
        std::shared_ptr<kp::Sequence> sq = nullptr;

        {
            kp::Manager mgr;

            mgr.rebuild({ tensorA, tensorB });

            sq = mgr.sequence();

            sq->begin();
            sq->record<kp::OpAlgoBase>(
              { tensorA, tensorB }, kp::Shader::compile_source(shader));
            sq->end();

            sq->eval();

            mgr.evalOpDefault<kp::OpTensorSyncLocal>({ tensorA, tensorB });

            mgr.destroy({ tensorA, tensorB });

            EXPECT_FALSE(tensorA->isInit());
            EXPECT_FALSE(tensorB->isInit());
        }
    }
    EXPECT_EQ(tensorA->data(), std::vector<float>({ 2, 2, 2 }));
    EXPECT_EQ(tensorB->data(), std::vector<float>({ 3, 3, 3 }));
}

TEST(TestDestroy, TestDestroyTensorVectorUninitialised)
{
    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor({ 1, 1, 1 }) };
    std::shared_ptr<kp::Tensor> tensorB{ new kp::Tensor({ 1, 1, 1 }) };

    {
        std::shared_ptr<kp::Sequence> sq = nullptr;

        {
            kp::Manager mgr;

            mgr.rebuild({ tensorA, tensorB });

            mgr.destroy({ tensorA, tensorB });

            EXPECT_FALSE(tensorA->isInit());
            EXPECT_FALSE(tensorB->isInit());
        }
    }
    EXPECT_EQ(tensorA->data(), std::vector<float>({ 1, 1, 1 }));
    EXPECT_EQ(tensorA->data(), std::vector<float>({ 1, 1, 1 }));
}

TEST(TestDestroy, TestDestroySequenceSingle)
{
    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor({ 0, 0, 0 }) };

    std::string shader(R"(
      #version 450
      layout (local_size_x = 1) in;
      layout(set = 0, binding = 0) buffer a { float pa[]; };
      void main() {
          uint index = gl_GlobalInvocationID.x;
          pa[index] = pa[index] + 1;
      })");

    {
        std::shared_ptr<kp::Sequence> sq = nullptr;

        {
            kp::Manager mgr;

            mgr.rebuild({ tensorA });

            sq = mgr.sequence();

            sq->begin();
            sq->record<kp::OpAlgoBase>(
              { tensorA }, kp::Shader::compile_source(shader));
            sq->end();

            sq->eval();

            mgr.evalOpDefault<kp::OpTensorSyncLocal>({ tensorA });

            mgr.destroy(sq);

            EXPECT_FALSE(sq->isInit());
        }
    }
    EXPECT_EQ(tensorA->data(), std::vector<float>({ 1, 1, 1 }));
}

TEST(TestDestroy, TestDestroySequenceVector)
{
    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor({ 0, 0, 0 }) };

    std::string shader(R"(
      #version 450
      layout (local_size_x = 1) in;
      layout(set = 0, binding = 0) buffer a { float pa[]; };
      void main() {
          uint index = gl_GlobalInvocationID.x;
          pa[index] = pa[index] + 1;
      })");

    {
        std::shared_ptr<kp::Sequence> sq1 = nullptr;
        std::shared_ptr<kp::Sequence> sq2 = nullptr;

        {
            kp::Manager mgr;

            mgr.rebuild({ tensorA });

            sq1 = mgr.sequence("One");
            sq1->begin();
            sq1->record<kp::OpAlgoBase>(
              { tensorA }, kp::Shader::compile_source(shader));
            sq1->end();
            sq1->eval();

            sq2 = mgr.sequence("Two");
            sq2->begin();
            sq2->record<kp::OpAlgoBase>(
              { tensorA }, kp::Shader::compile_source(shader));
            sq2->end();
            sq2->eval();

            mgr.evalOpDefault<kp::OpTensorSyncLocal>({ tensorA });

            mgr.destroy({ sq1, sq2 });

            EXPECT_FALSE(sq1->isInit());
            EXPECT_FALSE(sq2->isInit());
        }
    }
    EXPECT_EQ(tensorA->data(), std::vector<float>({ 2, 2, 2 }));
}

TEST(TestDestroy, TestDestroySequenceNameSingleInsideManager)
{
    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor({ 0, 0, 0 }) };

    std::string shader(R"(
      #version 450
      layout (local_size_x = 1) in;
      layout(set = 0, binding = 0) buffer a { float pa[]; };
      void main() {
          uint index = gl_GlobalInvocationID.x;
          pa[index] = pa[index] + 1;
      })");

    {
        kp::Manager mgr;
        {
            mgr.rebuild({ tensorA });

            mgr.evalOp<kp::OpAlgoBase>(
              { tensorA }, "one",
              kp::Shader::compile_source(shader));

            mgr.evalOp<kp::OpAlgoBase>(
              { tensorA }, "two",
              kp::Shader::compile_source(shader));

            mgr.evalOpDefault<kp::OpTensorSyncLocal>({ tensorA });

            mgr.destroy("one");
            mgr.destroy("two");
        }
    }
    EXPECT_EQ(tensorA->data(), std::vector<float>({ 2, 2, 2 }));
}

TEST(TestDestroy, TestDestroySequenceNameSingleOutsideManager)
{
    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor({ 0, 0, 0 }) };

    std::string shader(R"(
      #version 450
      layout (local_size_x = 1) in;
      layout(set = 0, binding = 0) buffer a { float pa[]; };
      void main() {
          uint index = gl_GlobalInvocationID.x;
          pa[index] = pa[index] + 1;
      })");

    {
        std::shared_ptr<kp::Sequence> sq1 = nullptr;

        {
            kp::Manager mgr;

            mgr.rebuild({ tensorA });

            sq1 = mgr.sequence("One");
            sq1->begin();
            sq1->record<kp::OpAlgoBase>(
              { tensorA }, kp::Shader::compile_source(shader));
            sq1->end();
            sq1->eval();

            mgr.evalOpDefault<kp::OpTensorSyncLocal>({ tensorA });

            mgr.destroy("One");

            EXPECT_FALSE(sq1->isInit());
        }
    }
    EXPECT_EQ(tensorA->data(), std::vector<float>({ 1, 1, 1 }));
}

TEST(TestDestroy, TestDestroySequenceNameVectorInsideManager)
{
    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor({ 0, 0, 0 }) };

    std::string shader(R"(
      #version 450
      layout (local_size_x = 1) in;
      layout(set = 0, binding = 0) buffer a { float pa[]; };
      void main() {
          uint index = gl_GlobalInvocationID.x;
          pa[index] = pa[index] + 1;
      })");

    {
        kp::Manager mgr;
        {
            mgr.rebuild({ tensorA });

            mgr.evalOp<kp::OpAlgoBase>(
              { tensorA }, "one",
              kp::Shader::compile_source(shader));

            mgr.evalOp<kp::OpAlgoBase>(
              { tensorA }, "two",
              kp::Shader::compile_source(shader));

            mgr.evalOpDefault<kp::OpTensorSyncLocal>({ tensorA });

            mgr.destroy(std::vector<std::string>({"one", "two"}));
        }
    }
    EXPECT_EQ(tensorA->data(), std::vector<float>({ 2, 2, 2 }));
}

TEST(TestDestroy, TestDestroySequenceNameVectorOutsideManager)
{
    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor({ 0, 0, 0 }) };

    std::string shader(R"(
      #version 450
      layout (local_size_x = 1) in;
      layout(set = 0, binding = 0) buffer a { float pa[]; };
      void main() {
          uint index = gl_GlobalInvocationID.x;
          pa[index] = pa[index] + 1;
      })");

    {
        kp::Manager mgr;
        {
            mgr.rebuild({ tensorA });

            mgr.evalOp<kp::OpAlgoBase>(
              { tensorA }, "one",
              kp::Shader::compile_source(shader));

            mgr.evalOp<kp::OpAlgoBase>(
              { tensorA }, "two",
              kp::Shader::compile_source(shader));

            mgr.evalOpDefault<kp::OpTensorSyncLocal>({ tensorA });

            mgr.destroy(std::vector<std::string>({"one", "two"}));
        }
    }
    EXPECT_EQ(tensorA->data(), std::vector<float>({ 2, 2, 2 }));
}

TEST(TestDestroy, TestDestroySequenceNameDefaultOutsideManager)
{
    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor({ 0, 0, 0 }) };

    std::string shader(R"(
      #version 450
      layout (local_size_x = 1) in;
      layout(set = 0, binding = 0) buffer a { float pa[]; };
      void main() {
          uint index = gl_GlobalInvocationID.x;
          pa[index] = pa[index] + 1;
      })");

    {
        kp::Manager mgr;
        {
            mgr.rebuild({ tensorA });

            mgr.evalOpDefault<kp::OpAlgoBase>(
              { tensorA },
              kp::Shader::compile_source(shader));

            mgr.evalOpDefault<kp::OpTensorSyncLocal>({ tensorA });

            mgr.destroy(KP_DEFAULT_SESSION);
        }
    }
    EXPECT_EQ(tensorA->data(), std::vector<float>({ 1, 1, 1 }));
}
