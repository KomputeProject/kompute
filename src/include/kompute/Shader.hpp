#pragma once

#include "kompute/Core.hpp"
#include "logger/Logger.hpp"
#include <memory>

namespace kp {

// forward declarations for std::shared_from_this
class Module;

/*
 * Wrapper for Vulkan's shader modules.
 * The purpose of this is to manage the module lifetime, while
 * building the groundwork for easily integrating things like
 * SPIR-V reflection and multiple entry points in the future.
 */
class Module : public std::enable_shared_from_this<Module>
{
	// the vulkan device; not owned by this object
	std::weak_ptr<vk::Device> mDevice;
	
	// the shader module handle
	vk::ShaderModule mShaderModule;
	
public:
	
	/*
	 * Constructor accepting a device and a SPIR-V binary
	 */
	Module(const std::shared_ptr<vk::Device>& device,
		const std::vector<uint32_t>& spv);
	
	/*
	 * getter for mShaderModule
	 */
	vk::ShaderModule& getShaderModule() { return mShaderModule; }
	
	/*
	 * Destroys the shader module properly.
	 */
	~Module();
};
	
} // End namespace kp
