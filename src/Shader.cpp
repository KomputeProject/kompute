#include "kompute/Shader.hpp"

namespace kp {

Module::Module(const std::shared_ptr<vk::Device>& device,
	const std::vector<uint32_t>& spv) :
	mDevice(device)
{
	KP_LOG_DEBUG("Kompute Module constructor started");
	KP_LOG_DEBUG("Kompute Module Creating shader module. ShaderFileSize: {}",
				 spv.size());
	vk::ShaderModuleCreateInfo shaderModuleInfo(vk::ShaderModuleCreateFlags(),
		sizeof(uint32_t) * spv.size(), spv.data());
	this->mDevice.lock()->createShaderModule(
		&shaderModuleInfo, nullptr, &(this->mShaderModule) );
	KP_LOG_DEBUG("Kompute Module constructor success");
}

Module::~Module()
{
	KP_LOG_DEBUG("Kompute Module destructor started");
	KP_LOG_DEBUG("Kompute Module Destroying shader module");
	if (!mDevice.expired() )
		mDevice.lock()->destroyShaderModule(mShaderModule);
	KP_LOG_DEBUG("Kompute Module destructor success");
}

} // end namespace kp
