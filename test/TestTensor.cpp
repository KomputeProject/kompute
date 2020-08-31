
#include <catch2/catch.hpp>

#include "kompute/Kompute.hpp"

TEST_CASE("test_tensor_constructor_data") {
    std::vector<uint32_t> vec{0,1,2};
    kp::Tensor tensor(vec);
    REQUIRE( tensor.size() == vec.size() );
    REQUIRE( tensor.data() == vec );
}

TEST_CASE("test_tensor_copy_from_other_tensor_host_data") {
    std::vector<uint32_t> vecA{0,1,2};
    std::vector<uint32_t> vecB{0,0,0};

    std::shared_ptr<kp::Tensor> tensorA = std::make_shared<kp::Tensor>(
            vecA,
            kp::Tensor::TensorTypes::eStaging);
    std::shared_ptr<kp::Tensor> tensorB = std::make_shared<kp::Tensor>(
            vecA,
            kp::Tensor::TensorTypes::eStaging);

    kp::Manager mgr;

    if(std::shared_ptr<kp::Sequence> sq = 
            mgr.getOrCreateManagedSequence("new").lock())
    {
        sq->begin();

        sq->record<kp::OpCreateTensor>({tensorA, tensorB});

        tensorA->mapDataIntoHostMemory();

        tensorB->recordCopyFrom(tensorA, true);

        sq->end();

        sq->eval();

        tensorB->mapDataFromHostMemory();
    }

    REQUIRE(tensorA->data() == tensorB->data());
}

