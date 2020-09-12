
#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"

TEST(TestMultipleAlgoExecutions, SingleSequenceRecord)
{

    kp::Manager mgr;

    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor({ 0, 0, 0 }) };

    std::string shader(
      "#version 450\n"
      "layout (local_size_x = 1) in;\n"
      "layout(set = 0, binding = 0) buffer a { float pa[]; };\n"
      "void main() {\n"
      "    uint index = gl_GlobalInvocationID.x;\n"
      "    pa[index] = pa[index] + 1;\n"
      "}\n");

    std::weak_ptr<kp::Sequence> sqWeakPtr =
      mgr.getOrCreateManagedSequence("newSequence");
    if (std::shared_ptr<kp::Sequence> sq = sqWeakPtr.lock()) {
        sq->begin();

        sq->record<kp::OpTensorCreate>({ tensorA });

        sq->record<kp::OpAlgoBase<3, 1, 1>>(
          { tensorA }, std::vector<char>(shader.begin(), shader.end()));
        sq->record<kp::OpAlgoBase<3, 1, 1>>(
          { tensorA }, std::vector<char>(shader.begin(), shader.end()));
        sq->record<kp::OpAlgoBase<3, 1, 1>>(
          { tensorA }, std::vector<char>(shader.begin(), shader.end()));

        sq->record<kp::OpTensorSyncLocal>({ tensorA });

        sq->end();
        sq->eval();
    }
    sqWeakPtr.reset();

    EXPECT_EQ(tensorA->data(), std::vector<float>({ 3, 3, 3 }));
}

TEST(TestMultipleAlgoExecutions, MultipleCmdBufRecords)
{

    kp::Manager mgr;

    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor({ 0, 0, 0 }) };

    std::string shader(
      "#version 450\n"
      "layout (local_size_x = 1) in;\n"
      "layout(set = 0, binding = 0) buffer a { float pa[]; };\n"
      "void main() {\n"
      "    uint index = gl_GlobalInvocationID.x;\n"
      "    pa[index] = pa[index] + 1;\n"
      "}\n");

    std::weak_ptr<kp::Sequence> sqWeakPtr =
      mgr.getOrCreateManagedSequence("newSequence");
    if (std::shared_ptr<kp::Sequence> sq = sqWeakPtr.lock()) {
        sq->begin();

        sq->record<kp::OpTensorCreate>({ tensorA });

        sq->record<kp::OpAlgoBase<3, 1, 1>>(
          { tensorA }, std::vector<char>(shader.begin(), shader.end()));

        sq->end();
        sq->eval();

        sq->begin();

        sq->record<kp::OpAlgoBase<3, 1, 1>>(
          { tensorA }, std::vector<char>(shader.begin(), shader.end()));

        sq->end();
        sq->eval();

        sq->begin();

        sq->record<kp::OpAlgoBase<3, 1, 1>>(
          { tensorA }, std::vector<char>(shader.begin(), shader.end()));

        sq->end();
        sq->eval();

        sq->begin();

        sq->record<kp::OpTensorSyncLocal>({ tensorA });

        sq->end();
        sq->eval();
    }
    sqWeakPtr.reset();

    EXPECT_EQ(tensorA->data(), std::vector<float>({ 3, 3, 3 }));
}

TEST(TestMultipleAlgoExecutions, MultipleSequences)
{

    kp::Manager mgr;

    std::shared_ptr<kp::Tensor> tensorA{ new kp::Tensor({ 0, 0, 0 }) };

    std::string shader(
      "#version 450\n"
      "layout (local_size_x = 1) in;\n"
      "layout(set = 0, binding = 0) buffer a { float pa[]; };\n"
      "void main() {\n"
      "    uint index = gl_GlobalInvocationID.x;\n"
      "    pa[index] = pa[index] + 1;\n"
      "}\n");

    std::weak_ptr<kp::Sequence> sqWeakPtr =
      mgr.getOrCreateManagedSequence("newSequence");
    if (std::shared_ptr<kp::Sequence> sq = sqWeakPtr.lock()) {
        sq->begin();

        sq->record<kp::OpTensorCreate>({ tensorA });

        sq->record<kp::OpAlgoBase<3, 1, 1>>(
          { tensorA }, std::vector<char>(shader.begin(), shader.end()));

        sq->end();
        sq->eval();
    }

    std::weak_ptr<kp::Sequence> sqWeakPtr2 =
      mgr.getOrCreateManagedSequence("newSequence2");
    if (std::shared_ptr<kp::Sequence> sq = sqWeakPtr2.lock()) {
        sq->begin();

        sq->record<kp::OpAlgoBase<3, 1, 1>>(
          { tensorA }, std::vector<char>(shader.begin(), shader.end()));

        sq->end();
        sq->eval();
    }

    std::weak_ptr<kp::Sequence> sqWeakPtr3 =
      mgr.getOrCreateManagedSequence("newSequence3");
    if (std::shared_ptr<kp::Sequence> sq = sqWeakPtr3.lock()) {
        sq->begin();

        sq->record<kp::OpAlgoBase<3, 1, 1>>(
          { tensorA }, std::vector<char>(shader.begin(), shader.end()));

        sq->end();
        sq->eval();
    }

    std::weak_ptr<kp::Sequence> sqWeakPtr4 =
      mgr.getOrCreateManagedSequence("newSequence5");
    if (std::shared_ptr<kp::Sequence> sq = sqWeakPtr4.lock()) {
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

    std::string shader(
      "#version 450\n"
      "layout (local_size_x = 1) in;\n"
      "layout(set = 0, binding = 0) buffer a { float pa[]; };\n"
      "void main() {\n"
      "    uint index = gl_GlobalInvocationID.x;\n"
      "    pa[index] = pa[index] + 1;\n"
      "}\n");

    std::weak_ptr<kp::Sequence> sqWeakPtr =
      mgr.getOrCreateManagedSequence("newSequence");
    if (std::shared_ptr<kp::Sequence> sq = sqWeakPtr.lock()) {
        sq->begin();

        sq->record<kp::OpTensorCreate>({ tensorA });

        sq->end();
        sq->eval();
    }

    std::weak_ptr<kp::Sequence> sqWeakPtr2 =
      mgr.getOrCreateManagedSequence("newSequence2");
    if (std::shared_ptr<kp::Sequence> sq = sqWeakPtr2.lock()) {
        sq->begin();

        sq->record<kp::OpAlgoBase<3, 1, 1>>(
          { tensorA }, std::vector<char>(shader.begin(), shader.end()));

        sq->end();

        sq->eval();
        sq->eval();
        sq->eval();
    }

    std::weak_ptr<kp::Sequence> sqWeakPtr3 =
      mgr.getOrCreateManagedSequence("newSequence3");
    if (std::shared_ptr<kp::Sequence> sq = sqWeakPtr2.lock()) {
        sq->begin();

        sq->record<kp::OpTensorSyncLocal>({ tensorA });

        sq->end();

        sq->eval();
        sq->eval();
        sq->eval();
    }

    EXPECT_EQ(tensorA->data(), std::vector<float>({ 3, 3, 3 }));
}
