
#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"

TEST(TestMultipleAlgoExecutions, SingleSequenceRecord)
{

    kp::Manager mgr;

    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor({ 0, 0, 0 }) };

    std::string shader(R"(
      #version 450
      layout (local_size_x = 1) in;
      layout(set = 0, binding = 0) buffer a { float pa[]; };
      void main() {
          uint index = gl_GlobalInvocationID.x;
          pa[index] = pa[index] + 1;
      })");

    mgr.rebuild({ tensorA });

    std::shared_ptr<kp::Sequence> sq =
      mgr.sequence("newSequence");

    {
        sq->begin();

        sq->record<kp::OpAlgoBase>(
          { tensorA }, std::vector<char>(shader.begin(), shader.end()));
        sq->record<kp::OpAlgoBase>(
          { tensorA }, std::vector<char>(shader.begin(), shader.end()));
        sq->record<kp::OpAlgoBase>(
          { tensorA }, std::vector<char>(shader.begin(), shader.end()));

        sq->record<kp::OpTensorSyncLocal>({ tensorA });

        sq->end();
        sq->eval();
    }

    EXPECT_EQ(tensorA->data(), std::vector<float>({ 3, 3, 3 }));
}

TEST(TestMultipleAlgoExecutions, MultipleCmdBufRecords)
{
    kp::Manager mgr;

    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor({ 0, 0, 0 }) };

    std::string shader(R"(
      #version 450
      layout (local_size_x = 1) in;
      layout(set = 0, binding = 0) buffer a { float pa[]; };
      void main() {
          uint index = gl_GlobalInvocationID.x;
          pa[index] = pa[index] + 1;
      })");

    mgr.rebuild({ tensorA }, false);

    std::shared_ptr<kp::Sequence> sqTensor = mgr.sequence();

    std::shared_ptr<kp::Sequence> sq = mgr.sequence();

    // First create the tensor in a separate sequence
    sqTensor->begin();
    sqTensor->record<kp::OpTensorSyncDevice>({ tensorA });
    sqTensor->end();
    sqTensor->eval();

    // Then perform the computations
    sq->begin();
    sq->record<kp::OpAlgoBase>({ tensorA },
                               std::vector<char>(shader.begin(), shader.end()));
    sq->end();
    sq->eval();

    sq->begin();
    sq->record<kp::OpAlgoBase>({ tensorA },
                               std::vector<char>(shader.begin(), shader.end()));
    sq->end();
    sq->eval();

    sq->begin();
    sq->record<kp::OpAlgoBase>({ tensorA },
                               std::vector<char>(shader.begin(), shader.end()));
    sq->end();
    sq->eval();

    sq->begin();
    sq->record<kp::OpTensorSyncLocal>({ tensorA });
    sq->end();
    sq->eval();

    EXPECT_EQ(tensorA->data(), std::vector<float>({ 3, 3, 3 }));
}

TEST(TestMultipleAlgoExecutions, MultipleSequences)
{

    kp::Manager mgr;

    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor({ 0, 0, 0 }) };

    std::string shader(R"(
      #version 450
      layout (local_size_x = 1) in;
      layout(set = 0, binding = 0) buffer a { float pa[]; };
      void main() {
          uint index = gl_GlobalInvocationID.x;
          pa[index] = pa[index] + 1;
      })");

    mgr.rebuild({ tensorA });

    {
        std::shared_ptr<kp::Sequence> sq =
          mgr.sequence("newSequence");

        sq->begin();

        sq->record<kp::OpAlgoBase>(
          { tensorA }, std::vector<char>(shader.begin(), shader.end()));

        sq->end();
        sq->eval();
    }

    {
        std::shared_ptr<kp::Sequence> sq =
          mgr.sequence("newSequence2");

        sq->begin();

        sq->record<kp::OpAlgoBase>(
          { tensorA }, std::vector<char>(shader.begin(), shader.end()));

        sq->end();
        sq->eval();
    }

    {
        std::shared_ptr<kp::Sequence> sq =
          mgr.sequence("newSequence3");

        sq->begin();

        sq->record<kp::OpAlgoBase>(
          { tensorA }, std::vector<char>(shader.begin(), shader.end()));

        sq->end();
        sq->eval();
    }

    {
        std::shared_ptr<kp::Sequence> sq =
          mgr.sequence("newSequence5");

        sq->begin();

        sq->record<kp::OpTensorSyncLocal>({ tensorA });

        sq->end();
        sq->eval();
    }

    EXPECT_EQ(tensorA->data(), std::vector<float>({ 3, 3, 3 }));
}

TEST(TestMultipleAlgoExecutions, SingleRecordMultipleEval)
{

    kp::Manager mgr;

    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor({ 0, 0, 0 }) };

    std::string shader(R"(
      #version 450
      layout (local_size_x = 1) in;
      layout(set = 0, binding = 0) buffer a { float pa[]; };
      void main() {
          uint index = gl_GlobalInvocationID.x;
          pa[index] = pa[index] + 1;
      })");

    mgr.rebuild({ tensorA }, false);

    {
        std::shared_ptr<kp::Sequence> sq =
          mgr.sequence("newSequence");

        sq->begin();

        sq->record<kp::OpTensorSyncDevice>({ tensorA });

        sq->end();
        sq->eval();
    }

    {
        std::shared_ptr<kp::Sequence> sq =
          mgr.sequence("newSequence2");

        sq->begin();

        sq->record<kp::OpAlgoBase>(
          { tensorA }, std::vector<char>(shader.begin(), shader.end()));

        sq->end();

        sq->eval();
        sq->eval();
        sq->eval();
    }

    {
        std::shared_ptr<kp::Sequence> sq =
          mgr.sequence("newSequence3");

        sq->begin();

        sq->record<kp::OpTensorSyncLocal>({ tensorA });

        sq->end();

        sq->eval();
        sq->eval();
        sq->eval();
    }

    EXPECT_EQ(tensorA->data(), std::vector<float>({ 3, 3, 3 }));
}

