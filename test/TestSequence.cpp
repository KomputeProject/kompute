
#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"

TEST(TestSequence, SequenceDestructorViaManager)
{
    std::shared_ptr<kp::Sequence> sq = nullptr;

    {
        kp::Manager mgr;

        sq = mgr.sequence();

        EXPECT_TRUE(sq->isInit());
    }

    EXPECT_FALSE(sq->isInit());
}
