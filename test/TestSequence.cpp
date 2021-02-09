
#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"

TEST(TestSequence, CmdBufSequenceBeginEnd)
{
    kp::Manager mgr;

    {
        std::shared_ptr<kp::Sequence> sq =
          mgr.getOrCreateManagedSequence("newSequence");

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

TEST(TestSequence, SequenceDestructorViaManager)
{
    std::shared_ptr<kp::Sequence> sq = nullptr;

    {
        kp::Manager mgr;

        sq = mgr.getOrCreateManagedSequence("newSequence");

        EXPECT_TRUE(sq->isInit());
    }

    EXPECT_FALSE(sq->isInit());
}

