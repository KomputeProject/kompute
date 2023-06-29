
#include <iostream>
#include <memory>
#include <vector>

#include "SHADER_Mult1.hpp"
#include <kompute/Kompute.hpp> 
#include "shaders/structs/ExampleStructure2.hpp"

int main()
{
    kp::Manager mgr; 

    ExampleStructure2 a{ 1,2,3,4,5,6,7,8,9,10, {11,12,13,14,15,16,17,18,19,20} };
    ExampleStructure2 b{ 1,2,3,4,5,6,7,8,9,10, {11,12,13,14,15,16,17,18,19,20} };
    ExampleStructure2 c{ 0 };

    std::shared_ptr<kp::TensorT<ExampleStructure2>> tensorInA =
      mgr.tensorT<ExampleStructure2>({ a, b });
    std::shared_ptr<kp::TensorT<ExampleStructure2>> tensorInB =
      mgr.tensorT<ExampleStructure2>({ b, a });
    std::shared_ptr<kp::TensorT<ExampleStructure2>> tensorOut =
      mgr.tensorT<ExampleStructure2>({ c, c });

    const std::vector<std::shared_ptr<kp::Tensor>> params = { tensorInA,
                                                              tensorInB,
                                                              tensorOut };

    const std::vector<uint32_t> shader = std::vector<uint32_t>(
        shaders::SHADER_MULT1_COMP_SPV.begin(), shaders::SHADER_MULT1_COMP_SPV.end());
    std::shared_ptr<kp::Algorithm> algo = mgr.algorithm(params, shader);
     
    mgr.sequence()
      ->record<kp::OpTensorSyncDevice>(params)
      ->record<kp::OpAlgoDispatch>(algo)
      ->record<kp::OpTensorSyncLocal>(params)
      ->eval();

    // prints "Output {  0  4  12  }"
    std::cout << "Output: {  " << std::endl;
    int i = 0;
    for (const ExampleStructure2& elem : tensorOut->vector()) {
        std::cout << "Elm " << i++ << ": ";
        std::cout << elem.a << "  ";
        std::cout << elem.b << "  ";
        std::cout << elem.c << "  ";
        std::cout << elem.d << "  ";
        std::cout << elem.e << "  ";
        std::cout << elem.f << "  ";
        std::cout << elem.g << "  ";
        std::cout << elem.h << "  ";
        std::cout << elem.i << "  ";
        std::cout << elem.j << "  ";

        std::cout << "{" << "  ";

        std::cout << elem.k.a << "  ";
        std::cout << elem.k.b << "  ";
        std::cout << elem.k.c << "  ";
        std::cout << elem.k.d << "  ";
        std::cout << elem.k.e << "  ";
        std::cout << elem.k.f << "  ";
        std::cout << elem.k.g << "  ";
        std::cout << elem.k.h << "  ";
        std::cout << elem.k.i << "  ";
        std::cout << elem.k.j << "  ";

        std::cout << "} " << std::endl;
    }
    std::cout << "}" << std::endl;
}
