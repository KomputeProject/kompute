#include "kompute/Shader.hpp"

namespace kp {

Shader::Shader(const std::shared_ptr<vk::Device>& device,
	const std::vector<uint32_t>& spv) :
	mDevice(device)
{
	KP_LOG_DEBUG("Kompute Module constructor started");
	KP_LOG_DEBUG("Kompute Module Creating shader module. ShaderFileSize: {}",
				 spv.size());
	vk::ShaderModuleCreateInfo shaderModuleInfo(vk::ShaderModuleCreateFlags(),
		sizeof(uint32_t) * spv.size(), spv.data());
	this->mDevice->createShaderModule(
		&shaderModuleInfo, nullptr, &(this->mShaderModule) );
	KP_LOG_DEBUG("Kompute Module constructor success");
}

const vk::ShaderModule& Shader::getShaderModule()
{
	if (this->mDestroyed)
		throw std::runtime_error("Attempting to get vk::ShaderModule from destroyed kp::Shader instance");
	return this->mShaderModule;
}

void Shader::destroy()
{
	KP_LOG_DEBUG("Kompute Module destructor started");
	KP_LOG_DEBUG("Kompute Module Destroying shader module");
	if (!this->mDestroyed)
	{
		this->mDestroyed = true;
		this->mDevice->destroyShaderModule(this->mShaderModule);
	}
	KP_LOG_DEBUG("Kompute Module destructor success");
}

Shader::~Shader()
{
	this->destroy();
}

} // end namespace kp
