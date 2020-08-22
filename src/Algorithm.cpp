#include <fstream>

#include "Algorithm.hpp"

namespace kp {

Algorithm::Algorithm()
{
    SPDLOG_DEBUG("Kompute Algorithm base constructor");
}

Algorithm::Algorithm(std::shared_ptr<vk::Device> device, std::shared_ptr<vk::CommandBuffer> commandBuffer)
{
    SPDLOG_DEBUG("Kompute Algorithm Constructor with device");

    this->mDevice = device;
    this->mCommandBuffer = commandBuffer;
}

Algorithm::~Algorithm()
{
    SPDLOG_DEBUG("Kompute Algorithm Destructor started");

    if (!this->mDevice) {
        spdlog::error(
          "Kompute Algorithm destructor reached with null Device pointer");
        return;
    }
}

void Algorithm::init(std::string shaderFilePath,
                   std::vector<std::shared_ptr<Tensor>> tensorParams) {
    SPDLOG_DEBUG("Kompute Algorithm init started");

    spdlog::info("Loading shader with file path {}", shaderFilePath);

    // TODO: Move to util function
    this->createParameters(tensorParams);
    this->createShaderModule(shaderFilePath);
    this->createPipeline();
}

void Algorithm::createParameters(std::vector<std::shared_ptr<Tensor>>& tensorParams) {
    std::vector<vk::DescriptorPoolSize> descriptorPoolSizes;

    for (std::shared_ptr<Tensor> tensorParam : tensorParams) {
        descriptorPoolSizes.push_back(
          vk::DescriptorPoolSize(
            vk::DescriptorType::eStorageBuffer, 
            1 // Descriptor count
          )
        );
    }

    // TODO: Explore design for having more than 1 set configurable
    vk::DescriptorPoolCreateInfo descriptorPoolInfo(
        vk::DescriptorPoolCreateFlags(), 
        1, // Max sets
        descriptorPoolSizes.size(),
        descriptorPoolSizes.data());

    this->mDescriptorPool = std::make_shared<vk::DescriptorPool>();
    this->mDevice->createDescriptorPool(&descriptorPoolInfo, nullptr, this->mDescriptorPool.get());

    // TODO: Explore allowing descriptor set bind index
    std::vector<vk::DescriptorSetLayoutBinding> descriptorSetBindings;
    for (size_t i = 0; i < tensorParams.size(); i++) {
        descriptorSetBindings.push_back(
            vk::DescriptorSetLayoutBinding(
                i, // Binding index
                vk::DescriptorType::eStorageBuffer,
                1, // Descriptor count
                vk::ShaderStageFlagBits::eCompute)
        );
    }

    // This is the component that is fed into the pipeline
    vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutInfo(
        vk::DescriptorSetLayoutCreateFlags(),
        descriptorSetBindings.size(),
        descriptorSetBindings.data()
    );

    // TODO: We createa  signle descriptor set layout which would have to be extended if multiple set layouts to be supported
    this->mDescriptorSetLayout = std::make_shared<vk::DescriptorSetLayout>();
    this->mDevice->createDescriptorSetLayout(&descriptorSetLayoutInfo, nullptr, this->mDescriptorSetLayout.get());

    vk::DescriptorSetAllocateInfo descriptorSetAllocateInfo(
        *this->mDescriptorPool, 
        1, // Descriptor set layout count
        this->mDescriptorSetLayout.get());

    std::vector<vk::DescriptorSet> descriptorSets =
        this->mDevice->allocateDescriptorSets(descriptorSetAllocateInfo);

    if (descriptorSets.size() != tensorParams.size()) {
        throw std::runtime_error("Number of descriptor sets does not match number of paramters");
    }

    std::vector<vk::WriteDescriptorSet> computeWriteDescriptorSets;
    for (size_t i = 0; i < descriptorSets.size(); i++) {

        std::shared_ptr<Tensor> currTensor = tensorParams[i];
        vk::DescriptorSet& currDescriptorSet = descriptorSets[i];
        this->mDescriptorSets.push_back(std::make_shared<vk::DescriptorSet>(currDescriptorSet));

        vk::DescriptorBufferInfo descriptorBufferInfo = currTensor->constructDescriptorBufferInfo();

        computeWriteDescriptorSets.push_back(
            vk::WriteDescriptorSet());
    }

    this->mDevice->updateDescriptorSets(computeWriteDescriptorSets, nullptr);
}

void Algorithm::createShaderModule(std::string shaderFilePath) {
    std::ifstream fileStream(
      shaderFilePath, std::ios::binary | std::ios::in | std::ios::ate);

    size_t shaderFileSize = fileStream.tellg();
    fileStream.seekg(0, std::ios::beg);
    char* shaderFileData = new char[shaderFileSize];
    fileStream.read(shaderFileData, shaderFileSize);
    fileStream.close();

    vk::ShaderModuleCreateInfo shaderModuleInfo(vk::ShaderModuleCreateFlags(), shaderFileSize, (uint32_t*)shaderFileData);

    this->mFreeShaderModule = true;
    this->mShaderModule = std::shared_ptr<vk::ShaderModule>();
    this->mDevice->createShaderModule(&shaderModuleInfo, nullptr, this->mShaderModule.get());
}

void Algorithm::createPipeline() {
    SPDLOG_DEBUG("Kompute Algorithm calling create Pipeline");

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo(
        vk::PipelineLayoutCreateFlags(),
        1, // Set layout count
        this->mDescriptorSetLayout.get());

    this->mPipelineLayout = std::make_shared<vk::PipelineLayout>();
    this->mDevice->createPipelineLayout(&pipelineLayoutInfo, nullptr, this->mPipelineLayout.get());

    vk::PipelineShaderStageCreateInfo shaderStage(vk::PipelineShaderStageCreateFlags(), vk::ShaderStageFlagBits::eCompute, *this->mShaderModule, "main", nullptr);

    vk::ComputePipelineCreateInfo pipelineInfo(vk::PipelineCreateFlags(), shaderStage, *this->mPipelineLayout, vk::Pipeline(), 0);

    // TODO: Confirm what the best structure is with pipeline cache
    this->mFreePipelineCache = true;
    vk::PipelineCacheCreateInfo pipelineCacheInfo = vk::PipelineCacheCreateInfo();
    this->mPipelineCache = std::make_shared<vk::PipelineCache>();
    this->mDevice->createPipelineCache(&pipelineCacheInfo, nullptr, this->mPipelineCache.get());

    vk::ResultValue<vk::Pipeline> pipelineResult = this->mDevice->createComputePipeline(*this->mPipelineCache, pipelineInfo);

    if (pipelineResult.result != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to create pipeline result: " + vk::to_string(pipelineResult.result));
    }

    this->mFreePipeline = true;
    this->mPipeline = std::make_shared<vk::Pipeline>(pipelineResult.value);
}

void Algorithm::recordDispatch(uint32_t x, uint32_t y, uint32_t z) {
    SPDLOG_DEBUG("Kompute Algorithm calling record dispatch");

    this->mCommandBuffer->bindPipeline(vk::PipelineBindPoint::eCompute, *this->mPipeline);

    // TODO: Simplify interaction given we store array of pointers
    std::vector<vk::DescriptorSet> descriptorSetRefs(this->mDescriptorSets.size());
    for (size_t i = 0; i < this->mDescriptorSets.size(); i++) {
        descriptorSetRefs[i] = *this->mDescriptorSets[i];
    }

    this->mCommandBuffer->bindDescriptorSets(vk::PipelineBindPoint::eCompute, *this->mPipelineLayout, 0, descriptorSetRefs, nullptr);

    this->mCommandBuffer->dispatch(x, y, z);
}

}
