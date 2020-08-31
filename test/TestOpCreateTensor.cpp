
#include "catch2/catch.hpp"

#include "kompute/Kompute.hpp"

#include <fmt/ranges.h>

TEST_CASE("test_opcreatetensor_create_single_tensor") {

    kp::Manager mgr;

    std::vector<uint32_t> testVecA{ 9, 8, 7 };

    std::shared_ptr<kp::Tensor> tensorA{new kp::Tensor(testVecA)};

    mgr.evalOpDefault<kp::OpCreateTensor>({tensorA});

    REQUIRE(tensorA->isInit());

    REQUIRE(tensorA->data() == testVecA);

    tensorA->freeMemoryDestroyGPUResources();
    REQUIRE(!tensorA->isInit());
}

TEST_CASE("test_opcreatetensor_create_multiple_tensors_single_op") {

    kp::Manager mgr;

    std::vector<uint32_t> testVecA{ 9, 8, 7 };
    std::vector<uint32_t> testVecB{ 6, 5, 4 };

    std::shared_ptr<kp::Tensor> tensorA{new kp::Tensor(testVecA)};
    std::shared_ptr<kp::Tensor> tensorB{new kp::Tensor(testVecB)};

    mgr.evalOpDefault<kp::OpCreateTensor>({tensorA, tensorB});

    REQUIRE(tensorA->isInit());
    REQUIRE(tensorB->isInit());

    REQUIRE(tensorA->data() == testVecA);
    REQUIRE(tensorB->data() == testVecB);
}

TEST_CASE("test_opcreatetensor_create_multiple_tensors_multiple_op") {

    kp::Manager mgr;

    std::vector<uint32_t> testVecA{ 9, 8, 7 };
    std::vector<uint32_t> testVecB{ 6, 5, 4 };

    std::shared_ptr<kp::Tensor> tensorA{new kp::Tensor(testVecA)};
    std::shared_ptr<kp::Tensor> tensorB{new kp::Tensor(testVecB)};

    mgr.evalOpDefault<kp::OpCreateTensor>({tensorA});
    mgr.evalOpDefault<kp::OpCreateTensor>({tensorB});

    REQUIRE(tensorA->isInit());
    REQUIRE(tensorB->isInit());

    REQUIRE(tensorA->data() == testVecA);
    REQUIRE(tensorB->data() == testVecB);
}

TEST_CASE("test_opcreatetensor_manage_tensor_memory_when_destroyed") {

    std::vector<uint32_t> testVecA{ 9, 8, 7 };
    std::vector<uint32_t> testVecB{ 6, 5, 4 };

    std::shared_ptr<kp::Tensor> tensorA{new kp::Tensor(testVecA)};
    std::shared_ptr<kp::Tensor> tensorB{new kp::Tensor(testVecB)};

    {
        kp::Manager mgr;
        mgr.evalOpDefault<kp::OpCreateTensor>({tensorA});
        mgr.evalOpDefault<kp::OpCreateTensor>({tensorB});

        REQUIRE(tensorA->isInit());
        REQUIRE(tensorB->isInit());

        REQUIRE(tensorA->data() == testVecA);
        REQUIRE(tensorB->data() == testVecB);
    }

    REQUIRE(!tensorA->isInit());
    REQUIRE(!tensorB->isInit());
}

TEST_CASE("test_opcreatetensor_no_error_if_tensor_freed_before") {

    std::vector<uint32_t> testVecA{ 9, 8, 7 };
    std::vector<uint32_t> testVecB{ 6, 5, 4 };

    std::shared_ptr<kp::Tensor> tensorA{new kp::Tensor(testVecA)};
    std::shared_ptr<kp::Tensor> tensorB{new kp::Tensor(testVecB)};

    kp::Manager mgr;

    mgr.evalOpDefault<kp::OpCreateTensor>({tensorA});
    mgr.evalOpDefault<kp::OpCreateTensor>({tensorB});

    REQUIRE(tensorA->isInit());
    REQUIRE(tensorB->isInit());

    REQUIRE(tensorA->data() == testVecA);
    REQUIRE(tensorB->data() == testVecB);

    tensorA->freeMemoryDestroyGPUResources();
    tensorB->freeMemoryDestroyGPUResources();
    REQUIRE(!tensorA->isInit());
    REQUIRE(!tensorB->isInit());
}

