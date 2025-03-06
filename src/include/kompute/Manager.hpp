// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "kompute/Core.hpp"

#include "kompute/Image.hpp"
#include "kompute/Sequence.hpp"
#include "logger/Logger.hpp"

#define KP_DEFAULT_SESSION "DEFAULT"

namespace kp {

/**
    Base orchestrator which creates and manages device and child components
*/
class Manager
{
  public:
    /**
        Base constructor and default used which creates the base resources
       including choosing the device 0 by default.
    */
    Manager();

    /**
     * Similar to base constructor but allows for further configuration to use
     * when creating the Vulkan resources.
     *
     * @param physicalDeviceIndex The index of the physical device to use
     * @param familyQueueIndices (Optional) List of queue indices to add for
     * explicit allocation
     * @param desiredExtensions The desired extensions to load from
     * physicalDevice
     */
    Manager(uint32_t physicalDeviceIndex,
            const std::vector<uint32_t>& familyQueueIndices = {},
            const std::vector<std::string>& desiredExtensions = {});

    /**
     * Manager constructor which allows your own vulkan application to integrate
     * with the kompute use.
     *
     * @param instance Vulkan compute instance to base this application
     * @param physicalDevice Vulkan physical device to use for application
     * @param device Vulkan logical device to use for all base resources
     * @param physicalDeviceIndex Index for vulkan physical device used
     */
    Manager(std::shared_ptr<vk::Instance> instance,
            std::shared_ptr<vk::PhysicalDevice> physicalDevice,
            std::shared_ptr<vk::Device> device);


    /**
     * @brief Make Manager uncopyable
     *
     */
    Manager(const Manager&) = delete;
    Manager(const Manager&&) = delete;
    Manager& operator=(const Manager&) = delete;
    Manager& operator=(const Manager&&) = delete;

    /**
     * Manager destructor which would ensure all owned resources are destroyed
     * unless explicitly stated that resources should not be destroyed or freed.
     */
    ~Manager();

    /**
     * Create a managed sequence that will be destroyed by this manager
     * if it hasn't been destroyed by its reference count going to zero.
     *
     * @param queueIndex The queue to use from the available queues
     * @param nrOfTimestamps The maximum number of timestamps to allocate.
     * If zero (default), disables latching of timestamps.
     * @returns Shared pointer with initialised sequence
     */
    std::shared_ptr<Sequence> sequence(uint32_t queueIndex = 0,
                                       uint32_t totalTimestamps = 0);

    /**
     * Create a managed tensor that will be destroyed by this manager
     * if it hasn't been destroyed by its reference count going to zero.
     *
     * @param data The data to initialize the tensor with
     * @param tensorType The type of tensor to initialize
     * @returns Shared pointer with initialised tensor
     */
    template<typename T>
    std::shared_ptr<TensorT<T>> tensorT(
      const std::vector<T>& data,
      Memory::MemoryTypes tensorType = Memory::MemoryTypes::eDevice)
    {
        KP_LOG_DEBUG("Kompute Manager tensor creation triggered");

        std::shared_ptr<TensorT<T>> tensor{ new kp::TensorT<T>(
          this->mPhysicalDevice, this->mDevice, data, tensorType) };

        if (this->mManageResources) {
            this->mManagedMemObjects.push_back(tensor);
        }

        return tensor;
    }

    /**
     * Create a managed tensor that will be destroyed by this manager
     * if it hasn't been destroyed by its reference count going to zero.
     *
     * @param size The number of element in this tensor
     * @param tensorType The type of tensor to initialize
     * @returns Shared pointer with initialised tensor
     */
    template<typename T>
    std::shared_ptr<TensorT<T>> tensorT(
      size_t size,
      Memory::MemoryTypes tensorType = Memory::MemoryTypes::eDevice)
    {
        KP_LOG_DEBUG("Kompute Manager tensor creation triggered");

        std::shared_ptr<TensorT<T>> tensor{ new kp::TensorT<T>(
          this->mPhysicalDevice, this->mDevice, size, tensorType) };

        if (this->mManageResources) {
            this->mManagedMemObjects.push_back(tensor);
        }

        return tensor;
    }
    std::shared_ptr<TensorT<float>> tensor(
      const std::vector<float>& data,
      Memory::MemoryTypes tensorType = Memory::MemoryTypes::eDevice)
    {
        return this->tensorT<float>(data, tensorType);
    }

