#include <fstream>

#include "kompute/Algorithm.hpp"

namespace kp {

Algorithm::Algorithm(std::shared_ptr<vk::Device> device,
                     const std::vector<std::shared_ptr<Tensor>>& tensors,
                     const std::vector<uint32_t>& spirv,
                     const Workgroup& workgroup,
                     const Constants& specializationConstants,
                     const Constants& pushConstants)
{
    KP_LOG_DEBUG("Kompute Algorithm Constructor with device");

    this->mDevice = device;

    if (tensors.size() && spirv.size()) {
        KP_LOG_INFO("Kompute Algorithm initialising with tensor size: {} and "
                    "spirv size: {}",
                    tensors.size(),
                    spirv.size());
        this->rebuild(
          tensors, spirv, workgroup, specializationConstants, pushConstants);
    } else {
        KP_LOG_INFO("Kompute Algorithm constructor with empty tensors and or "
                    "spirv so not rebuilding vulkan components");
    }
}

Algorithm::~Algorithm()
{
    KP_LOG_DEBUG("Kompute Algorithm Destructor started");

    this->destroy();
}

void
Algorithm::rebuild(const std::vector<std::shared_ptr<Tensor>>& tensors,
                   const std::vector<uint32_t>& spirv,
                   const Workgroup& workgroup,
                   const Constants& specializationConstants,
                   const Constants& pushConstants)
{
    KP_LOG_DEBUG("Kompute Algorithm rebuild started");

    this->mTensors = tensors;
    this->mSpirv = spirv;
    this->mSpecializationConstants = specializationConstants;
    this->mPushConstants = pushConstants;
    this->setWorkgroup(workgroup,
                       this->mTensors.size() ? this->mTensors[0]->size() : 1);

    // Descriptor pool is created first so if available then destroy all before
    // rebuild
    if (this->isInit()) {
        this->destroy();
    }

    this->createParameters();
    this->createShaderModule();
    this->createPipeline();
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
    KP_LOG_DEBUG("Kompute Algorithm createParameters started");

    std::vector<vk::DescriptorPoolSize> descriptorPoolSizes = {
        vk::DescriptorPoolSize(
          vk::DescriptorType::eStorageBuffer,
          static_cast<uint32_t>(this->mTensors.size()) // Descriptor count
          )
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
    for (size_t i = 0; i < this->mTensors.size(); i++) {
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
    for (size_t i = 0; i < this->mTensors.size(); i++) {
        std::vector<vk::WriteDescriptorSet> computeWriteDescriptorSets;

        vk::DescriptorBufferInfo descriptorBufferInfo =
          this->mTensors[i]->constructDescriptorBufferInfo();

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

    KP_LOG_DEBUG("Kompue Algorithm successfully run init");
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
    if (this->mPushConstants.size()) {
        pushConstantRange.setStageFlags(vk::ShaderStageFlagBits::eCompute);
        pushConstantRange.setOffset(0);
        pushConstantRange.setSize(sizeof(float) * this->mPushConstants.size());

        pipelineLayoutInfo.setPushConstantRangeCount(1);
        pipelineLayoutInfo.setPPushConstantRanges(&pushConstantRange);
    }

    this->mPipelineLayout = std::make_shared<vk::PipelineLayout>();
    this->mDevice->createPipelineLayout(
      &pipelineLayoutInfo, nullptr, this->mPipelineLayout.get());
    this->mFreePipelineLayout = true;

    std::vector<vk::SpecializationMapEntry> specializationEntries;

    for (uint32_t i = 0; i < this->mSpecializationConstants.size(); i++) {
        vk::SpecializationMapEntry specializationEntry(
          static_cast<uint32_t>(i),
          static_cast<uint32_t>(sizeof(float) * i),
          sizeof(float));

        specializationEntries.push_back(specializationEntry);
    }

    // This passes ownership of the memory so we remove ownership from
    // specialization container by using "transferDataOwnership"
    vk::SpecializationInfo specializationInfo(
      static_cast<uint32_t>(specializationEntries.size()),
      specializationEntries.data(),
      sizeof(float) * this->mSpecializationConstants.size(),
      this->mSpecializationConstants.data());

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
      this->mDevice->createComputePipeline(*this->mPipelineCache, pipelineInfo);
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
    if (this->mPushConstants.size()) {
        KP_LOG_DEBUG("Kompute Algorithm binding push constants size: {}",
                     this->mPushConstants.size());

        commandBuffer.pushConstants(*this->mPipelineLayout,
                                    vk::ShaderStageFlagBits::eCompute,
                                    0,
                                    this->mPushConstants.size() * sizeof(float),
                                    this->mPushConstants.data());
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

void
Algorithm::setPush(const Constants& pushConstants)
{

    if (pushConstants.size() != this->mPushConstants.size()) {
        throw std::runtime_error(
          fmt::format("Kompute Algorithm push "
                      "constant provided is size {} but expected size {}",
                      pushConstants.size(),
                      this->mPushConstants.size()));
    }

    this->mPushConstants = pushConstants;
}

const Workgroup&
Algorithm::getWorkgroup()
{
    return this->mWorkgroup;
}

const Constants&
Algorithm::getSpecializationConstants()
{
    return this->mSpecializationConstants;
}

const Constants&
Algorithm::getPush()
{
    return this->mPushConstants;
}

const std::vector<std::shared_ptr<Tensor>>&
Algorithm::getTensors()
{
    return this->mTensors;
}

}
