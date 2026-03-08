#pragma once

#include "kompute/Core.hpp"
#include "logger/Logger.hpp"
#include <memory>

namespace kp {

// forward declarations for std::shared_from_this
class Shader;

/*
 * Wrapper for Vulkan's shader modules.
 */
class Shader
{
	// not-owned resources
	std::shared_ptr<vk::Device> mDevice;
	
	// owned resources
	vk::ShaderModule mShaderModule;
	bool mDestroyed = false;
	
public:
	
	/**
	 * Constructor accepting a device and a SPIR-V binary
	 * @param device The vk::Device for the shader module to be compiled for
	 * @param spv The SPIR-V binary
	 **/
	Shader(const std::shared_ptr<vk::Device>& device,
		const std::vector<uint32_t>& spv);
	
	/**
	 * getter for mShaderModule
	 **/
	const vk::ShaderModule& getShaderModule();
	
	void destroy();
	
	~Shader();
};
	
} // End namespace kp