    std::shared_ptr<Tensor> tensor(
      void* data,
      uint32_t elementTotalCount,
      uint32_t elementMemorySize,
      const Memory::DataTypes& dataType,
      Memory::MemoryTypes tensorType = Memory::MemoryTypes::eDevice)
    {
        std::shared_ptr<Tensor> tensor{ new kp::Tensor(this->mPhysicalDevice,
                                                       this->mDevice,
                                                       data,
                                                       elementTotalCount,
                                                       elementMemorySize,
                                                       dataType,
                                                       tensorType) };

        if (this->mManageResources) {
            this->mManagedMemObjects.push_back(tensor);
        }

        return tensor;
    }

    std::shared_ptr<Tensor> tensor(
      uint32_t elementTotalCount,
      uint32_t elementMemorySize,
      const Memory::DataTypes& dataType,
      Memory::MemoryTypes tensorType = Memory::MemoryTypes::eDevice)
    {
        std::shared_ptr<Tensor> tensor{ new kp::Tensor(this->mPhysicalDevice,
                                                       this->mDevice,
                                                       elementTotalCount,
                                                       elementMemorySize,
                                                       dataType,
                                                       tensorType) };

        if (this->mManageResources) {
            this->mManagedMemObjects.push_back(tensor);
        }

        return tensor;
    }

    /**
     * Create a managed image that will be destroyed by this manager
     * if it hasn't been destroyed by its reference count going to zero.
     *
     * @param data The data to initialize the image with
     * @param tensorType The type of image to initialize
     * @returns Shared pointer with initialised image
     */
    template<typename T>
    std::shared_ptr<ImageT<T>> imageT(
      const std::vector<T>& data,
      uint32_t width,
      uint32_t height,
      uint32_t numChannels,
      vk::ImageTiling tiling,
      Memory::MemoryTypes imageType = Memory::MemoryTypes::eDevice)
    {
        KP_LOG_DEBUG("Kompute Manager image creation triggered");

        std::shared_ptr<ImageT<T>> image{ new kp::ImageT<T>(
          this->mPhysicalDevice,
          this->mDevice,
          data,
          width,
          height,
          numChannels,
          tiling,
          imageType) };

        if (this->mManageResources) {
            this->mManagedMemObjects.push_back(image);
        }

        return image;
    }

    template<typename T>
    std::shared_ptr<ImageT<T>> imageT(
      const std::vector<T>& data,
      uint32_t width,
      uint32_t height,
      uint32_t numChannels,
      Memory::MemoryTypes imageType = Memory::MemoryTypes::eDevice)
    {
        KP_LOG_DEBUG("Kompute Manager image creation triggered");

        std::shared_ptr<ImageT<T>> image{ new kp::ImageT<T>(
          this->mPhysicalDevice,
          this->mDevice,
          data,
          width,
          height,
          numChannels,
          imageType) };

        if (this->mManageResources) {
            this->mManagedMemObjects.push_back(image);
        }

        return image;
    }

    template<typename T>
    std::shared_ptr<ImageT<T>> imageT(
      uint32_t width,
      uint32_t height,
      uint32_t numChannels,
      vk::ImageTiling tiling,
      Memory::MemoryTypes imageType = Memory::MemoryTypes::eDevice)
    {
        KP_LOG_DEBUG("Kompute Manager image creation triggered");

        std::shared_ptr<ImageT<T>> image{ new kp::ImageT<T>(
          this->mPhysicalDevice,
          this->mDevice,
          width,
          height,
          numChannels,
          tiling,
          imageType) };

        if (this->mManageResources) {
            this->mManagedMemObjects.push_back(image);
        }

        return image;
    }

    template<typename T>
    std::shared_ptr<ImageT<T>> imageT(
      uint32_t width,
      uint32_t height,
      uint32_t numChannels,
      Memory::MemoryTypes imageType = Memory::MemoryTypes::eDevice)
    {
        KP_LOG_DEBUG("Kompute Manager image creation triggered");

        std::shared_ptr<ImageT<T>> image{ new kp::ImageT<T>(
          this->mPhysicalDevice,
          this->mDevice,
          width,
          height,
          numChannels,
          imageType) };

        if (this->mManageResources) {
            this->mManagedMemObjects.push_back(image);
        }

        return image;
    }
    std::shared_ptr<ImageT<float>> image(
      const std::vector<float>& data,
      uint32_t width,
      uint32_t height,
      uint32_t numChannels,
      vk::ImageTiling tiling,
      Memory::MemoryTypes imageType = Memory::MemoryTypes::eDevice)
    {
        return this->imageT<float>(
          data, width, height, numChannels, tiling, imageType);
    }