TEST(TestMultipleAlgoExecutions, ManagerEvalMultSourceStrOpCreate)
{

    kp::Manager mgr;

    std::shared_ptr<kp::Tensor> tensorInA{ new kp::Tensor({ 2.0, 4.0, 6.0 }) };
    std::shared_ptr<kp::Tensor> tensorInB{ new kp::Tensor({ 0.0, 1.0, 2.0 }) };
    std::shared_ptr<kp::Tensor> tensorOut{ new kp::Tensor({ 0.0, 0.0, 0.0 }) };

    mgr.rebuild({ tensorInA, tensorInB, tensorOut });

    std::string shader(R"(
        // The version to use 
        #version 450

        // The execution structure
        layout (local_size_x = 1) in;

        // The buffers are provided via the tensors
        layout(binding = 0) buffer bufA { float a[]; };
        layout(binding = 1) buffer bufB { float b[]; };
        layout(binding = 2) buffer bufOut { float o[]; };

        void main() {
            uint index = gl_GlobalInvocationID.x;

            o[index] = a[index] * b[index];
        }
      )");

    mgr.evalOpDefault<kp::OpAlgoBase>(
      { tensorInA, tensorInB, tensorOut },
      std::vector<char>(shader.begin(), shader.end()));

    mgr.evalOpDefault<kp::OpTensorSyncLocal>({ tensorOut });

    EXPECT_EQ(tensorOut->data(), std::vector<float>({ 0.0, 4.0, 12.0 }));
}

TEST(TestMultipleAlgoExecutions, ManagerEvalMultSourceStrMgrCreate)
{

    kp::Manager mgr;

    auto tensorInA = mgr.tensor(
      { 2.0, 4.0, 6.0 }, kp::Tensor::TensorTypes::eDevice, false);
    auto tensorInB = mgr.tensor(
      { 0.0, 1.0, 2.0 }, kp::Tensor::TensorTypes::eDevice, false);
    auto tensorOut = mgr.tensor(
      { 0.0, 0.0, 0.0 }, kp::Tensor::TensorTypes::eDevice, false);

    std::string shader(R"(
        // The version to use 
        #version 450

        // The execution structure
        layout (local_size_x = 1) in;

        // The buffers are provided via the tensors
        layout(binding = 0) buffer bufA { float a[]; };
        layout(binding = 1) buffer bufB { float b[]; };
        layout(binding = 2) buffer bufOut { float o[]; };

        void main() {
            uint index = gl_GlobalInvocationID.x;

            o[index] = a[index] * b[index];
        }
      )");

    mgr.evalOpDefault<kp::OpTensorSyncDevice>(
      { tensorInA, tensorInB, tensorOut });

    mgr.evalOpDefault<kp::OpAlgoBase>(
      { tensorInA, tensorInB, tensorOut },
      std::vector<char>(shader.begin(), shader.end()));

    mgr.evalOpDefault<kp::OpTensorSyncLocal>({ tensorOut });

    EXPECT_EQ(tensorOut->data(), std::vector<float>({ 0.0, 4.0, 12.0 }));
}

TEST(TestMultipleAlgoExecutions, SequenceAlgoDestroyOutsideManagerScope)
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
              { tensorA }, std::vector<char>(shader.begin(), shader.end()));
            sq->end();

            sq->eval();

            mgr.evalOpDefault<kp::OpTensorSyncLocal>({ tensorA });
        }
    }
    EXPECT_EQ(tensorA->data(), std::vector<float>({ 1, 1, 1 }));
}

TEST(TestMultipleAlgoExecutions, TestAlgorithmSpecialized)
{
    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor({ 0, 0, 0 }) };
    std::shared_ptr<kp::Tensor> tensorB{ new kp::Tensor({ 0, 0, 0 }) };

    std::string shader(R"(
      #version 450
      layout (constant_id = 0) const float cOne = 1;
      layout (constant_id = 1) const float cTwo = 1;
      layout (local_size_x = 1) in;
      layout(set = 0, binding = 0) buffer a { float pa[]; };
      layout(set = 0, binding = 1) buffer b { float pb[]; };
      void main() {
          uint index = gl_GlobalInvocationID.x;
          pa[index] = cOne;
          pb[index] = cTwo;
      })");

    {
        std::shared_ptr<kp::Sequence> sq = nullptr;

        {
            kp::Manager mgr;

            mgr.rebuild({ tensorA, tensorB });

            sq = mgr.sequence();

            auto spec = std::vector<float>({5.0, 0.3});

            sq->begin();
            sq->record<kp::OpAlgoBase>(
              { tensorA, tensorB },
              std::vector<char>(shader.begin(), shader.end()),
              std::array<uint32_t, 3>(), spec);
            sq->end();

            sq->eval();

            mgr.evalOpDefault<kp::OpTensorSyncLocal>({ tensorA, tensorB });

            EXPECT_EQ(tensorA->data(), std::vector<float>({ 5, 5, 5 }));
            EXPECT_EQ(tensorB->data(), std::vector<float>({ 0.3, 0.3, 0.3 }));
        }
    }
}
