#pragma once
#include "kompute/Core.hpp"

namespace kp {

typedef std::shared_ptr<vk::ShaderModule> shader_module_ptr;
typedef std::shared_ptr<vk::Pipeline> pipeline_ptr;


class Pipeline

class PipelineCache
{
	std::map<
		// shader module, entry point, spec constant data
		std::tuple<shader_module_ptr, std::string, std::vector<uint8_t>>,
		pipeline_ptr>
			mPipelines;
public:
	// gets pipeline if it exists; if not, pipeline is created and returned
	pipeline_ptr getPipeline(const shader_module_ptr& shaderModule,
		const std::string& entryPoint, const std::vector<uint8_t>& specConstants);
		
	void clear() { mPipelines.clear(); }
};

} // End namespace kp
