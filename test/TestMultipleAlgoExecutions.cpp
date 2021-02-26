
#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"

TEST(TestMultipleAlgoExecutions, SingleSequenceRecord)
{

    kp::Manager mgr;

    std::shared_ptr<kp::Tensor> tensorA = mgr.tensor({ 0, 0, 0 });

    std::string shader(R"(
      #version 450
      layout (local_size_x = 1) in;
      layout(set = 0, binding = 0) buffer a { float pa[]; };
      void main() {
          uint index = gl_GlobalInvocationID.x;
          pa[index] = pa[index] + 1;
      })");

    std::vector<uint32_t> spirv = kp::Shader::compile_source(shader);

    {
        mgr.sequence()
            ->record<kp::OpTensorSyncDevice>({ tensorA })
            ->record<kp::OpAlgoDispatch>(mgr.algorithm({tensorA}, spirv))
            ->record<kp::OpAlgoDispatch>(mgr.algorithm({tensorA}, spirv))
            ->record<kp::OpAlgoDispatch>(mgr.algorithm({tensorA}, spirv))
            ->record<kp::OpTensorSyncLocal>({ tensorA })
            ->eval();
    }

    EXPECT_EQ(tensorA->data(), std::vector<float>({ 3, 3, 3 }));
}

TEST(TestMultipleAlgoExecutions, MultipleCmdBufRecords)
{
    kp::Manager mgr;

    std::shared_ptr<kp::Tensor> tensorA = mgr.tensor({ 0, 0, 0 });

    std::string shader(R"(
      #version 450
      layout (local_size_x = 1) in;
      layout(set = 0, binding = 0) buffer a { float pa[]; };
      void main() {
          uint index = gl_GlobalInvocationID.x;
          pa[index] = pa[index] + 1;
      })");

    std::vector<uint32_t> spirv = kp::Shader::compile_source(shader);

    std::shared_ptr<kp::Algorithm> algorithm = mgr.algorithm({tensorA}, spirv);

    std::shared_ptr<kp::Sequence> sq = mgr.sequence();

    mgr.sequence()
        ->record<kp::OpTensorSyncDevice>({ tensorA })
        ->eval();

    mgr.sequence()
        ->record<kp::OpAlgoDispatch>(algorithm)
        ->eval();

    mgr.sequence()
        ->record<kp::OpAlgoDispatch>(algorithm)
        ->eval();

    mgr.sequence()
        ->record<kp::OpAlgoDispatch>(algorithm)
        ->eval();

    mgr.sequence()
        ->record<kp::OpTensorSyncLocal>({ tensorA })
        ->eval();

    EXPECT_EQ(tensorA->data(), std::vector<float>({ 3, 3, 3 }));
}

TEST(TestMultipleAlgoExecutions, MultipleSequences)
{

    kp::Manager mgr;

    std::shared_ptr<kp::Tensor> tensorA = mgr.tensor({ 0, 0, 0 });

    std::string shader(R"(
      #version 450
      layout (local_size_x = 1) in;
      layout(set = 0, binding = 0) buffer a { float pa[]; };
      void main() {
          uint index = gl_GlobalInvocationID.x;
          pa[index] = pa[index] + 1;
      })");

    std::vector<uint32_t> spirv = kp::Shader::compile_source(shader);

    std::shared_ptr<kp::Algorithm> algorithm = mgr.algorithm({tensorA}, spirv);

    std::shared_ptr<kp::Sequence> sq = mgr.sequence();

    sq->record<kp::OpTensorSyncDevice>({ tensorA })->eval();

    sq->record<kp::OpAlgoDispatch>(algorithm)
        ->eval();

    sq->record<kp::OpAlgoDispatch>(algorithm)
        ->eval();

    sq->record<kp::OpAlgoDispatch>(algorithm)
        ->eval();

    sq->record<kp::OpTensorSyncLocal>({ tensorA })
        ->eval();

    EXPECT_EQ(tensorA->data(), std::vector<float>({ 3, 3, 3 }));
}

TEST(TestMultipleAlgoExecutions, SingleRecordMultipleEval)
{
    kp::Manager mgr;

    std::shared_ptr<kp::Tensor> tensorA = mgr.tensor({ 0, 0, 0 });

    std::string shader(R"(
      #version 450
      layout (local_size_x = 1) in;
      layout(set = 0, binding = 0) buffer a { float pa[]; };
      void main() {
          uint index = gl_GlobalInvocationID.x;
          pa[index] = pa[index] + 1;
      })");

    std::vector<uint32_t> spirv = kp::Shader::compile_source(shader);

    std::shared_ptr<kp::Algorithm> algorithm = mgr.algorithm({tensorA}, spirv);

    std::shared_ptr<kp::Sequence> sq = mgr.sequence();

    sq->record<kp::OpTensorSyncDevice>({ tensorA })->eval();

    sq->record<kp::OpAlgoDispatch>(algorithm)
        ->eval()
        ->eval()
        ->eval();

    sq->record<kp::OpTensorSyncLocal>({ tensorA })
        ->eval();

    EXPECT_EQ(tensorA->data(), std::vector<float>({ 3, 3, 3 }));
}


TEST(TestMultipleAlgoExecutions, SequenceAlgoDestroyOutsideManagerScope)
{
    std::shared_ptr<kp::Tensor> tensorA = nullptr;

    {
        std::shared_ptr<kp::Sequence> sq = nullptr;
        {
            kp::Manager mgr;

            tensorA = mgr.tensor({ 0, 0, 0 });

            std::string shader(R"(
              #version 450
              layout (local_size_x = 1) in;
              layout(set = 0, binding = 0) buffer a { float pa[]; };
              void main() {
                  uint index = gl_GlobalInvocationID.x;
                  pa[index] = pa[index] + 1;
              })");

            std::vector<uint32_t> spirv = kp::Shader::compile_source(shader);

            std::shared_ptr<kp::Algorithm> algorithm = mgr.algorithm({tensorA}, spirv);

            sq = mgr.sequence();

            sq->record<kp::OpTensorSyncDevice>({ tensorA })->eval();

            sq->record<kp::OpAlgoDispatch>(algorithm)
                ->eval()
                ->eval()
                ->eval();

            sq->record<kp::OpTensorSyncLocal>({ tensorA })
                ->eval();
        }
    }

    EXPECT_EQ(tensorA->data(), std::vector<float>({ 3, 3, 3 }));
}

