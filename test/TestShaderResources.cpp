#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"

const TBuiltInResource noMaxLightResource = { /* .MaxLights = */ 0, 6, 32, 32, 64, 4096, 64, 32, 80, 32, 4096, 32, 128, 8, 16, 16, 15, -8, 7, 8, 65535, 65535, 65535, 1024, 1024, 64, 1024, 16, 8, 8, 1, 60, 64, 64, 128, 128, 8, 8, 8, 0, 0, 0, 0, 0, 8, 8, 16, 256, 1024, 1024, 64, 128, 128, 16, 1024, 4096, 128, 128, 16, 1024, 120, 32, 64, 16, 0, 0, 0, 0, 8, 8, 1, 0, 0, 0, 0, 1, 1, 16384, 4, 64, 8, 8, 4, 256, 512, 32, 1, 1, 32, 1, 1, 4, 1, {true, true, true, true, true, true, true, true, true}};

TEST(TestShaderResources, TestNoMaxLight)
{

    kp::Manager mgr; 

    auto tensorInA = mgr.tensor({ 2., 2., 2. });
    auto tensorInB = mgr.tensor({ 1., 2., 3. });
    auto tensorOut = mgr.tensor({ 0., 0., 0. });

    std::string shaderString = (R"(
        #version 450

        layout (local_size_x = 1) in;

        // The input tensors bind index is relative to index in parameter passed
        layout(set = 0, binding = 0) buffer bina { float tina[]; };
        layout(set = 0, binding = 1) buffer binb { float tinb[]; };
        layout(set = 0, binding = 2) buffer bout { float tout[]; };

        void main() {
            uint index = gl_GlobalInvocationID.x;
            
            int i = 1;
            
            while(i < 200) {
                tout[index] += tina[index] * tinb[index];
                i++;
            }
        }
    )");

    
    mgr.evalOpDefault<kp::OpAlgoBase>(
        { tensorInA, tensorInB, tensorOut },
        kp::Shader::compile_source(shaderString, "main", std::vector<std::pair<std::string,std::string>>({}), noMaxLightResource));
    
    mgr.evalOpDefault<kp::OpTensorSyncLocal>({ tensorInA, tensorInB, tensorOut });
    
    SUCCEED();
}

const TBuiltInResource noWhileLoopResource = { 32, 6, 32, 32, 64, 4096, 64, 32, 80, 32, 4096, 32, 128, 8, 16, 16, 15, -8, 7, 8, 65535, 65535, 65535, 1024, 1024, 64, 1024, 16, 8, 8, 1, 60, 64, 64, 128, 128, 8, 8, 8, 0, 0, 0, 0, 0, 8, 8, 16, 256, 1024, 1024, 64, 128, 128, 16, 1024, 4096, 128, 128, 16, 1024, 120, 32, 64, 16, 0, 0, 0, 0, 8, 8, 1, 0, 0, 0, 0, 1, 1, 16384, 4, 64, 8, 8, 4, 256, 512, 32, 1, 1, 32, 1, 1, 4, 1, {true, /* .whileLoops = */false, true, true, true, true, true, true, true}};


TEST(TestShaderResources, TestNoWhileLoop)
{

    kp::Manager mgr; 

    auto tensorInA = mgr.tensor({ 2., 2., 2. });
    auto tensorInB = mgr.tensor({ 1., 2., 3. });
    auto tensorOut = mgr.tensor({ 0., 0., 0. });

    std::string shaderString = (R"(
        #version 450

        layout (local_size_x = 1) in;

        layout(set = 0, binding = 0) buffer bina { float tina[]; };
        layout(set = 0, binding = 1) buffer binb { float tinb[]; };
        layout(set = 0, binding = 2) buffer bout { float tout[]; };

        void main() {
            uint index = gl_GlobalInvocationID.x;
            
            int i = 1;
            
            while(i < 200) {
                tout[index] += tina[index] * tinb[index];
                i++;
            }
        }
    )");

    
    mgr.evalOpDefault<kp::OpAlgoBase>(
        { tensorInA, tensorInB, tensorOut },
        kp::Shader::compile_source(shaderString, "main", std::vector<std::pair<std::string,std::string>>({}), noWhileLoopResource));
    
    mgr.evalOpDefault<kp::OpTensorSyncLocal>({ tensorInA, tensorInB, tensorOut });

    FAIL();
} 
