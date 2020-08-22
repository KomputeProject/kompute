#include <fstream>

#include "Algorithm.hpp"

namespace kp {

Algorithm::Algorithm()
{
    SPDLOG_DEBUG("Kompute Algorithm base constructor");
}

Algorithm::Algorithm(std::shared_ptr<vk::Device> device,
                     std::shared_ptr<vk::CommandBuffer> commandBuffer)
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

void
Algorithm::init(std::string shaderFilePath,
                std::vector<std::shared_ptr<Tensor>> tensorParams)
{
    SPDLOG_DEBUG("Kompute Algorithm init started");

    // TODO: Move to util function
    this->createParameters(tensorParams);
    this->createShaderModule(shaderFilePath);
    this->createPipeline();
}

void Algorithm::createDescriptorPool() {

}

void
Algorithm::createParameters(std::vector<std::shared_ptr<Tensor>>& tensorParams)
{
    SPDLOG_DEBUG("Kompute Algorithm createParameters started");

    // TODO: Explore design for having multiple descriptor pool sizes
    std::vector<vk::DescriptorPoolSize> descriptorPoolSizes = {
        vk::DescriptorPoolSize(vk::DescriptorType::eStorageBuffer,
                               static_cast<uint32_t>(tensorParams.size()) // Descriptor count
                               )
    };

    // TODO: Explore design for having more than 1 set configurable
    vk::DescriptorPoolCreateInfo descriptorPoolInfo(
      vk::DescriptorPoolCreateFlags(),
      1, // Max sets
      static_cast<uint32_t>(descriptorPoolSizes.size()),
      descriptorPoolSizes.data());

    SPDLOG_DEBUG("Kompute Algorithm creating descriptor pool");
    this->mDescriptorPool = std::make_shared<vk::DescriptorPool>();
    this->mDevice->createDescriptorPool(
      &descriptorPoolInfo, nullptr, this->mDescriptorPool.get());

    std::vector<vk::DescriptorSetLayoutBinding> descriptorSetBindings;
    // TODO: Explore allowing descriptor set bind index to be configurable by
    // user to specify which tensors woudl go on each binding
    for (size_t i = 0; i < tensorParams.size(); i++) {
        descriptorSetBindings.push_back(
          vk::DescriptorSetLayoutBinding(i, // Binding index
                                         vk::DescriptorType::eStorageBuffer,
                                         1, // Descriptor count
                                         vk::ShaderStageFlagBits::eCompute));
    }

    // This is the component that is fed into the pipeline
    vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutInfo(
      vk::DescriptorSetLayoutCreateFlags(),
      static_cast<uint32_t>(descriptorSetBindings.size()),
      descriptorSetBindings.data());

    SPDLOG_DEBUG("Kompute Algorithm creating descriptor set layout");
    // TODO: We createa  signle descriptor set layout which would have to be
    // extended if multiple set layouts to be supported
    this->mDescriptorSetLayout = std::make_shared<vk::DescriptorSetLayout>();
    this->mDevice->createDescriptorSetLayout(
      &descriptorSetLayoutInfo, nullptr, this->mDescriptorSetLayout.get());

    vk::DescriptorSetAllocateInfo descriptorSetAllocateInfo(
      *this->mDescriptorPool,
      1, // Descriptor set layout count
      this->mDescriptorSetLayout.get());

    SPDLOG_DEBUG("Kompute Algorithm allocating descriptor sets");
    this->mDescriptorSet = std::make_shared<vk::DescriptorSet>();
    this->mDevice->allocateDescriptorSets(&descriptorSetAllocateInfo,
                                          this->mDescriptorSet.get());

    std::vector<vk::DescriptorBufferInfo> descriptorBufferInfos;
    for (size_t i = 0; i < tensorParams.size(); i++) {
        descriptorBufferInfos.push_back(tensorParams[i]->constructDescriptorBufferInfo());
    }
    // TODO: Explore design exposing the destination array element
    std::vector<vk::WriteDescriptorSet> computeWriteDescriptorSets;
    for (size_t i = 0; i < tensorParams.size(); i++) {

        vk::DescriptorBufferInfo descriptorBufferInfo =
          tensorParams[i]->constructDescriptorBufferInfo();

        // TODO: Explore design exposing the destination array element
        computeWriteDescriptorSets.push_back(
          vk::WriteDescriptorSet(*this->mDescriptorSet,
                                 i, // Destination binding
                                 0, // Destination array element
                                 1, // Descriptor count
                                 vk::DescriptorType::eStorageBuffer,
                                 nullptr, // Descriptor image info
                                 &descriptorBufferInfo));
    }

    SPDLOG_DEBUG("Kompute Algorithm updating descriptor sets");
    this->mDevice->updateDescriptorSets(computeWriteDescriptorSets, nullptr);

    SPDLOG_DEBUG("Kompue Algorithm successfully run init");
}

void
Algorithm::createShaderModule(std::string shaderFilePath)
{
    SPDLOG_DEBUG("Kompute Algorithm createShaderModule started");

    std::ifstream fileStream(shaderFilePath,
                             std::ios::binary | std::ios::in | std::ios::ate);

    size_t shaderFileSize = fileStream.tellg();
    fileStream.seekg(0, std::ios::beg);
    char* shaderFileData = new char[shaderFileSize];
    fileStream.read(shaderFileData, shaderFileSize);
    fileStream.close();

    vk::ShaderModuleCreateInfo shaderModuleInfo(
      vk::ShaderModuleCreateFlags(), shaderFileSize, (uint32_t*)shaderFileData);

    SPDLOG_DEBUG("Kompute Algorithm Creating shader module. ShaderFileSize: {}",
                 shaderFileSize);
    this->mFreeShaderModule = true;
    this->mShaderModule = std::make_shared<vk::ShaderModule>();
    this->mDevice->createShaderModule(
      &shaderModuleInfo, nullptr, this->mShaderModule.get());

    SPDLOG_DEBUG("Kompute Algorithm create shader module success");
}

void
Algorithm::createPipeline()
{
    SPDLOG_DEBUG("Kompute Algorithm calling create Pipeline");

    // TODO: Explore design for supporting multiple sets
    vk::PipelineLayoutCreateInfo pipelineLayoutInfo(
      vk::PipelineLayoutCreateFlags(),
      1, // Set layout count
      this->mDescriptorSetLayout.get());

    this->mPipelineLayout = std::make_shared<vk::PipelineLayout>();
    this->mDevice->createPipelineLayout(
      &pipelineLayoutInfo, nullptr, this->mPipelineLayout.get());

    vk::PipelineShaderStageCreateInfo shaderStage(
      vk::PipelineShaderStageCreateFlags(),
      vk::ShaderStageFlagBits::eCompute,
      *this->mShaderModule,
      "main",
      nullptr);

    vk::ComputePipelineCreateInfo pipelineInfo(vk::PipelineCreateFlags(),
                                               shaderStage,
                                               *this->mPipelineLayout,
                                               vk::Pipeline(),
                                               0);

    // TODO: Confirm what the best structure is with pipeline cache
    this->mFreePipelineCache = true;
    vk::PipelineCacheCreateInfo pipelineCacheInfo =
      vk::PipelineCacheCreateInfo();
    this->mPipelineCache = std::make_shared<vk::PipelineCache>();
    this->mDevice->createPipelineCache(
      &pipelineCacheInfo, nullptr, this->mPipelineCache.get());

    vk::ResultValue<vk::Pipeline> pipelineResult =
      this->mDevice->createComputePipeline(*this->mPipelineCache, pipelineInfo);

    if (pipelineResult.result != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to create pipeline result: " +
                                 vk::to_string(pipelineResult.result));
    }

    this->mFreePipeline = true;
    this->mPipeline = std::make_shared<vk::Pipeline>(pipelineResult.value);
}

void
Algorithm::recordDispatch(uint32_t x, uint32_t y, uint32_t z)
{
    SPDLOG_DEBUG("Kompute Algorithm calling record dispatch");

    this->mCommandBuffer->bindPipeline(vk::PipelineBindPoint::eCompute,
                                       *this->mPipeline);

    this->mCommandBuffer->bindDescriptorSets(vk::PipelineBindPoint::eCompute,
                                             *this->mPipelineLayout,
                                             0, // First set
                                             *this->mDescriptorSet,
                                             nullptr // Dispatcher
    );

    this->mCommandBuffer->dispatch(x, y, z);
}

}
