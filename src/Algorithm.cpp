#include <fstream>

#include "kompute/Algorithm.hpp"

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
        SPDLOG_ERROR(
          "Kompute Algorithm destructor reached with null Device pointer");
        return;
    }

    if (this->mFreePipeline) {
        SPDLOG_DEBUG("Kompute Algorithm Destroying pipeline");
        if (!this->mPipeline) {
            SPDLOG_ERROR("Kompute Algorithm Error requested to destroy "
                         "pipeline but it is null");
        }
        this->mDevice->destroy(*this->mPipeline);
    }

    if (this->mFreePipelineCache) {
        SPDLOG_DEBUG("Kompute Algorithm Destroying pipeline cache");
        if (!this->mPipelineCache) {
            SPDLOG_ERROR("Kompute Algorithm Error requested to destroy "
                         "pipeline cache but it is null");
        }
        this->mDevice->destroy(*this->mPipelineCache);
    }

    if (this->mFreePipelineLayout) {
        SPDLOG_DEBUG("Kompute Algorithm Destroying pipeline layout");
        if (!this->mPipelineLayout) {
            SPDLOG_ERROR("Kompute Algorithm Error requested to destroy "
                         "pipeline layout but it is null");
        }
        this->mDevice->destroy(*this->mPipelineLayout);
    }

    if (this->mFreeShaderModule) {
        SPDLOG_DEBUG("Kompute Algorithm Destroying shader module");
        if (!this->mShaderModule) {
            SPDLOG_ERROR("Kompute Algorithm Error requested to destroy shader "
                         "module but it is null");
        }
        this->mDevice->destroy(*this->mShaderModule);
    }

    if (this->mFreeDescriptorSet) {
        SPDLOG_DEBUG("Kompute Algorithm Freeing Descriptor Set");
        if (!this->mDescriptorSet) {
            SPDLOG_ERROR(
              "Kompute Algorithm Error requested to free descriptor set");
        }
        this->mDevice->freeDescriptorSets(
          *this->mDescriptorPool, 1, this->mDescriptorSet.get());
    }

    if (this->mFreeDescriptorSetLayout) {
        SPDLOG_DEBUG("Kompute Algorithm Destroying Descriptor Set Layout");
        if (!this->mDescriptorSetLayout) {
            SPDLOG_ERROR("Kompute Algorithm Error requested to destroy "
                         "descriptor set layout but it is null");
        }
        this->mDevice->destroy(*this->mDescriptorSetLayout);
    }

    if (this->mFreeDescriptorPool) {
        SPDLOG_DEBUG("Kompute Algorithm Destroying Descriptor Pool");
        if (!this->mDescriptorPool) {
            SPDLOG_ERROR("Kompute Algorithm Error requested to destroy "
                         "descriptor pool but it is null");
        }
        this->mDevice->destroy(*this->mDescriptorPool);
    }
}

void
Algorithm::init(const std::vector<char>& shaderFileData,
                std::vector<std::shared_ptr<Tensor>> tensorParams)
{
    SPDLOG_DEBUG("Kompute Algorithm init started");

    this->createParameters(tensorParams);
    this->createShaderModule(shaderFileData);

    std::vector<uint32_t> sizes;
    for (std::shared_ptr<Tensor> tensor : tensorParams) {
        sizes.push_back(tensor->size());
    }
    this->createPipeline(sizes);
}

void
Algorithm::createDescriptorPool()
{}

void
Algorithm::createParameters(std::vector<std::shared_ptr<Tensor>>& tensorParams)
{
    SPDLOG_DEBUG("Kompute Algorithm createParameters started");

    std::vector<vk::DescriptorPoolSize> descriptorPoolSizes = {
        vk::DescriptorPoolSize(
          vk::DescriptorType::eStorageBuffer,
          static_cast<uint32_t>(tensorParams.size()) // Descriptor count
          )
    };

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
    this->mDescriptorSetLayout = std::make_shared<vk::DescriptorSetLayout>();
    this->mDevice->createDescriptorSetLayout(
      &descriptorSetLayoutInfo, nullptr, this->mDescriptorSetLayout.get());
    this->mFreeDescriptorSetLayout = true;

    vk::DescriptorSetAllocateInfo descriptorSetAllocateInfo(
      *this->mDescriptorPool,
      1, // Descriptor set layout count
      this->mDescriptorSetLayout.get());

    SPDLOG_DEBUG("Kompute Algorithm allocating descriptor sets");
    this->mDescriptorSet = std::make_shared<vk::DescriptorSet>();
    this->mDevice->allocateDescriptorSets(&descriptorSetAllocateInfo,
                                          this->mDescriptorSet.get());
    this->mFreeDescriptorSet = true;

    SPDLOG_DEBUG("Kompute Algorithm updating descriptor sets");
    for (size_t i = 0; i < tensorParams.size(); i++) {
        std::vector<vk::WriteDescriptorSet> computeWriteDescriptorSets;

        vk::DescriptorBufferInfo descriptorBufferInfo =
          tensorParams[i]->constructDescriptorBufferInfo();

        computeWriteDescriptorSets.push_back(
          vk::WriteDescriptorSet(*this->mDescriptorSet,
                                 i, // Destination binding
                                 0, // Destination array element
                                 1, // Descriptor count
                                 vk::DescriptorType::eStorageBuffer,
                                 nullptr, // Descriptor image info
                                 &descriptorBufferInfo));

        this->mDevice->updateDescriptorSets(computeWriteDescriptorSets,
                                            nullptr);
    }

    SPDLOG_DEBUG("Kompue Algorithm successfully run init");
}