    std::shared_ptr<ImageT<float>> image(
      const std::vector<float>& data,
      uint32_t width,
      uint32_t height,
      uint32_t numChannels,
      Memory::MemoryTypes imageType = Memory::MemoryTypes::eDevice)
    {
        return this->imageT<float>(data, width, height, numChannels, imageType);
    }

    std::shared_ptr<ImageT<float>> image(
      uint32_t width,
      uint32_t height,
      uint32_t numChannels,
      vk::ImageTiling tiling,
      Memory::MemoryTypes imageType = Memory::MemoryTypes::eDevice)
    {
        return this->imageT<float>(
          width, height, numChannels, tiling, imageType);
    }

    std::shared_ptr<ImageT<float>> image(
      uint32_t width,
      uint32_t height,
      uint32_t numChannels,
      Memory::MemoryTypes imageType = Memory::MemoryTypes::eDevice)
    {
        return this->imageT<float>(width, height, numChannels, imageType);
    }

    std::shared_ptr<Image> image(
      void* data,
      size_t dataSize,
      uint32_t width,
      uint32_t height,
      uint32_t numChannels,
      const Image::DataTypes& dataType,
      vk::ImageTiling tiling,
      Memory::MemoryTypes imageType = Memory::MemoryTypes::eDevice)
    {
        std::shared_ptr<Image> image{ new kp::Image(this->mPhysicalDevice,
                                                    this->mDevice,
                                                    data,
                                                    dataSize,
                                                    width,
                                                    height,
                                                    numChannels,
                                                    dataType,
                                                    tiling,
                                                    imageType) };

        if (this->mManageResources) {
            this->mManagedMemObjects.push_back(image);
        }

        return image;
    }

    std::shared_ptr<Image> image(
      void* data,
      size_t dataSize,
      uint32_t width,
      uint32_t height,
      uint32_t numChannels,
      const Memory::DataTypes& dataType,
      Memory::MemoryTypes imageType = Memory::MemoryTypes::eDevice)
    {
        std::shared_ptr<Image> image{ new kp::Image(this->mPhysicalDevice,
                                                    this->mDevice,
                                                    data,
                                                    dataSize,
                                                    width,
                                                    height,
                                                    numChannels,
                                                    dataType,
                                                    imageType) };

        if (this->mManageResources) {
            this->mManagedMemObjects.push_back(image);
        }

        return image;
    }

    std::shared_ptr<Image> image(
      uint32_t width,
      uint32_t height,
      uint32_t numChannels,
      const Memory::DataTypes& dataType,
      vk::ImageTiling tiling,
      Memory::MemoryTypes imageType = Memory::MemoryTypes::eDevice)
    {
        std::shared_ptr<Image> image{ new kp::Image(this->mPhysicalDevice,
                                                    this->mDevice,
                                                    width,
                                                    height,
                                                    numChannels,
                                                    dataType,
                                                    tiling,
                                                    imageType) };

        if (this->mManageResources) {
            this->mManagedMemObjects.push_back(image);
        }

        return image;
    }

    std::shared_ptr<Image> image(
      uint32_t width,
      uint32_t height,
      uint32_t numChannels,
      const Memory::DataTypes& dataType,
      Memory::MemoryTypes imageType = Memory::MemoryTypes::eDevice)
    {
        std::shared_ptr<Image> image{ new kp::Image(this->mPhysicalDevice,
                                                    this->mDevice,
                                                    width,
                                                    height,
                                                    numChannels,
                                                    dataType,
                                                    imageType) };

        if (this->mManageResources) {
            this->mManagedMemObjects.push_back(image);
        }

        return image;
    }

    /**
     * Default non-template function that can be used to create algorithm
     * objects which provides default types to the push and spec constants as
     * floats.
     *
     * @param memObjects (optional) The mem objects to initialise the algorithm
     * with
     * @param spirv (optional) The SPIRV bytes for the algorithm to dispatch
     * @param workgroup (optional) kp::Workgroup for algorithm to use, and
     * defaults to (tensor[0].size(), 1, 1)
     * @param specializationConstants (optional) float vector to use for
     * specialization constants, and defaults to an empty constant
     * @param pushConstants (optional) float vector to use for push constants,
     * and defaults to an empty constant
     * @returns Shared pointer with initialised algorithm
     */
    std::shared_ptr<Algorithm> algorithm(
      const std::vector<std::shared_ptr<Memory>>& memObjects = {},
      const std::vector<uint32_t>& spirv = {},
      const Workgroup& workgroup = {},
      const std::vector<float>& specializationConstants = {},
      const std::vector<float>& pushConstants = {})
    {
        return this->algorithm<>(
          memObjects, spirv, workgroup, specializationConstants, pushConstants);
    }

