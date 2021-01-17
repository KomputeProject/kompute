#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

#include <kompute/Kompute.hpp>

#include "docstrings.hpp"

namespace py = pybind11;

PYBIND11_MODULE(kp, m) {

    py::module_ np = py::module_::import("numpy");

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
            [np](const py::array_t<float> data, kp::Tensor::TensorTypes tensor_type) {
                const py::array_t<float> flatdata = np.attr("ravel")(data);
                const py::buffer_info info        = flatdata.request();
                const float* ptr                  = (float*) info.ptr;
                return std::unique_ptr<kp::Tensor>(
                    new kp::Tensor(std::vector<float>(ptr, ptr+flatdata.size()), tensor_type)
                );
            }), 
            "Construct Tensor with an array as initial data and an optional kp.TensorType (default:device).",
            py::arg("data"),
            py::arg("tensor_type") = kp::Tensor::TensorTypes::eDevice
            )
        .def("data", &kp::Tensor::data, DOC(kp, Tensor, data))
        .def("numpy", [](kp::Tensor& self) {
                return py::array(self.data().size(), self.data().data());
            }, "Returns stored data as a new numpy array.")
        .def("__getitem__", [](kp::Tensor &self, size_t index) -> float { return self.data()[index]; },
                "When only an index is necessary")
        .def("__setitem__", [](kp::Tensor &self, size_t index, float value) {
                self.data()[index] = value; })
        .def("set_data", [np](kp::Tensor &self, const py::array_t<float> data){
                const py::array_t<float> flatdata = np.attr("ravel")(data);
                const py::buffer_info info        = flatdata.request();
                const float* ptr                  = (float*) info.ptr;
                self.setData(std::vector<float>(ptr, ptr+flatdata.size()));
            }, "Overrides the data in the local Tensor memory.")
        .def("__iter__", [](kp::Tensor &self) {
                return py::make_iterator(self.data().begin(), self.data().end());
            }, py::keep_alive<0, 1>(), // Required to keep alive iterator while exists
            "Iterator to enable looping within data structure as required.")
        .def("__contains__", [](kp::Tensor &self, float v) {
                for (size_t i = 0; i < self.data().size(); ++i) {
                    if (v == self.data()[i]) {
                            return true;
                        }
                    }
                return false;
            })
        .def("__reversed__", [](kp::Tensor &self) { 
                size_t size = self.data().size();
                std::vector<float> reversed(size);
                for (size_t i = 0; i < size; i++) {
                    reversed[size - i - 1] = self.data()[i];
                }
                return reversed;
            })
        .def("size", &kp::Tensor::size, "Retrieves the size of the Tensor data as per the local Tensor memory.")
        .def("__len__", &kp::Tensor::size, "Retrieves the size of the Tensor data as per the local Tensor memory.")
        .def("tensor_type", &kp::Tensor::tensorType, "Retreves the memory type of the tensor.")
        .def("is_init", &kp::Tensor::isInit, "Checks whether the tensor GPU memory has been initialised.")
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
        .def("record_algo_file", [](kp::Sequence &self, 
                                    std::vector<std::shared_ptr<kp::Tensor>> tensors,
                                    const std::string& file_path,
                                    std::tuple<uint32_t,uint32_t,uint32_t> work_group) -> bool {
                const kp::OpAlgoBase::KomputeWorkgroup wgroup{
                    std::get<0>(work_group), std::get<1>(work_group), std::get<2>(work_group),
                };
                return self.record<kp::OpAlgoBase>(tensors, file_path, wgroup);
            },
            "Records an operation using a custom shader provided from a shader path",
            py::arg("tensors"), py::arg("file_path"), py::arg("work_group") = std::make_tuple(0,0,0)  )
        .def("record_algo_data", [](kp::Sequence &self,
                                    std::vector<std::shared_ptr<kp::Tensor>> tensors,
                                    py::bytes &bytes,
                                    std::tuple<uint32_t,uint32_t,uint32_t> work_group) -> bool {
                // Bytes have to be converted into std::vector
                py::buffer_info info(py::buffer(bytes).request());
                const char *data = reinterpret_cast<const char *>(info.ptr);
                size_t length = static_cast<size_t>(info.size);
                const kp::OpAlgoBase::KomputeWorkgroup wgroup{
                    std::get<0>(work_group), std::get<1>(work_group), std::get<2>(work_group),
                };
                return self.record<kp::OpAlgoBase>(
                    tensors, std::vector<char>(data, data + length), wgroup
                );
            },
            "Records an operation using a custom shader provided as spirv bytes",
            py::arg("tensors"), py::arg("bytes"), py::arg("work_group") = std::make_tuple(0,0,0)  )
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
                py::arg("name") = "", py::arg("queueIndex") = 0, "Create a sequence with specific name and specified index of available queues")
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
        .def("eval_algo_str_def", &kp::Manager::evalOpDefault<kp::OpAlgoBase, std::vector<char>>,
            "Evaluates an operation using a custom shader provided as string provided as list of characters with new anonymous Sequence")
        .def("eval_algo_str_def", [](kp::Manager &self,
                                     std::vector<std::shared_ptr<kp::Tensor>> tensors,
                                     const std::string& shader_str){
                const std::vector<char> shader_vec(shader_str.begin(), shader_str.end());
                self.evalOpDefault<kp::OpAlgoBase>(tensors, shader_vec);
        },
            "Evaluates an operation using a custom shader provided as string with a new anonymous Sequence")
        .def("eval_algo_data_def", [](kp::Manager &self,
                                    std::vector<std::shared_ptr<kp::Tensor>> tensors,
                                    py::bytes &bytes) {
                // Bytes have to be converted into std::vector
                py::buffer_info info(py::buffer(bytes).request());
                const char *data = reinterpret_cast<const char *>(info.ptr);
                size_t length = static_cast<size_t>(info.size);
                self.evalOpDefault<kp::OpAlgoBase>(
                    tensors,
                    std::vector<char>(data, data + length));
            },
            "Evaluates an operation using a custom shader provided as spirv bytes with new anonymous Sequence")
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
        .def("eval_algo_str", &kp::Manager::evalOp<kp::OpAlgoBase, std::vector<char>>,
            "Evaluates an operation using a custom shader provided as string provided as list of characters with explicitly named Sequence")
        .def("eval_algo_str", [](kp::Manager &self,
                                    std::vector<std::shared_ptr<kp::Tensor>> tensors,
                                    const std::string& sequenceName,
                                    const std::string& shader_str) {
                const std::vector<char> shader_vec(shader_str.begin(), shader_str.end());
                self.evalOp<kp::OpAlgoBase>(tensors, sequenceName, shader_vec);
            },
            "Evaluates an operation using a custom shader provided as string with explicitly named Sequence")
        .def("eval_algo_data", [](kp::Manager &self,
                                    std::vector<std::shared_ptr<kp::Tensor>> tensors,
                                    std::string sequenceName,
                                    py::bytes &bytes) {
                // Bytes have to be converted into std::vector
                py::buffer_info info(py::buffer(bytes).request());
                const char *data = reinterpret_cast<const char *>(info.ptr);
                size_t length = static_cast<size_t>(info.size);
                self.evalOp<kp::OpAlgoBase>(
                    tensors,
                    sequenceName,
                    std::vector<char>(data, data + length));
            },
            "Evaluates an operation using a custom shader provided as spirv bytes with explicitly named Sequence")
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
        .def("eval_async_algo_str_def", &kp::Manager::evalOpAsyncDefault<kp::OpAlgoBase, std::vector<char>>,
            "Evaluates Asynchronously an operation using a custom shader provided as string provided as list of characters with new anonymous Sequence")
        .def("eval_async_algo_str_def", [](kp::Manager &self,
                                           std::vector<std::shared_ptr<kp::Tensor>> tensors,
                                           const std::string& shader_str) {
                const std::vector<char> shader_vec(shader_str.begin(), shader_str.end());
                self.evalOpAsyncDefault<kp::OpAlgoBase>(tensors, shader_vec);
            },
            "Evaluates Asynchronously an operation using a custom shader provided as string with new anonymous Sequence")
        .def("eval_async_algo_data_def", [](kp::Manager &self,
                                    std::vector<std::shared_ptr<kp::Tensor>> tensors,
                                    py::bytes &bytes) {
                // Bytes have to be converted into std::vector
                py::buffer_info info(py::buffer(bytes).request());
                const char *data = reinterpret_cast<const char *>(info.ptr);
                size_t length = static_cast<size_t>(info.size);
                self.evalOpAsyncDefault<kp::OpAlgoBase>(
                    tensors,
                    std::vector<char>(data, data + length));
            },
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
        .def("eval_async_algo_str", &kp::Manager::evalOpAsync<kp::OpAlgoBase, std::vector<char>>,
            "Evaluates Asynchronous an operation using a custom shader provided as string provided as list of characters with explicitly named Sequence")
        .def("eval_async_algo_str", [](kp::Manager &self,
                                    std::vector<std::shared_ptr<kp::Tensor>> tensors,
                                    const std::string& sequenceName,
                                    const std::string& shader_str) {
                const std::vector<char> shader_vec(shader_str.begin(), shader_str.end());
                self.evalOpAsync<kp::OpAlgoBase>(tensors, sequenceName, shader_vec);
            },
            "Evaluates Asynchronous an operation using a custom shader provided as string with explicitly named Sequence")
        .def("eval_async_algo_data", [](kp::Manager &self,
                                    std::vector<std::shared_ptr<kp::Tensor>> tensors,
                                    std::string sequenceName,
                                    py::bytes &bytes) {
                // Bytes have to be converted into std::vector
                py::buffer_info info(py::buffer(bytes).request());
                const char *data = reinterpret_cast<const char *>(info.ptr);
                size_t length = static_cast<size_t>(info.size);
                self.evalOpAsync<kp::OpAlgoBase>(
                    tensors,
                    sequenceName,
                    std::vector<char>(data, data + length));
            },
            "Evaluates asynchronously an operation using a custom shader provided as raw string or spirv bytes with explicitly named Sequence")
        .def("eval_async_algo_lro", &kp::Manager::evalOpAsync<kp::OpAlgoLhsRhsOut>,
            "Evaluates asynchronously operation to run left right out operation with custom shader with explicitly named Sequence");

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}
