
#include <kompute/Kompute.hpp>
#include <pybind11/pybind11.h>
#include <vulkan/vulkan.hpp>

using namespace pybind11::literals; // for the `_a` literal

namespace kp {
namespace py {
static pybind11::dict
vkPropertiesToDict(const vk::PhysicalDeviceProperties& properties)
{
    std::string deviceName = properties.deviceName;
    pybind11::dict pyDict(
      "device_name"_a = deviceName,
      "max_work_group_count"_a =
        pybind11::make_tuple(properties.limits.maxComputeWorkGroupCount[0],
                             properties.limits.maxComputeWorkGroupCount[1],
                             properties.limits.maxComputeWorkGroupCount[2]),
      "max_work_group_invocations"_a =
        properties.limits.maxComputeWorkGroupInvocations,
      "max_work_group_size"_a =
        pybind11::make_tuple(properties.limits.maxComputeWorkGroupSize[0],
                             properties.limits.maxComputeWorkGroupSize[1],
                             properties.limits.maxComputeWorkGroupSize[2]),
      "max_push_constants_size"_a =
        properties.limits.maxPushConstantsSize,
      "timestamps_supported"_a =
        (bool)properties.limits.timestampComputeAndGraphics);

    return pyDict;
}

static pybind11::dict
vkFeaturesToDict(const vk::PhysicalDeviceFeatures& features)
{
    pybind11::dict pyDict(
      "robust_buffer_access"_a = (bool)features.robustBufferAccess,
      "shader_float64"_a = (bool)features.shaderFloat64,
      "shader_int64"_a = (bool)features.shaderInt64,
      "shader_int16"_a = (bool)features.shaderInt16
    );
    return pyDict;
}
}
}
