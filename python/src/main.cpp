#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <kompute/Kompute.hpp>

#include "docstrings.hpp"

namespace py = pybind11;

PYBIND11_MODULE(kp, m) {

#if KOMPUTE_ENABLE_SPDLOG
    spdlog::set_level(
      static_cast<spdlog::level::level_enum>(SPDLOG_ACTIVE_LEVEL));
#endif

    m.def("log_level", [](uint8_t logLevel) {
#if KOMPUTE_ENABLE_SPDLOG
            spdlog::set_level(
              static_cast<spdlog::level::level_enum>(SPDLOG_LEVEL_INFO));
#else
            SPDLOG_WARN("SPDLOG not enabled so log level config function not supported");
#endif
        });

    py::enum_<kp::Tensor::TensorTypes>(m, "TensorTypes", DOC(kp, Tensor, TensorTypes))
        .value("device", kp::Tensor::TensorTypes::eDevice, "Tensor holding data in GPU memory.")
        .value("staging", kp::Tensor::TensorTypes::eStaging, "Tensor used for transfer of data to device.")
        .value("storage", kp::Tensor::TensorTypes::eStorage, "Tensor with host visible gpu memory.")
        .export_values();

    py::class_<kp::Tensor, std::shared_ptr<kp::Tensor>>(m, "Tensor", DOC(kp, Tensor))
        .def(py::init(
            [](const std::vector<float>& data) {
                return std::unique_ptr<kp::Tensor>(new kp::Tensor(data));
            }), DOC(kp, Tensor, Tensor, 2))
        .def(py::init(
            [](const std::vector<float>& data, kp::Tensor::TensorTypes tensorTypes) {
                return std::unique_ptr<kp::Tensor>(new kp::Tensor(data, tensorTypes));
            }), "Initialiser with list of data components and tensor GPU memory type.")
        .def("data", &kp::Tensor::data, DOC(kp, Tensor, data))
        .def("size", &kp::Tensor::size, "Retrieves the size of the Tensor data as per the local Tensor memory.")
        .def("tensor_type", &kp::Tensor::tensorType, "Retreves the memory type of the tensor.")
        .def("is_init", &kp::Tensor::isInit, "Checks whether the tensor GPU memory has been initialised.")
        .def("set_data", &kp::Tensor::setData, "Overrides the data in the local Tensor memory.")
        .def("map_data_from_host", &kp::Tensor::mapDataFromHostMemory, "Maps data into GPU memory from tensor local data.")
        .def("map_data_into_host", &kp::Tensor::mapDataIntoHostMemory, "Maps data from GPU memory into tensor local data.");

    py::class_<kp::Sequence, std::shared_ptr<kp::Sequence>>(m, "Sequence")
        .def("init", &kp::Sequence::init, "Initialises Vulkan resources within sequence using provided device.")
        // record
        .def("begin", &kp::Sequence::begin, "Clears previous commands and starts recording commands in sequence which can be run in batch.")
        .def("end", &kp::Sequence::end, "Stops listening and recording for new commands.")
        // eval
        .def("eval", &kp::Sequence::eval, "Executes the currently recorded commands synchronously by waiting on Vulkan Fence.")
        .def("eval_async", &kp::Sequence::evalAsync, "Executes the currently recorded commands asynchronously.")
        .def("eval_await", &kp::Sequence::evalAwait, "Waits until the execution finishes using Vulkan Fence.")
        // status
        .def("is_running", &kp::Sequence::isRunning, "Checks whether the Sequence operations are currently still executing.")
        .def("is_rec", &kp::Sequence::isRecording, "Checks whether the Sequence is currently in recording mode.")
        .def("is_init", &kp::Sequence::isInit, "Checks if the Sequence has been initialized")
        // record
        .def("record_tensor_create", &kp::Sequence::record<kp::OpTensorCreate>,
            "Records operation to create and initialise tensor GPU memory and buffer")
        .def("record_tensor_copy", &kp::Sequence::record<kp::OpTensorCopy>,
            "Records operation to copy one tensor to one or many tensors")
        .def("record_tensor_sync_device", &kp::Sequence::record<kp::OpTensorSyncDevice>,
            "Records operation to sync tensor from local memory to GPU memory")
        .def("record_tensor_sync_local", &kp::Sequence::record<kp::OpTensorSyncLocal>,
            "Records operation to sync tensor(s) from GPU memory to local memory using staging tensors")
        .def("record_algo_mult", &kp::Sequence::record<kp::OpMult>,
            "Records operation to run multiplication compute shader to two input tensors and an output tensor")
        .def("record_algo_file", &kp::Sequence::record<kp::OpAlgoBase, std::string>,
            "Records an operation using a custom shader provided from a shader path")
        .def("record_algo_data", &kp::Sequence::record<kp::OpAlgoBase, py::bytes>,
            "Records an operation using a custom shader provided as raw string or spirv bytes")
        .def("record_algo_lro", &kp::Sequence::record<kp::OpAlgoLhsRhsOut>,
            "Records operation to run left right out operation with custom shader");

    py::class_<kp::Manager>(m, "Manager")
        .def(py::init(), "Default initializer uses device 0 and first compute compatible GPU queueFamily")
        .def(py::init(
            [](uint32_t physicalDeviceIndex) {
                return std::unique_ptr<kp::Manager>(new kp::Manager(physicalDeviceIndex));
            }), "Manager initialiser can provide specified device index but will use first compute compatible GPU queueFamily")
        .def(py::init(
            [](uint32_t physicalDeviceIndex, const std::vector<uint32_t>& familyQueueIndices) {
                return std::unique_ptr<kp::Manager>(new kp::Manager(physicalDeviceIndex, familyQueueIndices));
            }), "Manager initialiser can provide specified device and array of GPU queueFamilies to load.")
        .def("get_create_sequence", &kp::Manager::getOrCreateManagedSequence, "Get a Sequence or create a new one with given name")
        .def("create_sequence", &kp::Manager::createManagedSequence,
                py::arg("name"), py::arg("queueIndex") = 0, "Create a sequence with specific name and specified index of available queues")
        .def("build_tensor", &kp::Manager::buildTensor, 
                py::arg("data"), py::arg("tensorType") = kp::Tensor::TensorTypes::eDevice,
                "Build and initialise tensor")
        // Await functions
        .def("eval_await", &kp::Manager::evalOpAwait,
                py::arg("sequenceName"), py::arg("waitFor") = UINT64_MAX,
                "Awaits for asynchronous operation on a named Sequence")
        .def("eval_await_def", &kp::Manager::evalOpAwaitDefault,
                py::arg("waitFor") = UINT64_MAX, "Awaits for asynchronous operation on the last anonymous Sequence created")
        // eval default
        .def("eval_tensor_create_def", &kp::Manager::evalOpDefault<kp::OpTensorCreate>,
            "Evaluates operation to create and initialise tensor GPU memory and buffer with new anonymous Sequence")
        .def("eval_tensor_copy_def", &kp::Manager::evalOpDefault<kp::OpTensorCopy>,
            "Evaluates operation to copy one tensor to one or many tensors with new anonymous Sequence")
        .def("eval_tensor_sync_device_def", &kp::Manager::evalOpDefault<kp::OpTensorSyncDevice>,
            "Evaluates operation to sync tensor from local memory to GPU memory with new anonymous Sequence")
        .def("eval_tensor_sync_local_def", &kp::Manager::evalOpDefault<kp::OpTensorSyncLocal>,
            "Evaluates operation to sync tensor(s) from GPU memory to local memory using staging tensors with new anonymous Sequence")
        .def("eval_algo_mult_def", &kp::Manager::evalOpDefault<kp::OpMult>,
            "Evaluates operation to run multiplication compute shader to two input tensors and an output tensor with new anonymous Sequence")
        .def("eval_algo_file_def", &kp::Manager::evalOpDefault<kp::OpAlgoBase, std::string>,
            "Evaluates an operation using a custom shader provided from a shader path with new anonymous Sequence")
        .def("eval_algo_data_def", &kp::Manager::evalOpDefault<kp::OpAlgoBase, py::bytes>,
            "Evaluates an operation using a custom shader provided as raw string or spirv bytes with new anonymous Sequence")
        .def("eval_algo_lro_def", &kp::Manager::evalOpDefault<kp::OpAlgoLhsRhsOut>,
            "Evaluates operation to run left right out operation with custom shader with new anonymous Sequence")
        // eval
        .def("eval_tensor_create", &kp::Manager::evalOp<kp::OpTensorCreate>,
            "Evaluates operation to create and initialise tensor GPU memory and buffer with explicitly named Sequence")
        .def("eval_tensor_copy", &kp::Manager::evalOp<kp::OpTensorCopy>,
            "Evaluates operation to copy one tensor to one or many tensors with explicitly named Sequence")
        .def("eval_tensor_sync_device", &kp::Manager::evalOp<kp::OpTensorSyncDevice>,
            "Evaluates operation to sync tensor from local memory to GPU memory with explicitly named Sequence")
        .def("eval_tensor_sync_local", &kp::Manager::evalOp<kp::OpTensorSyncLocal>,
            "Evaluates operation to sync tensor(s) from GPU memory to local memory using staging tensors with explicitly named Sequence")
        .def("eval_algo_mult", &kp::Manager::evalOp<kp::OpMult>,
            "Evaluates operation to run multiplication compute shader to two input tensors and an output tensor with explicitly named Sequence")
        .def("eval_algo_file", &kp::Manager::evalOp<kp::OpAlgoBase, std::string>,
            "Evaluates an operation using a custom shader provided from a shader path with explicitly named Sequence")
        .def("eval_algo_data", &kp::Manager::evalOp<kp::OpAlgoBase, py::bytes>,
            "Evaluates an operation using a custom shader provided as raw string or spirv bytes with explicitly named Sequence")
        .def("eval_algo_lro", &kp::Manager::evalOp<kp::OpAlgoLhsRhsOut>,
            "Evaluates operation to run left right out operation with custom shader with explicitly named Sequence")
        // eval async default
        .def("eval_async_tensor_create_def", &kp::Manager::evalOpAsyncDefault<kp::OpTensorCreate>,
            "Evaluates asynchronously operation to create and initialise tensor GPU memory and buffer with anonymous Sequence")
        .def("eval_async_tensor_copy_def", &kp::Manager::evalOpAsyncDefault<kp::OpTensorCopy>,
            "Evaluates asynchronously operation to copy one tensor to one or many tensors with anonymous Sequence")
        .def("eval_async_tensor_sync_device_def", &kp::Manager::evalOpAsyncDefault<kp::OpTensorSyncDevice>,
            "Evaluates asynchronously operation to sync tensor from local memory to GPU memory with anonymous Sequence")
        .def("eval_async_tensor_sync_local_def", &kp::Manager::evalOpAsyncDefault<kp::OpTensorSyncLocal>,
            "Evaluates asynchronously operation to sync tensor(s) from GPU memory to local memory using staging tensors with anonymous Sequence")
        .def("eval_async_algo_mult_def", &kp::Manager::evalOpAsyncDefault<kp::OpMult>,
            "Evaluates asynchronously operation to run multiplication compute shader to two input tensors and an output tensor with anonymous Sequence")
        .def("eval_async_algo_file_def", &kp::Manager::evalOpAsyncDefault<kp::OpAlgoBase, std::string>,
            "Evaluates asynchronously an operation using a custom shader provided from a shader path with anonymous Sequence")
        .def("eval_async_algo_data_def", &kp::Manager::evalOpAsyncDefault<kp::OpAlgoBase, py::bytes>,
            "Evaluates asynchronously an operation using a custom shader provided as raw string or spirv bytes with anonymous Sequence")
        .def("eval_async_algo_lro_def", &kp::Manager::evalOpAsyncDefault<kp::OpAlgoLhsRhsOut>,
            "Evaluates asynchronously operation to run left right out operation with custom shader with anonymous Sequence")
        // eval async
        .def("eval_async_tensor_create", &kp::Manager::evalOpAsync<kp::OpTensorCreate>,
            "Evaluates asynchronously operation to create and initialise tensor GPU memory and buffer with explicitly named Sequence")
        .def("eval_async_tensor_copy", &kp::Manager::evalOpAsync<kp::OpTensorCopy>,
            "Evaluates asynchronously operation to copy one tensor to one or many tensors with explicitly named Sequence")
        .def("eval_async_tensor_sync_device", &kp::Manager::evalOpAsync<kp::OpTensorSyncDevice>,
            "Evaluates asynchronously operation to sync tensor from local memory to GPU memory with explicitly named Sequence")
        .def("eval_async_tensor_sync_local", &kp::Manager::evalOpAsync<kp::OpTensorSyncLocal>,
            "Evaluates asynchronously operation to sync tensor(s) from GPU memory to local memory using staging tensors with explicitly named Sequence")
        .def("eval_async_algo_mult", &kp::Manager::evalOpAsync<kp::OpMult>,
            "Evaluates asynchronously operation to run multiplication compute shader to two input tensors and an output tensor with explicitly named Sequence")
        .def("eval_async_algo_file", &kp::Manager::evalOpAsync<kp::OpAlgoBase, std::string>,
            "Evaluates asynchronously an operation using a custom shader provided from a shader path with explicitly named Sequence")
        .def("eval_async_algo_data", &kp::Manager::evalOpAsync<kp::OpAlgoBase, py::bytes>,
            "Evaluates asynchronously an operation using a custom shader provided as raw string or spirv bytes with explicitly named Sequence")
        .def("eval_async_algo_lro", &kp::Manager::evalOpAsync<kp::OpAlgoLhsRhsOut>,
            "Evaluates asynchronously operation to run left right out operation with custom shader with explicitly named Sequence");

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}
