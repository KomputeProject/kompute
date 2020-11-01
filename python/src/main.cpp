#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <kompute/Kompute.hpp>

namespace py = pybind11;

PYBIND11_MODULE(komputepy, m) {

    py::enum_<kp::Tensor::TensorTypes>(m, "TensorTypes")
        .value("device", kp::Tensor::TensorTypes::eDevice)
        .value("staging", kp::Tensor::TensorTypes::eStaging)
        .value("storage", kp::Tensor::TensorTypes::eStorage)
        .export_values();

    py::class_<kp::Tensor, std::shared_ptr<kp::Tensor>>(m, "Tensor")
        .def(py::init(
            [](const std::vector<float>& data) {
                return std::unique_ptr<kp::Tensor>(new kp::Tensor(data));
            }))
        .def(py::init(
            [](const std::vector<float>& data, kp::Tensor::TensorTypes tensorTypes) {
                return std::unique_ptr<kp::Tensor>(new kp::Tensor(data, tensorTypes));
            }))
        .def("data", &kp::Tensor::data)
        .def("size", &kp::Tensor::size)
        .def("tensor_type", &kp::Tensor::tensorType)
        .def("is_init", &kp::Tensor::isInit)
        .def("set_data", &kp::Tensor::setData)
        .def("map_data_from_host", &kp::Tensor::mapDataFromHostMemory)
        .def("map_data_into_host", &kp::Tensor::mapDataIntoHostMemory);

    py::class_<kp::Sequence, std::shared_ptr<kp::Sequence>>(m, "Sequence")
        .def("init", &kp::Sequence::init)
        // record
        .def("begin", &kp::Sequence::begin)
        .def("end", &kp::Sequence::end)
        // eval
        .def("eval", &kp::Sequence::eval)
        .def("eval_async", &kp::Sequence::evalAsync)
        .def("eval_await", &kp::Sequence::evalAwait)
        // status
        .def("is_running", &kp::Sequence::isRunning)
        .def("is_rec", &kp::Sequence::isRecording)
        .def("is_init", &kp::Sequence::isInit)
        // record
        .def("record_tensor_create", &kp::Sequence::record<kp::OpTensorCreate>)
        .def("record_tensor_copy", &kp::Sequence::record<kp::OpTensorCopy>)
        .def("record_tensor_sync_device", &kp::Sequence::record<kp::OpTensorSyncDevice>)
        .def("record_tensor_sync_local", &kp::Sequence::record<kp::OpTensorSyncLocal>)
        .def("record_algo_mult", &kp::Sequence::record<kp::OpMult>)
        .def("record_algo_file", &kp::Sequence::record<kp::OpAlgoBase, std::string>)
        .def("record_algo_data", &kp::Sequence::record<kp::OpAlgoBase, std::vector<char>>)
        .def("record_algo_lro", &kp::Sequence::record<kp::OpAlgoLhsRhsOut>);

    py::class_<kp::Manager>(m, "Manager")
        .def(py::init())
        .def(py::init(
            [](uint32_t physicalDeviceIndex) {
                return std::unique_ptr<kp::Manager>(new kp::Manager(physicalDeviceIndex));
            }))
        .def(py::init(
            [](uint32_t physicalDeviceIndex, const std::vector<uint32_t>& familyQueueIndices) {
                return std::unique_ptr<kp::Manager>(new kp::Manager(physicalDeviceIndex, familyQueueIndices));
            }))
        .def("get_create_sequence", &kp::Manager::getOrCreateManagedSequence)
        .def("create_sequence", &kp::Manager::createManagedSequence,
                py::arg("name"), py::arg("queueIndex") = 0)
        .def("build_tensor", &kp::Manager::buildTensor, 
                py::arg("data"), py::arg("tensorType") = kp::Tensor::TensorTypes::eDevice)
        // Await functions
        .def("eval_await", &kp::Manager::evalOpAwait,
                py::arg("sequenceName"), py::arg("waitFor") = UINT64_MAX)
        .def("eval_await_def", &kp::Manager::evalOpAwaitDefault,
                py::arg("waitFor") = UINT64_MAX)
        // eval default
        .def("eval_tensor_create_def", &kp::Manager::evalOpDefault<kp::OpTensorCreate>)
        .def("eval_tensor_copy_def", &kp::Manager::evalOpDefault<kp::OpTensorCopy>)
        .def("eval_tensor_sync_device_def", &kp::Manager::evalOpDefault<kp::OpTensorSyncDevice>)
        .def("eval_tensor_sync_local_def", &kp::Manager::evalOpDefault<kp::OpTensorSyncLocal>)
        .def("eval_algo_mult_def", &kp::Manager::evalOpDefault<kp::OpMult>)
        .def("eval_algo_file_def", &kp::Manager::evalOpDefault<kp::OpAlgoBase, std::string>)
        .def("eval_algo_data_def", &kp::Manager::evalOpDefault<kp::OpAlgoBase, std::vector<char>>)
        .def("eval_algo_lro_def", &kp::Manager::evalOpDefault<kp::OpAlgoLhsRhsOut>)
        // eval
        .def("eval_tensor_create", &kp::Manager::evalOp<kp::OpTensorCreate>)
        .def("eval_tensor_copy", &kp::Manager::evalOp<kp::OpTensorCopy>)
        .def("eval_tensor_sync_device", &kp::Manager::evalOp<kp::OpTensorSyncDevice>)
        .def("eval_tensor_sync_local", &kp::Manager::evalOp<kp::OpTensorSyncLocal>)
        .def("eval_algo_mult", &kp::Manager::evalOp<kp::OpMult>)
        .def("eval_algo_file", &kp::Manager::evalOp<kp::OpAlgoBase, std::string>)
        .def("eval_algo_data", &kp::Manager::evalOp<kp::OpAlgoBase, std::vector<char>>)
        .def("eval_algo_lro", &kp::Manager::evalOp<kp::OpAlgoLhsRhsOut>)
        // eval async default
        .def("eval_async_tensor_create_def", &kp::Manager::evalOpAsyncDefault<kp::OpTensorCreate>)
        .def("eval_async_tensor_copy_def", &kp::Manager::evalOpAsyncDefault<kp::OpTensorCopy>)
        .def("eval_async_tensor_sync_device_def", &kp::Manager::evalOpAsyncDefault<kp::OpTensorSyncDevice>)
        .def("eval_async_tensor_sync_local_def", &kp::Manager::evalOpAsyncDefault<kp::OpTensorSyncLocal>)
        .def("eval_async_algo_mult_def", &kp::Manager::evalOpAsyncDefault<kp::OpMult>)
        .def("eval_async_algo_file_def", &kp::Manager::evalOpAsyncDefault<kp::OpAlgoBase, std::string>)
        .def("eval_async_algo_data_def", &kp::Manager::evalOpAsyncDefault<kp::OpAlgoBase, std::vector<char>>)
        .def("eval_async_algo_lro_def", &kp::Manager::evalOpAsyncDefault<kp::OpAlgoLhsRhsOut>)
        // eval async
        .def("eval_tensor_create", &kp::Manager::evalOpAsync<kp::OpTensorCreate>)
        .def("eval_tensor_copy", &kp::Manager::evalOpAsync<kp::OpTensorCopy>)
        .def("eval_tensor_sync_device", &kp::Manager::evalOpAsync<kp::OpTensorSyncDevice>)
        .def("eval_tensor_sync_local", &kp::Manager::evalOpAsync<kp::OpTensorSyncLocal>)
        .def("eval_algo_mult", &kp::Manager::evalOpAsync<kp::OpMult>)
        .def("eval_algo_file", &kp::Manager::evalOpAsync<kp::OpAlgoBase, std::string>)
        .def("eval_algo_data", &kp::Manager::evalOpAsync<kp::OpAlgoBase, std::vector<char>>)
        .def("eval_algo_lro", &kp::Manager::evalOpAsync<kp::OpAlgoLhsRhsOut>);


#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}
