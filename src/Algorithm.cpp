// SPDX-License-Identifier: Apache-2.0
#include <fstream>

#include "kompute/Algorithm.hpp"
#include "kompute/Image.hpp"

namespace kp {

Algorithm::~Algorithm() noexcept
{
    KP_LOG_DEBUG("Kompute Algorithm Destructor started");

    this->destroy();
}

bool
Algorithm::isInit()
{
    return this->mPipeline && this->mPipelineCache && this->mPipelineLayout &&
           this->mDescriptorPool && this->mDescriptorSet &&
           this->mDescriptorSetLayout && this->mShaderModule;
}

void
Algorithm::destroy()
{
    // We don't have to free memory on destroy as it's freed by the
    // commandBuffer destructor if (this->mPushConstantsData) {
    //     free(this->mPushConstantsData);
    // }
    // if (this->mSpecializationConstantsData) {
    //     free(this->mSpecializationConstantsData);
    // }

    if (!this->mDevice) {
        KP_LOG_WARN("Kompute Algorithm destroy function reached with null "
                    "Device pointer");
        return;
    }

    if (this->mFreePipeline && this->mPipeline) {
        KP_LOG_DEBUG("Kompute Algorithm Destroying pipeline");
        if (!this->mPipeline) {
            KP_LOG_WARN("Kompute Algorithm Error requested to destroy "
                        "pipeline but it is null");
        }
        this->mDevice->destroy(
          *this->mPipeline,
          (vk::Optional<const vk::AllocationCallbacks>)nullptr);
        this->mPipeline = nullptr;
    }

    if (this->mFreePipelineCache && this->mPipelineCache) {
        KP_LOG_DEBUG("Kompute Algorithm Destroying pipeline cache");
        if (!this->mPipelineCache) {
            KP_LOG_WARN("Kompute Algorithm Error requested to destroy "
                        "pipeline cache but it is null");
        }
        this->mDevice->destroy(
          *this->mPipelineCache,
          (vk::Optional<const vk::AllocationCallbacks>)nullptr);
        this->mPipelineCache = nullptr;
    }

    if (this->mFreePipelineLayout && this->mPipelineLayout) {
        KP_LOG_DEBUG("Kompute Algorithm Destroying pipeline layout");
        if (!this->mPipelineLayout) {
            KP_LOG_WARN("Kompute Algorithm Error requested to destroy "
                        "pipeline layout but it is null");
        }
        this->mDevice->destroy(
          *this->mPipelineLayout,
          (vk::Optional<const vk::AllocationCallbacks>)nullptr);
        this->mPipelineLayout = nullptr;
    }

    if (this->mFreeShaderModule && this->mShaderModule) {
        KP_LOG_DEBUG("Kompute Algorithm Destroying shader module");
        if (!this->mShaderModule) {
            KP_LOG_WARN("Kompute Algorithm Error requested to destroy shader "
                        "module but it is null");
        }
        this->mDevice->destroy(
          *this->mShaderModule,
          (vk::Optional<const vk::AllocationCallbacks>)nullptr);
        this->mShaderModule = nullptr;
    }

    // We don't call freeDescriptorSet as the descriptor pool is not created
    // with VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT more at
    // (https://www.khronos.org/registry/vulkan/specs/1.0/html/vkspec.html#VUID-vkFreeDescriptorSets-descriptorPool-00312))
    // if (this->mFreeDescriptorSet && this->mDescriptorSet) {
    //    KP_LOG_DEBUG("Kompute Algorithm Freeing Descriptor Set");
    //    if (!this->mDescriptorSet) {
    //        KP_LOG_WARN(
    //          "Kompute Algorithm Error requested to free descriptor set");
    //    }
    //    this->mDevice->freeDescriptorSets(
    //      *this->mDescriptorPool, 1, this->mDescriptorSet.get());
    //    this->mDescriptorSet = nullptr;
    //}

    if (this->mFreeDescriptorSetLayout && this->mDescriptorSetLayout) {
        KP_LOG_DEBUG("Kompute Algorithm Destroying Descriptor Set Layout");
        if (!this->mDescriptorSetLayout) {
            KP_LOG_WARN("Kompute Algorithm Error requested to destroy "
                        "descriptor set layout but it is null");
        }
        this->mDevice->destroy(
          *this->mDescriptorSetLayout,
          (vk::Optional<const vk::AllocationCallbacks>)nullptr);
        this->mDescriptorSetLayout = nullptr;
    }

    if (this->mFreeDescriptorPool && this->mDescriptorPool) {
        KP_LOG_DEBUG("Kompute Algorithm Destroying Descriptor Pool");
        if (!this->mDescriptorPool) {
            KP_LOG_WARN("Kompute Algorithm Error requested to destroy "
                        "descriptor pool but it is null");
        }
        this->mDevice->destroy(
          *this->mDescriptorPool,
          (vk::Optional<const vk::AllocationCallbacks>)nullptr);
        this->mDescriptorPool = nullptr;
    }
}

void
Algorithm::createParameters()
{
    uint32_t numImages = 0;
    uint32_t numTensors = 0;

    KP_LOG_DEBUG("Kompute Algorithm createParameters started");

    for (const std::shared_ptr<Memory>& mem : this->mMemObjects) {
        if (mem->type() == Memory::Type::eImage) {
            numImages++;
        } else {
            numTensors++;
        }
    }

    std::vector<vk::DescriptorPoolSize> descriptorPoolSizes;

    if (numTensors > 0) {
        descriptorPoolSizes.push_back(vk::DescriptorPoolSize(
          vk::DescriptorType::eStorageBuffer,
          static_cast<uint32_t>(numTensors) // Descriptor count
          ));
    }

    if (numImages > 0) {
        descriptorPoolSizes.push_back(vk::DescriptorPoolSize(
          vk::DescriptorType::eStorageImage,
          static_cast<uint32_t>(numImages) // Descriptor count
          ));
    };

    vk::DescriptorPoolCreateInfo descriptorPoolInfo(
      vk::DescriptorPoolCreateFlags(),
      1, // Max sets
      static_cast<uint32_t>(descriptorPoolSizes.size()),
      descriptorPoolSizes.data());

    KP_LOG_DEBUG("Kompute Algorithm creating descriptor pool");
    this->mDescriptorPool = std::make_shared<vk::DescriptorPool>();
    this->mDevice->createDescriptorPool(
      &descriptorPoolInfo, nullptr, this->mDescriptorPool.get());
    this->mFreeDescriptorPool = true;

    std::vector<vk::DescriptorSetLayoutBinding> descriptorSetBindings;
    for (size_t i = 0; i < this->mMemObjects.size(); i++) {
        descriptorSetBindings.push_back(
          vk::DescriptorSetLayoutBinding(i, // Binding index
                                         mMemObjects[i]->getDescriptorType(),
                                         1, // Descriptor count
                                         vk::ShaderStageFlagBits::eCompute));
    }

    // This is the component that is fed into the pipeline
    vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutInfo(
      vk::DescriptorSetLayoutCreateFlags(),
      static_cast<uint32_t>(descriptorSetBindings.size()),
      descriptorSetBindings.data());

    KP_LOG_DEBUG("Kompute Algorithm creating descriptor set layout");
    this->mDescriptorSetLayout = std::make_shared<vk::DescriptorSetLayout>();
    this->mDevice->createDescriptorSetLayout(
      &descriptorSetLayoutInfo, nullptr, this->mDescriptorSetLayout.get());
    this->mFreeDescriptorSetLayout = true;

    vk::DescriptorSetAllocateInfo descriptorSetAllocateInfo(
      *this->mDescriptorPool,
      1, // Descriptor set layout count
      this->mDescriptorSetLayout.get());

    KP_LOG_DEBUG("Kompute Algorithm allocating descriptor sets");
    this->mDescriptorSet = std::make_shared<vk::DescriptorSet>();
    this->mDevice->allocateDescriptorSets(&descriptorSetAllocateInfo,
                                          this->mDescriptorSet.get());
    this->mFreeDescriptorSet = true;

    KP_LOG_DEBUG("Kompute Algorithm updating descriptor sets");
    for (size_t i = 0; i < this->mMemObjects.size(); i++) {
        std::vector<vk::WriteDescriptorSet> computeWriteDescriptorSets;

        vk::WriteDescriptorSet descriptorSet =
          this->mMemObjects[i]->constructDescriptorSet(*this->mDescriptorSet,
                                                       i);

        computeWriteDescriptorSets.push_back(descriptorSet);

        this->mDevice->updateDescriptorSets(computeWriteDescriptorSets,
                                            nullptr);
    }

    KP_LOG_DEBUG("Kompute Algorithm successfully run init");
}

void
Algorithm::createShaderModule()
{
    KP_LOG_DEBUG("Kompute Algorithm createShaderModule started");

    vk::ShaderModuleCreateInfo shaderModuleInfo(vk::ShaderModuleCreateFlags(),
                                                sizeof(uint32_t) *
                                                  this->mSpirv.size(),
                                                this->mSpirv.data());

    KP_LOG_DEBUG("Kompute Algorithm Creating shader module. ShaderFileSize: {}",
                 this->mSpirv.size());
    this->mFreeShaderModule = true;
    this->mShaderModule = std::make_shared<vk::ShaderModule>();
    this->mDevice->createShaderModule(
      &shaderModuleInfo, nullptr, this->mShaderModule.get());
    this->mFreeShaderModule = true;

    KP_LOG_DEBUG("Kompute Algorithm create shader module success");
}

void
Algorithm::createPipeline()
{
    KP_LOG_DEBUG("Kompute Algorithm calling create Pipeline");

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo(
      vk::PipelineLayoutCreateFlags(),
      1, // Set layout count
      this->mDescriptorSetLayout.get());

    vk::PushConstantRange pushConstantRange;
    if (this->mPushConstantsSize) {
        pushConstantRange.setStageFlags(vk::ShaderStageFlagBits::eCompute);
        pushConstantRange.setOffset(0);
        pushConstantRange.setSize(this->mPushConstantsDataTypeMemorySize *
                                  this->mPushConstantsSize);

        pipelineLayoutInfo.setPushConstantRangeCount(1);
        pipelineLayoutInfo.setPPushConstantRanges(&pushConstantRange);
    }

    this->mPipelineLayout = std::make_shared<vk::PipelineLayout>();
    this->mDevice->createPipelineLayout(
      &pipelineLayoutInfo, nullptr, this->mPipelineLayout.get());
    this->mFreePipelineLayout = true;

    std::vector<vk::SpecializationMapEntry> specializationEntries;

    for (uint32_t i = 0; i < this->mSpecializationConstantsSize; i++) {
        vk::SpecializationMapEntry specializationEntry(
          static_cast<uint32_t>(i),
          static_cast<uint32_t>(
            this->mSpecializationConstantsDataTypeMemorySize * i),
          this->mSpecializationConstantsDataTypeMemorySize);

        specializationEntries.push_back(specializationEntry);
    }

    // This passes ownership of the memory so we remove ownership from
    // specialization container by using "transferDataOwnership"
    vk::SpecializationInfo specializationInfo(
      static_cast<uint32_t>(specializationEntries.size()),
      specializationEntries.data(),
      this->mSpecializationConstantsDataTypeMemorySize *
        this->mSpecializationConstantsSize,
      this->mSpecializationConstantsData);

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
      this->mDevice->createComputePipeline(*this->mPipelineCache, pipelineInfo);

    if (pipelineResult.result != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to create pipeline result: " +
                                 vk::to_string(pipelineResult.result));
    }