    /**
     * Create a managed algorithm that will be destroyed by this manager
     * if it hasn't been destroyed by its reference count going to zero.
     *
     * @param memObjects (optional) The mem objects to initialise the algorithm
     * with
     * @param spirv (optional) The SPIRV bytes for the algorithm to dispatch
     * @param workgroup (optional) kp::Workgroup for algorithm to use, and
     * defaults to (tensor[0].size(), 1, 1)
     * @param specializationConstants (optional) templatable vector parameter to
     * use for specialization constants, and defaults to an empty constant
     * @param pushConstants (optional) templatable vector parameter to use for
     * push constants, and defaults to an empty constant
     * @returns Shared pointer with initialised algorithm
     */
    template<typename S = float, typename P = float>
    std::shared_ptr<Algorithm> algorithm(
      const std::vector<std::shared_ptr<Memory>>& memObjects,
      const std::vector<uint32_t>& spirv,
      const Workgroup& workgroup,
      const std::vector<S>& specializationConstants,
      const std::vector<P>& pushConstants)
    {

        KP_LOG_DEBUG("Kompute Manager algorithm creation triggered");

        std::shared_ptr<Algorithm> algorithm{ new kp::Algorithm(
          this->mDevice,
          memObjects,
          spirv,
          workgroup,
          specializationConstants,
          pushConstants) };

        if (this->mManageResources) {
            this->mManagedAlgorithms.push_back(algorithm);
        }

        return algorithm;
    }

    /**
     * Destroy the GPU resources and all managed resources by manager.
     **/
    void destroy();
    /**
     * Run a pseudo-garbage collection to release all the managed resources
     * that have been already freed due to these reaching to zero ref count.
     **/
    void clear();

    /**
     * Information about the current device.
     *
     * @return vk::PhysicalDeviceProperties containing information about the
     *device
     **/
    vk::PhysicalDeviceProperties getDeviceProperties() const;

    /**
     * List the devices available in the current vulkan instance.
     *
     * @return vector of physical devices containing their respective properties
     **/
    std::vector<vk::PhysicalDevice> listDevices() const;

    /**
     * The current Vulkan instance.
     *
     * @return a shared pointer to the current Vulkan instance held by this
     *object
     **/
    std::shared_ptr<vk::Instance> getVkInstance() const;

  private:
    // -------------- OPTIONALLY OWNED RESOURCES
    std::shared_ptr<vk::Instance> mInstance = nullptr;
    bool mFreeInstance = false;
    std::shared_ptr<vk::PhysicalDevice> mPhysicalDevice = nullptr;
    std::shared_ptr<vk::Device> mDevice = nullptr;
    bool mFreeDevice = false;

    // -------------- ALWAYS OWNED RESOURCES
    std::vector<std::weak_ptr<Memory>> mManagedMemObjects;
    std::vector<std::weak_ptr<Sequence>> mManagedSequences;
    std::vector<std::weak_ptr<Algorithm>> mManagedAlgorithms;

    std::vector<uint32_t> mComputeQueueFamilyIndices;
    std::vector<std::shared_ptr<vk::Queue>> mComputeQueues;

    bool mManageResources = false;

#ifndef KOMPUTE_DISABLE_VK_DEBUG_LAYERS
    vk::DebugReportCallbackEXT mDebugReportCallback;
#ifdef VK_VERSION_1_4
    vk::detail::DispatchLoaderDynamic mDebugDispatcher;
#else
    vk::DispatchLoaderDynamic mDebugDispatcher;
#endif // VK_VERSION_1_4
#endif // KOMPUTE_DISABLE_VK_DEBUG_LAYERS

    // Create functions
    void createInstance();
    void createDevice(const std::vector<uint32_t>& familyQueueIndices = {},
                      uint32_t hysicalDeviceIndex = 0,
                      const std::vector<std::string>& desiredExtensions = {});
};

} // End namespace kp
