
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

    mgr.rebuildTensors({ tensorA });

    std::shared_ptr<kp::Sequence> sq =
      mgr.getOrCreateManagedSequence("newSequence");

    {
        sq->begin();

        sq->record<kp::OpTensorSyncDevice>({ tensorA });

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

    mgr.rebuildTensors({ tensorA });

    std::shared_ptr<kp::Sequence> sqTensor = mgr.createManagedSequence();

    std::shared_ptr<kp::Sequence> sq = mgr.createManagedSequence();

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

    mgr.rebuildTensors({ tensorA });

    {
        std::shared_ptr<kp::Sequence> sq =
          mgr.getOrCreateManagedSequence("newSequence");

        sq->begin();

        sq->record<kp::OpTensorSyncDevice>({ tensorA });

        sq->record<kp::OpAlgoBase>(
          { tensorA }, std::vector<char>(shader.begin(), shader.end()));

        sq->end();
        sq->eval();
    }

    {
        std::shared_ptr<kp::Sequence> sq =
          mgr.getOrCreateManagedSequence("newSequence2");

        sq->begin();

        sq->record<kp::OpAlgoBase>(
          { tensorA }, std::vector<char>(shader.begin(), shader.end()));

        sq->end();
        sq->eval();
    }

    {
        std::shared_ptr<kp::Sequence> sq =
          mgr.getOrCreateManagedSequence("newSequence3");

        sq->begin();

        sq->record<kp::OpAlgoBase>(
          { tensorA }, std::vector<char>(shader.begin(), shader.end()));

        sq->end();
        sq->eval();
    }

    {
        std::shared_ptr<kp::Sequence> sq =
          mgr.getOrCreateManagedSequence("newSequence5");

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

    mgr.rebuildTensors({ tensorA });

    {
        std::shared_ptr<kp::Sequence> sq =
          mgr.getOrCreateManagedSequence("newSequence");

        sq->begin();

        sq->record<kp::OpTensorSyncDevice>({ tensorA });

        sq->end();
        sq->eval();
    }

    {
        std::shared_ptr<kp::Sequence> sq =
          mgr.getOrCreateManagedSequence("newSequence2");

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
          mgr.getOrCreateManagedSequence("newSequence3");

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

    mgr.rebuildTensors({ tensorInA, tensorInB, tensorOut });

    mgr.evalOpDefault<kp::OpTensorSyncDevice>({ tensorInA, tensorInB, tensorOut });

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

    auto tensorInA = mgr.buildTensor({ 2.0, 4.0, 6.0 });
    auto tensorInB = mgr.buildTensor({ 0.0, 1.0, 2.0 });
    auto tensorOut = mgr.buildTensor({ 0.0, 0.0, 0.0 });

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

    mgr.evalOpDefault<kp::OpTensorSyncDevice>({ tensorInA, tensorInB, tensorOut });

    mgr.evalOpDefault<kp::OpAlgoBase>(
      { tensorInA, tensorInB, tensorOut },
      std::vector<char>(shader.begin(), shader.end()));

    mgr.evalOpDefault<kp::OpTensorSyncLocal>({ tensorOut });

    EXPECT_EQ(tensorOut->data(), std::vector<float>({ 0.0, 4.0, 12.0 }));
}