    vk::Pipeline& pipeline = pipelineResult.value;
    this->mPipeline = std::make_shared<vk::Pipeline>(pipeline);
    this->mFreePipeline = true;
#else
    vk::Pipeline pipeline =
      this->mDevice->createComputePipeline(*this->mPipelineCache, pipelineInfo)
        .value;
    this->mPipeline = std::make_shared<vk::Pipeline>(pipeline);
    this->mFreePipeline = true;
#endif

    // TODO: Update to consistent
    // this->mPipeline = std::make_shared<vk::Pipeline>();
    // this->mDevice->createComputePipelines(
    //         *this->mPipelineCache, 1, &pipelineInfo, nullptr,
    //         this->mPipeline.get());

    KP_LOG_DEBUG("Kompute Algorithm Create Pipeline Success");
}

void
Algorithm::recordBindCore(const vk::CommandBuffer& commandBuffer)
{
    KP_LOG_DEBUG("Kompute Algorithm binding pipeline");

    commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute,
                               *this->mPipeline);

    KP_LOG_DEBUG("Kompute Algorithm binding descriptor sets");

    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute,
                                     *this->mPipelineLayout,
                                     0, // First set
                                     *this->mDescriptorSet,
                                     nullptr // Dispatcher
    );
}

void
Algorithm::recordBindPush(const vk::CommandBuffer& commandBuffer)
{
    if (this->mPushConstantsSize) {
        KP_LOG_DEBUG("Kompute Algorithm binding push constants memory size: {}",
                     this->mPushConstantsSize *
                       this->mPushConstantsDataTypeMemorySize);

        commandBuffer.pushConstants(*this->mPipelineLayout,
                                    vk::ShaderStageFlagBits::eCompute,
                                    0,
                                    this->mPushConstantsSize *
                                      this->mPushConstantsDataTypeMemorySize,
                                    this->mPushConstantsData);
    }
}

