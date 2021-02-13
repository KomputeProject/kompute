#pragma once

#include "kompute/Core.hpp"

#include "kompute/Tensor.hpp"

namespace kp {

/**
    Abstraction for compute shaders that are run on top of tensors grouped via
   ParameterGroups (which group descriptorsets)
*/
class Algorithm
{
public:
    // TODO: Move as internal struct of speccontainer
    class SpecializationConstant {
    public:
        SpecializationConstant(const SpecializationConstant& specializationConstant) {
            SPDLOG_DEBUG("Kompute SpecializationConstant copy constructor: {}", *((uint32_t*)specializationConstant.mInstanceData));
            this->mInstanceData = (char*)malloc(sizeof(uint32_t));
            memcpy(this->mInstanceData, specializationConstant.mInstanceData, sizeof(uint32_t));
        }
        // This class is required in absence of std::variant to ensure C++11 support
        SpecializationConstant(uint32_t val) {
            SPDLOG_DEBUG("Kompute SpecializationConstant uint32_t constructor: {}", val);
            this->mInstanceData = (char*)malloc(sizeof(uint32_t));
            memcpy(this->mInstanceData, &val, sizeof(uint32_t));
        }
        SpecializationConstant(float val) {
            SPDLOG_DEBUG("Kompute SpecializationConstant float constructor: {}", val);
            this->mInstanceData = (char*)malloc(sizeof(uint32_t));
            memcpy(this->mInstanceData, &val, sizeof(uint32_t));
        }
        ~SpecializationConstant() {
            free(this->mInstanceData);
        }
        void *data() {
            return this->mInstanceData;
        }
    private:
        // We use char pointer to enable for pointer arithmetic
        char *mInstanceData = nullptr;
    };

    class SpecializationContainer {
    public:
        SpecializationContainer() {
            SPDLOG_DEBUG("Kompute SpecializationContainer default initialiser");
            this->mFreeData = false;
        }

        SpecializationContainer(const SpecializationContainer& specializationContainer)
        {
            SPDLOG_DEBUG("Kompute SpecializationContainer copy constructor, size: {}", specializationContainer.mSpecializationConstants.size());
            SpecializationContainer(specializationContainer.mSpecializationConstants);
        }

        SpecializationContainer(std::vector<SpecializationConstant> instances) {
            SPDLOG_DEBUG("Kompute SpecializationContainer initialiser with instances size {}", instances.size());

            static_assert(sizeof(uint32_t) == sizeof(float) && sizeof(uint32_t) == sizeof(char) * 4,
                    "Kompute requires uint32_t and float to be of same size. Please report this to github.");

            // totalMemorySize depends on instances being set so this needs to be set before
            this->mSpecializationConstants = instances;

            // Data has then to be allocated in order to copy memory into it
            this->mData = (char*)malloc(this->totalMemorySize());

            this->mFreeData = true;

            for (size_t i = 0; i < this->size(); i++) {

                memcpy(this->mData + (i * sizeof(uint32_t)), instances[i].data(), sizeof(uint32_t));
            }
        }

        ~SpecializationContainer() {
            SPDLOG_DEBUG("Kompute SpecializationContainer destructor started");

            this->mSpecializationConstants.clear();

            if (this->mFreeData) {
                SPDLOG_DEBUG("Kompute SpecializationContainer freeing data");
                this->mFreeData = false;
                free(this->mData);
            } else {
                SPDLOG_DEBUG("Kompute SpecializationContainer no data was freed");
            }

            SPDLOG_DEBUG("kompute SpecializationContainer freed data");
        }

        void *transferDataOwnership() {
            SPDLOG_DEBUG("Kompute SpecializationContainer data transfer ownership requested");
            this->mFreeData = false;
            return (void*)this->mData;
        }

        uint32_t size() {
            return this->mSpecializationConstants.size();
        }

        uint32_t totalMemorySize() {
            return this->instanceMemorySize() * this->size();
        }

        uint32_t instanceMemorySize() {
            // At this point only variables accepted are uint32_t and float which are same size
            return sizeof(uint32_t);
        }

    private:

        std::vector<SpecializationConstant> mSpecializationConstants;
        bool mFreeData = false;
        // We use char pointer to enable for pointer arithmetic
        char *mData = nullptr;
    };
private:
    // Private struct template which is then 
public:
    /**
        Base constructor for Algorithm. Should not be used unless explicit
       intended.
    */
    Algorithm();

    /**
     *  Default constructor for Algorithm
     *
     *  @param device The Vulkan device to use for creating resources
     *  @param commandBuffer The vulkan command buffer to bind the pipeline and
     * shaders
     */
    Algorithm(std::shared_ptr<vk::Device> device,
              std::shared_ptr<vk::CommandBuffer> commandBuffer,
              const SpecializationContainer& specializationConstants = {});

    /**
     * Initialiser for the shader data provided to the algorithm as well as
     * tensor parameters that will be used in shader.
     *
     * @param shaderFileData The bytes in spir-v format of the shader
     * @tensorParams The Tensors to be used in the Algorithm / shader for
     * @specalizationInstalces The specialization parameters to pass to the function
     * processing
     */
    void init(const std::vector<char>& shaderFileData,
              std::vector<std::shared_ptr<Tensor>> tensorParams);

    /**
     * Destructor for Algorithm which is responsible for freeing and desroying
     * respective pipelines and owned parameter groups.
     */
    ~Algorithm();

    /**
     * Records the dispatch function with the provided template parameters or
     * alternatively using the size of the tensor by default.
     *
     * @param x Layout X dispatch value
     * @param y Layout Y dispatch value
     * @param z Layout Z dispatch value
     */
    void recordDispatch(uint32_t x = 1, uint32_t y = 1, uint32_t z = 1);

private:
    // -------------- NEVER OWNED RESOURCES
    std::shared_ptr<vk::Device> mDevice;
    std::shared_ptr<vk::CommandBuffer> mCommandBuffer;

    // -------------- OPTIONALLY OWNED RESOURCES
    std::shared_ptr<vk::DescriptorSetLayout> mDescriptorSetLayout;
    bool mFreeDescriptorSetLayout = false;
    std::shared_ptr<vk::DescriptorPool> mDescriptorPool;
    bool mFreeDescriptorPool = false;
    std::shared_ptr<vk::DescriptorSet> mDescriptorSet;
    bool mFreeDescriptorSet = false;
    std::shared_ptr<vk::ShaderModule> mShaderModule;
    bool mFreeShaderModule = false;
    std::shared_ptr<vk::PipelineLayout> mPipelineLayout;
    bool mFreePipelineLayout = false;
    std::shared_ptr<vk::PipelineCache> mPipelineCache;
    bool mFreePipelineCache = false;
    std::shared_ptr<vk::Pipeline> mPipeline;
    bool mFreePipeline = false;

    // -------------- ALWAYS OWNED RESOURCES
    SpecializationContainer mSpecializationConstants;

    // Create util functions
    void createShaderModule(const std::vector<char>& shaderFileData);
    void createPipeline();

    // Parameters
    void createParameters(std::vector<std::shared_ptr<Tensor>>& tensorParams);
    void createDescriptorPool();
};

} // End namespace kp
