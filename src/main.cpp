
#include "common.hpp"
#include "vks/context.hpp"
#include "vks/shaders.hpp"
#include "utils.hpp"

#define BUFFER_ELEMENTS 32

#if defined(VK_USE_PLATFORM_ANDROID_KHR)
#define LOG(...) ((void)__android_log_print(ANDROID_LOG_INFO, "vulkanExample", __VA_ARGS__))
#else
#define LOG(...) printf(__VA_ARGS__)
#endif

class VulkanExample {
public:
    vks::Context context;
    vk::Device& device{ context.device };
    vk::Queue queue;
    vk::CommandPool commandPool;
    vk::CommandBuffer commandBuffer;
    vk::DescriptorPool descriptorPool;
    vk::DescriptorSetLayout descriptorSetLayout;
    vk::DescriptorSet descriptorSet;
    vk::PipelineLayout pipelineLayout;
    vk::Pipeline pipeline;
    vk::ShaderModule shaderModule;
    /*
    Prepare storage buffers
    */
    std::vector<uint32_t> computeInput;
    std::vector<uint32_t> computeOutput;
    vks::Buffer deviceBuffer, hostBuffer;
    size_t bufferSize = sizeof(uint32_t) * BUFFER_ELEMENTS;

    VulkanExample() {}

    void prepare() {
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
        LOG("loading vulkan lib");
        vks::android::loadVulkanLibrary();
#endif
        context.createInstance();
        context.createDevice();
        LOG("GPU: %s\n", context.deviceProperties.deviceName);

        // Get a compute queue
        queue = context.device.getQueue(context.queueIndices.compute, 0);

        // Compute command pool

        vk::CommandPoolCreateInfo cmdPoolInfo = {};
        cmdPoolInfo.queueFamilyIndex = context.queueIndices.compute;
        cmdPoolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
        commandPool = device.createCommandPool(cmdPoolInfo);

        setupBuffers();
        setupDescriptors();
        setupPipeline();
        setupCommandBuffer();
    }

    void setupBuffers() {
        computeInput.resize(BUFFER_ELEMENTS);
        computeOutput.resize(BUFFER_ELEMENTS);
        // Fill input data
        uint32_t n = 0;
        std::generate(computeInput.begin(), computeInput.end(), [&n] { return n++; });
        // Copy input data to VRAM using a staging buffer
        deviceBuffer = context.stageToDeviceBuffer<uint32_t>(vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferSrc, computeInput);
        hostBuffer = context.createBuffer(vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eHostVisible, bufferSize);
    }

    void setupDescriptors() {
        std::vector<vk::DescriptorPoolSize> poolSizes = {
            vk::DescriptorPoolSize{ vk::DescriptorType::eStorageBuffer, 1 },
        };

        descriptorPool = device.createDescriptorPool(vk::DescriptorPoolCreateInfo{ {}, 1, static_cast<uint32_t>(poolSizes.size()), poolSizes.data() });

        std::vector<vk::DescriptorSetLayoutBinding> setLayoutBindings {
            vk::DescriptorSetLayoutBinding{ 0, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eCompute },
        };
        descriptorSetLayout = device.createDescriptorSetLayout({ {}, 1, setLayoutBindings.data() });

        pipelineLayout = device.createPipelineLayout(vk::PipelineLayoutCreateInfo{ {}, 1, &descriptorSetLayout });

        descriptorSet = device.allocateDescriptorSets({ descriptorPool, 1, &descriptorSetLayout })[0];

        vk::DescriptorBufferInfo bufferDescriptor{ deviceBuffer.buffer, 0, VK_WHOLE_SIZE };
        std::vector<vk::WriteDescriptorSet> computeWriteDescriptorSets = {
            vk::WriteDescriptorSet{ descriptorSet, 0, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr, &bufferDescriptor },
        };
        device.updateDescriptorSets(computeWriteDescriptorSets, nullptr);
    }

