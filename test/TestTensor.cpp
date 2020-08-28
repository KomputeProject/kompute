
#include <catch2/catch.hpp>

#include "kompute/Tensor.hpp"

TEST_CASE("Tensor should have same vector as initialised") {
    std::vector<uint32_t> vec{0,1,2};
    kp::Tensor tensor(vec);
    REQUIRE( tensor.data() == vec );
}

