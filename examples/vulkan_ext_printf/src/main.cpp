#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <random>
#include <bits/stdc++.h>
#include <functional> // std::multiplies
#include <algorithm> // std::transform

#include <kompute/Kompute.hpp>
#include <shader/example_shader.hpp>

std::vector<float> generate_random_floats(int n, float min_val, float max_val) {
  // create std::vector and pre-allocate the vector with n floats
  std::vector<float> vectorF = std::vector<float>(n);
  // 1. Obtain a random seed from the hardware
  std::random_device rd;
  // 2. Initialize the generator with the seed
  std::mt19937 gen(rd()); 
  // 3. Define the distribution range [0, n)
  std::uniform_real_distribution<float> dis(min_val, max_val); 
  //fill the vector with randomly distributed floats
  for (int i = 0; i < n; i++) {vectorF[i] = dis(gen);}
  //return the filled vector
  return vectorF;
}

int main(int argc, char *argv[])
{
    int device_id = 0;

    if(argc>1){
      device_id = atoi(argv[1]);
    }else{
      std::cout<<"Using device 0"<<std::endl;
    }

    // make sure to add the extension, check vulkan_info to see if your GPU vulkan driver supports the extension
    // vulkaninfo | grep VK_KHR_shader_non_semantic_info
    const std::vector<std::string> desiredExtensions = std::vector<std::string>({
      "VK_KHR_shader_non_semantic_info",
    });
    const std::vector<uint32_t> familyQueueIndices = std::vector<uint32_t>({});

    kp::Manager mgr(device_id, familyQueueIndices, desiredExtensions);

    int vector_length = 10;

    const std::vector<float> A = generate_random_floats(vector_length, 1.0, 10.0);
    const std::vector<float> B = generate_random_floats(vector_length, 1.0, 10.0);
    const std::vector<float> C = generate_random_floats(vector_length, 0.0, 0.0);

    std::shared_ptr<kp::TensorT<float>> tensorInA = mgr.tensorT<float>(A);
    std::shared_ptr<kp::TensorT<float>> tensorInB = mgr.tensorT<float>(B);
    std::shared_ptr<kp::TensorT<float>> tensorOut = mgr.tensorT<float>(C);

    const std::vector<std::shared_ptr<kp::Memory>> params = { tensorInA,
                                                              tensorInB,
                                                              tensorOut };

    kp::Workgroup workgroup = { vector_length, 1, 1 };

    const std::vector<uint32_t> shader = std::vector<uint32_t>(
      shader::EXAMPLE_SHADER_COMP_SPV.begin(), shader::EXAMPLE_SHADER_COMP_SPV.end());
    std::shared_ptr<kp::Algorithm> algo = mgr.algorithm(params, shader, workgroup);

    mgr.sequence()
      ->record<kp::OpSyncDevice>(params)
      ->record<kp::OpAlgoDispatch>(algo)
      ->record<kp::OpSyncLocal>(params)
      ->eval();
    
    std::cout << "Output: {  ";
    for (const float& elem : tensorOut->vector()) { std::cout << elem << "  ";}
    std::cout << "}" << std::endl;
}
