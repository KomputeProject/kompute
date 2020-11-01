#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <kompute/Kompute.hpp>

namespace py = pybind11;

PYBIND11_MODULE(komputepy, m) {

    py::enum_<kp::Tensor::TensorTypes>(m, "TensorTypes")
        .value("eDevice", kp::Tensor::TensorTypes::eDevice)
        .value("eStaging", kp::Tensor::TensorTypes::eStaging)
        .value("eStorage", kp::Tensor::TensorTypes::eStorage)
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
        .def("data", &kp::Tensor::data);

    py::class_<kp::Sequence, std::shared_ptr<kp::Sequence>>(m, "Sequence")
        .def("init", &kp::Sequence::init)
        .def("begin", &kp::Sequence::begin)
        .def("end", &kp::Sequence::end)
        .def("eval", &kp::Sequence::eval)
        .def("evalAsync", &kp::Sequence::evalAsync)
        .def("evalAwait", &kp::Sequence::evalAwait)
        .def("isRunning", &kp::Sequence::isRunning)
        .def("isRecording", &kp::Sequence::isRecording)
        .def("isInit", &kp::Sequence::isInit)
        .def("recordOpTensorCreate", &kp::Sequence::record<kp::OpTensorCreate>)
        .def("recordOpTensorCopy", &kp::Sequence::record<kp::OpTensorCopy>)
        .def("recordOpTensorSyncDevice", &kp::Sequence::record<kp::OpTensorSyncDevice>)
        .def("recordOpTensorSyncLocal", &kp::Sequence::record<kp::OpTensorSyncLocal>)
        .def("recordOpAlgoMult", &kp::Sequence::record<kp::OpMult>)
        .def("recordOpAlgoBaseFile", &kp::Sequence::record<kp::OpAlgoBase, std::string>)
        .def("recordOpAlgoBaseData", &kp::Sequence::record<kp::OpAlgoBase, std::vector<char>>)
        .def("recordOpAlgoLhsRhsOut", &kp::Sequence::record<kp::OpAlgoLhsRhsOut>);

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
        .def("getOrCreateManagedSequence", &kp::Manager::getOrCreateManagedSequence)
        .def("createManagedSequence", &kp::Manager::createManagedSequence,
                py::arg("name"), py::arg("queueIndex") = 0)
        .def("buildTensor", &kp::Manager::buildTensor, 
                py::arg("data"), py::arg("tensorType") = kp::Tensor::TensorTypes::eDevice)
        .def("evalOpAsync", &kp::Manager::evalOpAsync<kp::OpMult>)
        .def("evalOpAsyncDefault", &kp::Manager::evalOpAsyncDefault<kp::OpMult>)
        .def("evalOpDefaultTensorCreate", &kp::Manager::evalOpDefault<kp::OpTensorCreate>)
        .def("evalOpDefaultTensorCopy", &kp::Manager::evalOpDefault<kp::OpTensorCopy>)
        .def("evalOpDefaultTensorSyncDevice", &kp::Manager::evalOpDefault<kp::OpTensorSyncDevice>)
        .def("evalOpDefaultTensorSyncLocal", &kp::Manager::evalOpDefault<kp::OpTensorSyncLocal>)
        .def("evalOpDefaultAlgoMult", &kp::Manager::evalOpDefault<kp::OpMult>)
        .def("evalOpDefaultAlgoBaseFile", &kp::Manager::evalOpDefault<kp::OpAlgoBase, std::string>)
        .def("evalOpDefaultAlgoBaseData", &kp::Manager::evalOpDefault<kp::OpAlgoBase, std::vector<char>>)
        .def("evalOpDefaultAlgoLhsRhsOut", &kp::Manager::evalOpDefault<kp::OpAlgoLhsRhsOut>)
        .def("evalOpTensorCreate", &kp::Manager::evalOp<kp::OpTensorCreate>)
        .def("evalOpTensorCopy", &kp::Manager::evalOp<kp::OpTensorCopy>)
        .def("evalOpTensorSyncDevice", &kp::Manager::evalOp<kp::OpTensorSyncDevice>)
        .def("evalOpTensorSyncLocal", &kp::Manager::evalOp<kp::OpTensorSyncLocal>)
        .def("evalOpAlgoMult", &kp::Manager::evalOp<kp::OpMult>)
        .def("evalOpAlgoBaseFile", &kp::Manager::evalOp<kp::OpAlgoBase, std::string>)
        .def("evalOpAlgoBaseData", &kp::Manager::evalOp<kp::OpAlgoBase, std::vector<char>>)
        .def("evalOpAlgoLhsRhsOut", &kp::Manager::evalOp<kp::OpAlgoLhsRhsOut>)
        .def("evalOpAsyncDefaultTensorCreate", &kp::Manager::evalOpAsyncDefault<kp::OpTensorCreate>)
        .def("evalOpAsyncDefaultTensorCopy", &kp::Manager::evalOpAsyncDefault<kp::OpTensorCopy>)
        .def("evalOpAsyncDefaultTensorSyncDevice", &kp::Manager::evalOpAsyncDefault<kp::OpTensorSyncDevice>)
        .def("evalOpAsyncDefaultTensorSyncLocal", &kp::Manager::evalOpAsyncDefault<kp::OpTensorSyncLocal>)
        .def("evalOpAsyncDefaultAlgoMult", &kp::Manager::evalOpAsyncDefault<kp::OpMult>)
        .def("evalOpAsyncDefaultAlgoBaseFile", &kp::Manager::evalOpAsyncDefault<kp::OpAlgoBase, std::string>)
        .def("evalOpAsyncDefaultAlgoBaseData", &kp::Manager::evalOpAsyncDefault<kp::OpAlgoBase, std::vector<char>>)
        .def("evalOpAsyncDefaultAlgoLhsRhsOut", &kp::Manager::evalOpAsyncDefault<kp::OpAlgoLhsRhsOut>)
        .def("evalOpAsyncTensorCreate", &kp::Manager::evalOpAsync<kp::OpTensorCreate>)
        .def("evalOpAsyncTensorCopy", &kp::Manager::evalOpAsync<kp::OpTensorCopy>)
        .def("evalOpAsyncTensorSyncDevice", &kp::Manager::evalOpAsync<kp::OpTensorSyncDevice>)
        .def("evalOpAsyncTensorSyncLocal", &kp::Manager::evalOpAsync<kp::OpTensorSyncLocal>)
        .def("evalOpAsync", &kp::Manager::evalOpAsync<kp::OpMult>)
        .def("evalOpAsyncAlgoBaseFile", &kp::Manager::evalOpAsync<kp::OpAlgoBase, std::string>)
        .def("evalOpAsyncAlgoBase", &kp::Manager::evalOpAsync<kp::OpAlgoBase, std::vector<char>>)
        .def("evalOpAsyncAlgoLhsRhsOut", &kp::Manager::evalOpAsync<kp::OpAlgoLhsRhsOut>);


#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}
