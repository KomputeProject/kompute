
#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"

TEST(TestSequence, CmdBufSequenceBeginEnd)
{
    kp::Manager mgr;

    std::weak_ptr<kp::Sequence> sqWeakPtr =
      mgr.getOrCreateManagedSequence("newSequence");

    if (std::shared_ptr<kp::Sequence> sq = sqWeakPtr.lock()) {
        EXPECT_TRUE(sq->eval());
        EXPECT_TRUE(!sq->isRecording());
        EXPECT_TRUE(sq->begin());
        EXPECT_TRUE(sq->isRecording());
        EXPECT_TRUE(!sq->begin());
        EXPECT_TRUE(sq->isRecording());
        EXPECT_TRUE(sq->end());
        EXPECT_TRUE(!sq->isRecording());
        EXPECT_TRUE(!sq->end());
        EXPECT_TRUE(!sq->isRecording());
        EXPECT_TRUE(sq->eval());
    }
}
