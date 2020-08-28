
#include "catch2/catch.hpp"

#include "kompute/Kompute.hpp"

TEST_CASE("Sequence begin end recording should work as expected") {
    kp::Manager mgr;

    std::weak_ptr<kp::Sequence> sqWeakPtr = 
        mgr.getOrCreateManagedSequence("newSequence");

    if (std::shared_ptr<kp::Sequence> sq = sqWeakPtr.lock()) {
        REQUIRE(sq->eval());
        REQUIRE(!sq->isRecording());
        REQUIRE(sq->begin());
        REQUIRE(sq->isRecording());
        REQUIRE(!sq->begin());
        REQUIRE(sq->isRecording());
        REQUIRE(sq->end());
        REQUIRE(!sq->isRecording());
        REQUIRE(!sq->end());
        REQUIRE(!sq->isRecording());
        REQUIRE(sq->eval());
    }
}