void
Algorithm::createShaderModule(const std::vector<char>& shaderFileData)
{
    SPDLOG_DEBUG("Kompute Algorithm createShaderModule started");

    vk::ShaderModuleCreateInfo shaderModuleInfo(
      vk::ShaderModuleCreateFlags(),
      shaderFileData.size(),
      (uint32_t*)shaderFileData.data());

    SPDLOG_DEBUG("Kompute Algorithm Creating shader module. ShaderFileSize: {}",
                 shaderFileData.size());
    this->mFreeShaderModule = true;
    this->mShaderModule = std::make_shared<vk::ShaderModule>();
    this->mDevice->createShaderModule(
      &shaderModuleInfo, nullptr, this->mShaderModule.get());
    this->mFreeShaderModule = true;

    SPDLOG_DEBUG("Kompute Algorithm create shader module success");
}

void
Algorithm::createPipeline(std::vector<uint32_t> specializationData)
{
    SPDLOG_DEBUG("Kompute Algorithm calling create Pipeline");

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo(
      vk::PipelineLayoutCreateFlags(),
      1, // Set layout count
      this->mDescriptorSetLayout.get());

    this->mPipelineLayout = std::make_shared<vk::PipelineLayout>();
    this->mDevice->createPipelineLayout(
      &pipelineLayoutInfo, nullptr, this->mPipelineLayout.get());
    this->mFreePipelineLayout = true;

    std::vector<vk::SpecializationMapEntry> specializationEntries;

    for (size_t i = 0; i < specializationData.size(); i++) {
        vk::SpecializationMapEntry specializationEntry(
          static_cast<uint32_t>(i),
          static_cast<uint32_t>(sizeof(uint32_t) * i),
          sizeof(uint32_t));

        specializationEntries.push_back(specializationEntry);
    }

    vk::SpecializationInfo specializationInfo(
      static_cast<uint32_t>(specializationEntries.size()),
      specializationEntries.data(),
      sizeof(uint32_t) * specializationEntries.size(),
      specializationData.data());

    vk::PipelineShaderStageCreateInfo shaderStage(
      vk::PipelineShaderStageCreateFlags(),
      vk::ShaderStageFlagBits::eCompute,
      *this->mShaderModule,
      "main",
      &specializationInfo);

    vk::ComputePipelineCreateInfo pipelineInfo(vk::PipelineCreateFlags(),
                                               shaderStage,
                                               *this->mPipelineLayout,
                                               vk::Pipeline(),
                                               0);

    vk::PipelineCacheCreateInfo pipelineCacheInfo =
      vk::PipelineCacheCreateInfo();
    this->mPipelineCache = std::make_shared<vk::PipelineCache>();
    this->mDevice->createPipelineCache(
      &pipelineCacheInfo, nullptr, this->mPipelineCache.get());
    this->mFreePipelineCache = true;

#ifdef KOMPUTE_CREATE_PIPELINE_RESULT_VALUE
    vk::ResultValue<vk::Pipeline> pipelineResult =
      this->mDevice->createComputePipeline(*this->mPipelineCache,
pipelineInfo);

    if (pipelineResult.result != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to create pipeline result: " +
                                 vk::to_string(pipelineResult.result));
    }
#else
    vk::Pipeline pipelineResult =
      this->mDevice->createComputePipeline(*this->mPipelineCache, pipelineInfo);
    this->mFreePipeline = true;
#endif

    this->mFreePipeline = true;
    this->mPipeline = std::make_shared<vk::Pipeline>(pipelineResult);
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