    void setupPipeline() {
        // Create pipeline
        vk::ComputePipelineCreateInfo computePipelineCreateInfo;
        computePipelineCreateInfo.layout = pipelineLayout;
        computePipelineCreateInfo.stage =
            vks::shaders::loadShader(context.device, vkx::getAssetPath() + "shaders/computeheadless/headless.comp.spv", vk::ShaderStageFlagBits::eCompute);

        // Pass SSBO size via specialization constant
        struct SpecializationData {
            uint32_t BUFFER_ELEMENT_COUNT = BUFFER_ELEMENTS;
        } specializationData;
        vk::SpecializationMapEntry specializationMapEntry{ 0, 0, sizeof(uint32_t) };
        vk::SpecializationInfo specializationInfo{ 1, &specializationMapEntry, sizeof(SpecializationData), &specializationData };
        computePipelineCreateInfo.stage.pSpecializationInfo = &specializationInfo;
        vk::ResultValue<vk::Pipeline> result = device.createComputePipeline(context.pipelineCache, computePipelineCreateInfo);
        pipeline = result.value;
        device.destroyShaderModule(computePipelineCreateInfo.stage.module);
    }

    void setupCommandBuffer() {
        // Create a command buffer for compute operations
        commandBuffer = device.allocateCommandBuffers({ commandPool, vk::CommandBufferLevel::ePrimary, 1 })[0];
        commandBuffer.begin(vk::CommandBufferBeginInfo{});
        // Barrier to ensure that input buffer transfer is finished before compute shader reads from it
        vk::BufferMemoryBarrier bufferBarrier;
        bufferBarrier.buffer = deviceBuffer.buffer;
        bufferBarrier.size = VK_WHOLE_SIZE;
        bufferBarrier.srcAccessMask = vk::AccessFlagBits::eHostWrite;
        bufferBarrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
        bufferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        bufferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

        commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eHost, vk::PipelineStageFlagBits::eComputeShader, {}, nullptr, bufferBarrier, nullptr);
        commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, pipeline);
        commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, pipelineLayout, 0, descriptorSet, nullptr);
        commandBuffer.dispatch(BUFFER_ELEMENTS, 1, 1);

        // Barrier to ensure that shader writes are finished before buffer is read back from GPU
        bufferBarrier.srcAccessMask = vk::AccessFlagBits::eShaderWrite;
        bufferBarrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;
        commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eComputeShader, vk::PipelineStageFlagBits::eTransfer, {}, nullptr, bufferBarrier, nullptr);

        // Read back to host visible buffer
        vk::BufferCopy copyRegion{ 0, 0, bufferSize };
        commandBuffer.copyBuffer(deviceBuffer.buffer, hostBuffer.buffer, copyRegion);

        // Barrier to ensure that buffer copy is finished before host reading from it
        bufferBarrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        bufferBarrier.dstAccessMask = vk::AccessFlagBits::eHostRead;
        bufferBarrier.buffer = hostBuffer.buffer;
        commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eHost, {}, nullptr, bufferBarrier, nullptr);
        commandBuffer.end();
    }

    void run() {
        LOG("Running headless compute example\n");
        prepare();

        {
            // Fence for compute CB sync
            vk::Fence fence = device.createFence(vk::FenceCreateInfo{});

            // Submit compute work
            const vk::PipelineStageFlags waitStageMask = vk::PipelineStageFlagBits::eTransfer;
            vk::SubmitInfo computeSubmitInfo;
            computeSubmitInfo.pWaitDstStageMask = &waitStageMask;
            computeSubmitInfo.commandBufferCount = 1;
            computeSubmitInfo.pCommandBuffers = &commandBuffer;
            queue.submit(computeSubmitInfo, fence);
            device.waitForFences(fence, VK_TRUE, UINT64_MAX);
            device.destroy(fence);
        }

        {
            // Make device writes visible to the host
            hostBuffer.map();
            hostBuffer.invalidate();
            // Copy to output
            memcpy(computeOutput.data(), hostBuffer.mapped, bufferSize);
            hostBuffer.unmap();
        }

        // Output buffer contents
        LOG("Compute input:\n");
        for (auto v : computeInput) {
            LOG("%d \t", v);
        }
        std::cout << std::endl;

        LOG("Compute output:\n");
        for (auto v : computeOutput) {
            LOG("%d \t", v);
        }
        std::cout << std::endl;
    }

    ~VulkanExample() {
        deviceBuffer.destroy();
        hostBuffer.destroy();
        device.destroy(pipelineLayout);
        device.destroy(descriptorSetLayout);
        device.destroy(descriptorPool);
        device.destroy(pipeline);
        device.destroy(commandPool);
        context.destroy();
        std::cout << "Finished. Press enter to terminate...";
        getchar();
    }
};

int main(const int argc, const char* argv[]) {
    VulkanExample().run();
    return 0;
}
