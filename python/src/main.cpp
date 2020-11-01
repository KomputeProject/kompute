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

    py::class_<kp::Tensor>(m, "Tensor")
        .def(py::init(
            [](const std::vector<float>& data) {
                return std::unique_ptr<kp::Tensor>(new kp::Tensor(data));
            }))
        .def(py::init(
            [](const std::vector<float>& data, kp::Tensor::TensorTypes tensorTypes) {
                return std::unique_ptr<kp::Tensor>(new kp::Tensor(data, tensorTypes));
            }))
        .def("data", &kp::Tensor::data);

    py::class_<kp::OpBase>(m, "OpBase");

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}