void
Algorithm::recordDispatch(const vk::CommandBuffer& commandBuffer)
{
    KP_LOG_DEBUG("Kompute Algorithm recording dispatch");

    commandBuffer.dispatch(
      this->mWorkgroup[0], this->mWorkgroup[1], this->mWorkgroup[2]);
}

void
Algorithm::setWorkgroup(const Workgroup& workgroup, uint32_t minSize)
{

    KP_LOG_INFO("Kompute OpAlgoCreate setting dispatch size");

    // The dispatch size is set up based on either explicitly provided template
    // parameters or by default it would take the shape and size of the tensors
    if (workgroup[0] > 0) {
        // If at least the x value is provided we use mainly the parameters
        // provided
        this->mWorkgroup = { workgroup[0],
                             workgroup[1] > 0 ? workgroup[1] : 1,
                             workgroup[2] > 0 ? workgroup[2] : 1 };
    } else {
        this->mWorkgroup = { minSize, 1, 1 };
    }

    KP_LOG_INFO("Kompute OpAlgoCreate set dispatch size X: {}, Y: {}, Z: {}",
                this->mWorkgroup[0],
                this->mWorkgroup[1],
                this->mWorkgroup[2]);
}

const Workgroup&
Algorithm::getWorkgroup()
{
    return this->mWorkgroup;
}

const std::vector<std::shared_ptr<Memory>>&
Algorithm::getMemObjects()
{
    return this->mMemObjects;
}

}
