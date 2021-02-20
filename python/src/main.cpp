#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

#include <kompute/Kompute.hpp>

#include "docstrings.hpp"

namespace py = pybind11;
//used in Core.hpp
py::object kp_debug, kp_info, kp_warning, kp_error;

PYBIND11_MODULE(kp, m) {

    // The logging modules are used in the Kompute.hpp file
    py::module_ logging  = py::module_::import("logging");
    py::object kp_logger = logging.attr("getLogger")("kp");
    kp_debug             = kp_logger.attr("debug");
    kp_info              = kp_logger.attr("info");
    kp_warning           = kp_logger.attr("warning");
    kp_error             = kp_logger.attr("error");
    logging.attr("basicConfig")();

    py::module_ np = py::module_::import("numpy");


    py::enum_<kp::Tensor::TensorTypes>(m, "TensorTypes", DOC(kp, Tensor, TensorTypes))
        .value("device", kp::Tensor::TensorTypes::eDevice, "Tensor holding data in GPU memory.")
        .value("host", kp::Tensor::TensorTypes::eHost, "Tensor used for CPU visible GPU data.")
        .value("storage", kp::Tensor::TensorTypes::eStorage, "Tensor with host visible gpu memory.")
        .export_values();


    py::class_<kp::Shader>(m, "Shader", "Shader class")
        .def_static("compile_source", [](
                                    const std::string& source,
                                    const std::string& entryPoint,
                                    const std::vector<std::pair<std::string,std::string>>& definitions) {
                std::vector<uint32_t> spirv = kp::Shader::compile_source(source, entryPoint, definitions);
                return py::bytes((const char*)spirv.data(), spirv.size() * sizeof(uint32_t));
            },
            "Compiles string source provided and returns the value in bytes",
            py::arg("source"), py::arg("entryPoint") = "main", py::arg("definitions") = std::vector<std::pair<std::string,std::string>>() )
        .def_static("compile_sources", [](
                                    const std::vector<std::string>& source,
                                    const std::vector<std::string>& files,
                                    const std::string& entryPoint,
                                    const std::vector<std::pair<std::string,std::string>>& definitions) {
                std::vector<uint32_t> spirv = kp::Shader::compile_sources(source, files, entryPoint, definitions);
                return py::bytes((const char*)spirv.data(), spirv.size() * sizeof(uint32_t));
            },
            "Compiles sources provided with file names and returns the value in bytes",
            py::arg("sources"), py::arg("files") = std::vector<std::string>(), py::arg("entryPoint") = "main", py::arg("definitions") = std::vector<std::pair<std::string,std::string>>() );

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
        .def("init", &kp::Sequence::init, DOC(kp, Sequence, init))
        
        // record
        .def("begin", &kp::Sequence::begin, DOC(kp, Sequence, begin))
        .def("end", &kp::Sequence::end, DOC(kp, Sequence, end))
        
        // eval
        .def("eval", &kp::Sequence::eval, DOC(kp, Sequence, eval))
        .def("eval_async", &kp::Sequence::evalAsync, DOC(kp, Sequence, evalAsync))
        .def("eval_await", &kp::Sequence::evalAwait, DOC(kp, Sequence, evalAwait))
        
        // status
        .def("is_running", &kp::Sequence::isRunning, DOC(kp, Sequence, isRunning))
        .def("is_rec", &kp::Sequence::isRecording, DOC(kp, Sequence, isRecording))
        .def("is_init", &kp::Sequence::isInit, DOC(kp, Sequence, isInit))
        
        // record
        .def("record_tensor_copy", &kp::Sequence::record<kp::OpTensorCopy>, DOC(kp, Sequence, record))
        .def("record_tensor_sync_device", &kp::Sequence::record<kp::OpTensorSyncDevice>,
            "Records operation to sync tensor from local memory to GPU memory")
        .def("record_tensor_sync_local", &kp::Sequence::record<kp::OpTensorSyncLocal>,
            "Records operation to sync tensor(s) from GPU memory to local memory")
        .def("record_algo_file", &kp::Sequence::record<
                                    kp::OpAlgoBase,
                                    const std::string&,
                                    kp::Workgroup,
                                    kp::Constants>,
        "Records an operation using a custom shader provided from a shader path",
            py::arg("tensors"), py::arg("data"), py::arg("workgroup") = kp::Workgroup(), py::arg("constants") = kp::Constants() )
        .def("record_algo_data", [](kp::Sequence &self,
                                    std::vector<std::shared_ptr<kp::Tensor>> tensors,
                                    py::bytes &bytes,
                                    kp::Workgroup workgroup,
                                    kp::Constants constants) -> bool {
                // Bytes have to be converted into std::vector
                py::buffer_info info(py::buffer(bytes).request());
                const char *data = reinterpret_cast<const char *>(info.ptr);
                size_t length = static_cast<size_t>(info.size);
                return self.record<kp::OpAlgoBase>(
                    tensors, std::vector<uint32_t>((uint32_t*)data, (uint32_t*)(data + length)), workgroup, constants);
            },
            "Records an operation using a custom shader provided as spirv bytes",
            py::arg("tensors"), py::arg("bytes"), py::arg("workgroup") = kp::Workgroup(), py::arg("constants") = kp::Constants() );


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
        .def("sequence", &kp::Manager::sequence,
                py::arg("name") = "", py::arg("queueIndex") = 0, "Get or create a sequence with specific name and specified index of available queues")
        .def("tensor", &kp::Manager::tensor, 
                py::arg("data"), py::arg("tensorType") = kp::Tensor::TensorTypes::eDevice, py::arg("syncDataToGPU") = true,
                "Build and initialise tensor")
        .def("rebuild", py::overload_cast<std::vector<std::shared_ptr<kp::Tensor>>, bool>(&kp::Manager::rebuild),
                py::arg("tensors"), py::arg("syncDataToGPU") = true,
                "Build and initialise list of tensors")
        .def("rebuild", py::overload_cast<std::shared_ptr<kp::Tensor>, bool>(&kp::Manager::rebuild),
                py::arg("tensor"), py::arg("syncDataToGPU") = true,
                "Build and initialise tensor")
        .def("destroy", py::overload_cast<std::shared_ptr<kp::Tensor>>(&kp::Manager::destroy),
                py::arg("tensor"), DOC(kp, Manager, destroy))
        .def("destroy", py::overload_cast<std::vector<std::shared_ptr<kp::Tensor>>>(&kp::Manager::destroy),
                py::arg("tensors"), DOC(kp, Manager, destroy, 2))
        .def("destroy", py::overload_cast<std::vector<std::shared_ptr<kp::Sequence>>>(&kp::Manager::destroy),
                py::arg("sequences"), DOC(kp, Manager, destroy, 3))
        .def("destroy", py::overload_cast<std::shared_ptr<kp::Sequence>>(&kp::Manager::destroy),
                py::arg("sequence"), DOC(kp, Manager, destroy, 4))
        .def("destroy", py::overload_cast<const std::string &>(&kp::Manager::destroy),
                py::arg("sequenceName"), DOC(kp, Manager, destroy, 5))
        .def("destroy", py::overload_cast<const std::vector<std::string>&>(&kp::Manager::destroy),
                py::arg("sequenceNames"), DOC(kp, Manager, destroy, 6))
        // temporary backwards compatibility
        .def("eval_tensor_create_def",[](kp::Manager& self, std::vector<std::shared_ptr<kp::Tensor>> tensors, bool syncDataToGPU) -> void {
                    kp_error("IMPORTANT: eval_tensor_create_def is depricated! Please use Manager.rebuild instead as function will be removed soon.");
                    self.rebuild(tensors, syncDataToGPU);
                },
                py::arg("tensors"), py::arg("syncDataToGPU") = true,
                "Temporary backwards compatibility for tensor creation function which will be removed in the next version.")

        // Await functions
        .def("eval_await", &kp::Manager::evalOpAwait,
                py::arg("sequenceName"), py::arg("waitFor") = UINT64_MAX,
                "Awaits for asynchronous operation on a named Sequence")
        .def("eval_await_def", &kp::Manager::evalOpAwaitDefault,
                py::arg("waitFor") = UINT64_MAX, "Awaits for asynchronous operation on the last anonymous Sequence created")
        
        // eval default
        .def("eval_tensor_copy_def", &kp::Manager::evalOpDefault<kp::OpTensorCopy>,
            "Evaluates operation to copy one tensor to one or many tensors with new anonymous Sequence")
        .def("eval_tensor_sync_device_def", &kp::Manager::evalOpDefault<kp::OpTensorSyncDevice>,
            "Evaluates operation to sync tensor from local memory to GPU memory with new anonymous Sequence")
        .def("eval_tensor_sync_local_def", &kp::Manager::evalOpDefault<kp::OpTensorSyncLocal>,
            "Evaluates operation to sync tensor(s) from GPU memory to local memory with new anonymous Sequence")
        .def("eval_algo_file_def", &kp::Manager::evalOpDefault<
                                    kp::OpAlgoBase,
                                    const std::string&,
                                    kp::Workgroup,
                                    kp::Constants>,
            "Evaluates an operation using a custom shader provided from a shader path with new anonymous Sequence",
            py::arg("tensors"), py::arg("data"), py::arg("workgroup") = kp::Workgroup(), py::arg("constants") = kp::Constants() )
        .def("eval_algo_data_def", [](kp::Manager &self,
                                    std::vector<std::shared_ptr<kp::Tensor>> tensors,
                                    py::bytes &bytes,
                                    kp::Workgroup workgroup,
                                    kp::Constants constants) {
                // Bytes have to be converted into std::vector
                py::buffer_info info(py::buffer(bytes).request());
                const char *data = reinterpret_cast<const char *>(info.ptr);
                size_t length = static_cast<size_t>(info.size);
                self.evalOpDefault<kp::OpAlgoBase>(
                    tensors, std::vector<uint32_t>((uint32_t*)data, (uint32_t*)(data + length)), workgroup, constants);
            },
            "Evaluates an operation using a custom shader provided as spirv bytes with new anonymous Sequence",
            py::arg("tensors"), py::arg("bytes"), py::arg("workgroup") = kp::Workgroup(), py::arg("constants") = kp::Constants() )
        
        // eval
        .def("eval_tensor_copy", &kp::Manager::evalOp<kp::OpTensorCopy>,
            "Evaluates operation to copy one tensor to one or many tensors with explicitly named Sequence")
        .def("eval_tensor_sync_device", &kp::Manager::evalOp<kp::OpTensorSyncDevice>,
            "Evaluates operation to sync tensor from local memory to GPU memory with explicitly named Sequence")
        .def("eval_tensor_sync_local", &kp::Manager::evalOp<kp::OpTensorSyncLocal>,
            "Evaluates operation to sync tensor(s) from GPU memory to local memory with explicitly named Sequence")
        .def("eval_algo_file", &kp::Manager::evalOp<
                                    kp::OpAlgoBase,
                                    const std::string&,
                                    kp::Workgroup,
                                    kp::Constants>,
            "Evaluates an operation using a custom shader provided from a shader path with explicitly named Sequence",
            py::arg("tensors"), py::arg("sequence_name"), py::arg("data"),py::arg("workgroup") = kp::Workgroup(), py::arg("constants") = kp::Constants() )
        .def("eval_algo_data", [](kp::Manager &self,
                                    std::vector<std::shared_ptr<kp::Tensor>> tensors,
                                    std::string sequenceName,
                                    py::bytes &bytes,
                                    kp::Workgroup workgroup,
                                    kp::Constants constants) {
                // Bytes have to be converted into std::vector
                py::buffer_info info(py::buffer(bytes).request());
                const char *data = reinterpret_cast<const char *>(info.ptr);
                size_t length = static_cast<size_t>(info.size);
                self.evalOp<kp::OpAlgoBase>(
                    tensors, sequenceName, std::vector<uint32_t>((uint32_t*)data, (uint32_t*)(data + length)), workgroup, constants);
            },
            "Evaluates an operation using a custom shader provided as spirv bytes with explicitly named Sequence",
            py::arg("tensors"), py::arg("sequence_name"), py::arg("bytes"), py::arg("workgroup") = kp::Workgroup(), py::arg("constants") = kp::Constants() )
        
        // eval async default
        .def("eval_async_tensor_copy_def", &kp::Manager::evalOpAsyncDefault<kp::OpTensorCopy>,
            "Evaluates asynchronously operation to copy one tensor to one or many tensors with anonymous Sequence")
        .def("eval_async_tensor_sync_device_def", &kp::Manager::evalOpAsyncDefault<kp::OpTensorSyncDevice>,
            "Evaluates asynchronously operation to sync tensor from local memory to GPU memory with anonymous Sequence")
        .def("eval_async_tensor_sync_local_def", &kp::Manager::evalOpAsyncDefault<kp::OpTensorSyncLocal>,
            "Evaluates asynchronously operation to sync tensor(s) from GPU memory to local memory with anonymous Sequence")
        .def("eval_async_algo_file_def", &kp::Manager::evalOpAsyncDefault<
                                    kp::OpAlgoBase,
                                    const std::string&,
                                    kp::Workgroup,
                                    kp::Constants>,
            "Evaluates asynchronously an operation using a custom shader provided from a shader path with anonymous Sequence",
            py::arg("tensors"), py::arg("data"), py::arg("workgroup") = kp::Workgroup(), py::arg("constants") = kp::Constants() )
        .def("eval_async_algo_data_def", [](kp::Manager &self,
                                    std::vector<std::shared_ptr<kp::Tensor>> tensors,
                                    py::bytes &bytes,
                                    kp::Workgroup workgroup,
                                    kp::Constants constants) {
                // Bytes have to be converted into std::vector
                py::buffer_info info(py::buffer(bytes).request());
                const char *data = reinterpret_cast<const char *>(info.ptr);
                size_t length = static_cast<size_t>(info.size);
                self.evalOpAsyncDefault<kp::OpAlgoBase>(
                    tensors, std::vector<uint32_t>((uint32_t*)data, (uint32_t*)(data + length)), workgroup, constants);
            },
            "Evaluates asynchronously an operation using a custom shader provided as raw string or spirv bytes with anonymous Sequence",
            py::arg("tensors"), py::arg("bytes"), py::arg("workgroup") = kp::Workgroup(), py::arg("constants") = kp::Constants() )
        
        // eval async
        .def("eval_async_tensor_copy", &kp::Manager::evalOpAsync<kp::OpTensorCopy>,
            "Evaluates asynchronously operation to copy one tensor to one or many tensors with explicitly named Sequence")
        .def("eval_async_tensor_sync_device", &kp::Manager::evalOpAsync<kp::OpTensorSyncDevice>,
            "Evaluates asynchronously operation to sync tensor from local memory to GPU memory with explicitly named Sequence")
        .def("eval_async_tensor_sync_local", &kp::Manager::evalOpAsync<kp::OpTensorSyncLocal>,
            "Evaluates asynchronously operation to sync tensor(s) from GPU memory to local memory with explicitly named Sequence")
        .def("eval_async_algo_file", &kp::Manager::evalOpAsync<
                                    kp::OpAlgoBase,
                                    const std::string&,
                                    kp::Workgroup,
                                    kp::Constants>,
            "Evaluates asynchronously an operation using a custom shader provided from a shader path with explicitly named Sequence",
            py::arg("tensors"), py::arg("sequence_name"), py::arg("data"), py::arg("workgroup") = kp::Workgroup(), py::arg("constants") = kp::Constants() )
        .def("eval_async_algo_data", [](kp::Manager &self,
                                    std::vector<std::shared_ptr<kp::Tensor>> tensors,
                                    std::string sequenceName,
                                    py::bytes &bytes,
                                    kp::Workgroup workgroup,
                                    kp::Constants constants) {
                // Bytes have to be converted into std::vector
                py::buffer_info info(py::buffer(bytes).request());
                const char *data = reinterpret_cast<const char *>(info.ptr);
                size_t length = static_cast<size_t>(info.size);
                self.evalOpAsync<kp::OpAlgoBase>(
                    tensors, sequenceName, std::vector<uint32_t>((uint32_t*)data, (uint32_t*)(data + length)), workgroup, constants);
            },
            "Evaluates asynchronously an operation using a custom shader provided as raw string or spirv bytes with explicitly named Sequence",
            py::arg("tensors"), py::arg("sequence_name"), py::arg("bytes"), py::arg("workgroup") = kp::Workgroup(), py::arg("constants") = kp::Constants() );

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}
