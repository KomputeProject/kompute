#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

#include <kompute/Kompute.hpp>

#include "fmt/ranges.h"

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

    py::enum_<kp::Tensor::TensorTypes>(m, "TensorTypes")
        .value("device", kp::Tensor::TensorTypes::eDevice, "Tensor holding data in GPU memory.")
        .value("host", kp::Tensor::TensorTypes::eHost, "Tensor used for CPU visible GPU data.")
        .value("storage", kp::Tensor::TensorTypes::eStorage, "Tensor with host visible gpu memory.")
        .export_values();

#if !defined(KOMPUTE_DISABLE_SHADER_UTILS) || !KOMPUTE_DISABLE_SHADER_UTILS
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
#endif // KOMPUTE_DISABLE_SHADER_UTILS

    py::class_<kp::OpBase, std::shared_ptr<kp::OpBase>>(m, "OpBase");

    py::class_<kp::OpTensorSyncDevice, std::shared_ptr<kp::OpTensorSyncDevice>>(m, "OpTensorSyncDevice", py::base<kp::OpBase>())
        .def(py::init<const std::vector<std::shared_ptr<kp::Tensor>>&>());

    py::class_<kp::OpTensorSyncLocal, std::shared_ptr<kp::OpTensorSyncLocal>>(m, "OpTensorSyncLocal", py::base<kp::OpBase>())
        .def(py::init<const std::vector<std::shared_ptr<kp::Tensor>>&>());

    py::class_<kp::OpTensorCopy, std::shared_ptr<kp::OpTensorCopy>>(m, "OpTensorCopy", py::base<kp::OpBase>())
        .def(py::init<const std::vector<std::shared_ptr<kp::Tensor>>&>());

    py::class_<kp::OpAlgoDispatch, std::shared_ptr<kp::OpAlgoDispatch>>(m, "OpAlgoDispatch", py::base<kp::OpBase>())
        .def(py::init<const std::shared_ptr<kp::Algorithm>&,const kp::Constants&>(),
                py::arg("algorithm"), py::arg("push_consts") = kp::Constants());

    py::class_<kp::OpMult, std::shared_ptr<kp::OpMult>>(m, "OpMult", py::base<kp::OpBase>())
        .def(py::init<const std::vector<std::shared_ptr<kp::Tensor>>&,const std::shared_ptr<kp::Algorithm>&>());

    py::class_<kp::Algorithm, std::shared_ptr<kp::Algorithm>>(m, "Algorithm")
        .def("get_tensors", &kp::Algorithm::getTensors)
        .def("destroy", &kp::Algorithm::destroy)
        .def("get_spec_consts", &kp::Algorithm::getSpecializationConstants)
        .def("is_init", &kp::Algorithm::isInit);

    py::class_<kp::Tensor, std::shared_ptr<kp::Tensor>>(m, "Tensor", DOC(kp, Tensor))
        .def("data", [](kp::Tensor& self) {
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
        .def("destroy", &kp::Tensor::destroy, "Destroy tensor GPU resources.");

    py::class_<kp::Sequence, std::shared_ptr<kp::Sequence>>(m, "Sequence")
        .def("record", [](kp::Sequence& self, std::shared_ptr<kp::OpBase> op) { return self.record(op); })
        .def("eval", [](kp::Sequence& self) { return self.eval(); })
        .def("eval", [](kp::Sequence& self, std::shared_ptr<kp::OpBase> op) { return self.eval(op); })
        .def("eval_async", [](kp::Sequence& self) { return self.eval(); })
        .def("eval_async", [](kp::Sequence& self, std::shared_ptr<kp::OpBase> op) { return self.evalAsync(op); })
        .def("eval_await", [](kp::Sequence& self) { return self.evalAwait(); })
        .def("eval_await", [](kp::Sequence& self, uint32_t wait) { return self.evalAwait(wait); })
        .def("is_recording", &kp::Sequence::isRecording)
        .def("is_running", &kp::Sequence::isRunning)
        .def("is_init", &kp::Sequence::isInit)
        .def("clear", &kp::Sequence::clear)
        .def("destroy", &kp::Sequence::destroy);

    py::class_<kp::Manager, std::shared_ptr<kp::Manager>>(m, "Manager")
        .def(py::init())
        .def(py::init<uint32_t>())
        .def(py::init<uint32_t,const std::vector<uint32_t>&,const std::vector<std::string>&>())
        .def("sequence", &kp::Manager::sequence, py::arg("queueIndex") = 0)
        .def("tensor", [np](kp::Manager& self,
                            const py::array_t<float> data,
                            kp::Tensor::TensorTypes tensor_type) {
                const py::array_t<float> flatdata = np.attr("ravel")(data);
                const py::buffer_info info        = flatdata.request();
                const float* ptr                  = (float*) info.ptr;
                return self.tensor(std::vector<float>(ptr, ptr+flatdata.size()), tensor_type);
            },
            "Tensor initialisation function with data and tensor type",
            py::arg("data"), py::arg("tensor_type") = kp::Tensor::TensorTypes::eDevice)
        .def("algorithm", [](kp::Manager& self,
                             const std::vector<std::shared_ptr<kp::Tensor>>& tensors,
                             const py::bytes& spirv,
                             const kp::Workgroup& workgroup,
                             const kp::Constants& spec_consts,
                             const kp::Constants& push_consts) {
                    py::buffer_info info(py::buffer(spirv).request());
                    const char *data = reinterpret_cast<const char *>(info.ptr);
                    size_t length = static_cast<size_t>(info.size);
                    std::vector<uint32_t> spirvVec((uint32_t*)data, (uint32_t*)(data + length));
                    return self.algorithm(tensors, spirvVec, workgroup, spec_consts, push_consts);
                },
            "Algorithm initialisation function",
            py::arg("tensors"), py::arg("spirv"), py::arg("workgroup") = kp::Workgroup(), py::arg("spec_consts") = kp::Constants(), py::arg("push_consts") = kp::Constants());

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}
