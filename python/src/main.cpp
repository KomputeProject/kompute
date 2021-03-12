#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

#include <kompute/Kompute.hpp>

#include "fmt/ranges.h"

#include "docstrings.hpp"

namespace py = pybind11;
using namespace pybind11::literals; // for the `_a` literal

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
        .value("device", kp::Tensor::TensorTypes::eDevice, DOC(kp, Tensor, TensorTypes, eDevice))
        .value("host", kp::Tensor::TensorTypes::eHost, DOC(kp, Tensor, TensorTypes, eHost))
        .value("storage", kp::Tensor::TensorTypes::eStorage, DOC(kp, Tensor, TensorTypes, eStorage))
        .export_values();

#if !defined(KOMPUTE_DISABLE_SHADER_UTILS) || !KOMPUTE_DISABLE_SHADER_UTILS
    py::class_<kp::Shader>(m, "Shader", "Shader class")
        .def_static("compile_source", [](
                                    const std::string& source,
                                    const std::string& entryPoint,
                                    const std::vector<std::pair<std::string,std::string>>& definitions) {
                std::vector<uint32_t> spirv = kp::Shader::compileSource(source, entryPoint, definitions);
                return py::bytes((const char*)spirv.data(), spirv.size() * sizeof(uint32_t));
            },
            DOC(kp, Shader, compileSource),
            py::arg("source"),
            py::arg("entryPoint") = "main",
            py::arg("definitions") = std::vector<std::pair<std::string,std::string>>() )
        .def_static("compile_sources", [](
                                    const std::vector<std::string>& source,
                                    const std::vector<std::string>& files,
                                    const std::string& entryPoint,
                                    const std::vector<std::pair<std::string,std::string>>& definitions) {
                std::vector<uint32_t> spirv = kp::Shader::compileSources(source, files, entryPoint, definitions);
                return py::bytes((const char*)spirv.data(), spirv.size() * sizeof(uint32_t));
            },
            DOC(kp, Shader, compileSources),
            py::arg("sources"),
            py::arg("files") = std::vector<std::string>(),
            py::arg("entryPoint") = "main",
            py::arg("definitions") = std::vector<std::pair<std::string,std::string>>() );
#endif // KOMPUTE_DISABLE_SHADER_UTILS

    py::class_<kp::OpBase, std::shared_ptr<kp::OpBase>>(m, "OpBase", DOC(kp, OpBase));

    py::class_<kp::OpTensorSyncDevice, std::shared_ptr<kp::OpTensorSyncDevice>>(
            m, "OpTensorSyncDevice", py::base<kp::OpBase>(), DOC(kp, OpTensorSyncDevice))
        .def(py::init<const std::vector<std::shared_ptr<kp::Tensor>>&>(), DOC(kp, OpTensorSyncDevice, OpTensorSyncDevice));

    py::class_<kp::OpTensorSyncLocal, std::shared_ptr<kp::OpTensorSyncLocal>>(
            m, "OpTensorSyncLocal", py::base<kp::OpBase>(), DOC(kp, OpTensorSyncLocal))
        .def(py::init<const std::vector<std::shared_ptr<kp::Tensor>>&>(), DOC(kp, OpTensorSyncLocal, OpTensorSyncLocal));

    py::class_<kp::OpTensorCopy, std::shared_ptr<kp::OpTensorCopy>>(
            m, "OpTensorCopy", py::base<kp::OpBase>(), DOC(kp, OpTensorCopy))
        .def(py::init<const std::vector<std::shared_ptr<kp::Tensor>>&>(), DOC(kp, OpTensorCopy, OpTensorCopy));

    py::class_<kp::OpAlgoDispatch, std::shared_ptr<kp::OpAlgoDispatch>>(
            m, "OpAlgoDispatch", py::base<kp::OpBase>(), DOC(kp, OpAlgoDispatch))
        .def(py::init<const std::shared_ptr<kp::Algorithm>&,const kp::Constants&>(),
                DOC(kp, OpAlgoDispatch, OpAlgoDispatch),
                py::arg("algorithm"), py::arg("push_consts") = kp::Constants());

    py::class_<kp::OpMult, std::shared_ptr<kp::OpMult>>(
            m, "OpMult", py::base<kp::OpBase>(), DOC(kp, OpMult))
        .def(py::init<const std::vector<std::shared_ptr<kp::Tensor>>&,const std::shared_ptr<kp::Algorithm>&>(),
                DOC(kp, OpMult, OpMult));

    py::class_<kp::Algorithm, std::shared_ptr<kp::Algorithm>>(m, "Algorithm", DOC(kp, Algorithm, Algorithm))
        .def("get_tensors", &kp::Algorithm::getTensors, DOC(kp, Algorithm, getTensors))
        .def("destroy", &kp::Algorithm::destroy, DOC(kp, Algorithm, destroy))
        .def("get_spec_consts", &kp::Algorithm::getSpecializationConstants, DOC(kp, Algorithm, getSpecializationConstants))
        .def("is_init", &kp::Algorithm::isInit, DOC(kp, Algorithm, isInit));

    py::class_<kp::Tensor, std::shared_ptr<kp::Tensor>>(m, "Tensor", DOC(kp, Tensor))
        .def("data", [](kp::Tensor& self) {
                // Non-owning container exposing the underlying pointer
                py::str dummyDataOwner; // Explicitly request data to not be owned by np
                switch (self.dataType()) {
                case kp::Tensor::TensorDataTypes::eFloat:
                    return py::array(self.size(), self.data<float>(), dummyDataOwner);
                case kp::Tensor::TensorDataTypes::eUnsignedInt:
                    return py::array(self.size(), self.data<uint32_t>(), dummyDataOwner);
                case kp::Tensor::TensorDataTypes::eInt:
                    return py::array(self.size(), self.data<int32_t>(), dummyDataOwner);
                case kp::Tensor::TensorDataTypes::eDouble:
                    return py::array(self.size(), self.data<double>(), dummyDataOwner);
                case kp::Tensor::TensorDataTypes::eBool:
                    return py::array(self.size(), self.data<bool>(), dummyDataOwner);
                default:
                    throw std::runtime_error("Kompute Python data type not supported");
                }
            }, DOC(kp, Tensor, data))
        .def("size", &kp::Tensor::size, DOC(kp, Tensor, size))
        .def("__len__", &kp::Tensor::size, DOC(kp, Tensor, size))
        .def("tensor_type", &kp::Tensor::tensorType, DOC(kp, Tensor, tensorType))
        .def("data_type", &kp::Tensor::dataType, DOC(kp, Tensor, dataType))
        .def("is_init", &kp::Tensor::isInit, DOC(kp, Tensor, isInit))
        .def("destroy", &kp::Tensor::destroy, DOC(kp, Tensor, destroy));

    py::class_<kp::Sequence, std::shared_ptr<kp::Sequence>>(m, "Sequence")
        .def("record", [](kp::Sequence& self, std::shared_ptr<kp::OpBase> op) { return self.record(op); },
                DOC(kp, Sequence, record))
        .def("eval", [](kp::Sequence& self) { return self.eval(); },
                DOC(kp, Sequence, eval))
        .def("eval", [](kp::Sequence& self, std::shared_ptr<kp::OpBase> op) { return self.eval(op); },
                DOC(kp, Sequence, eval_2))
        .def("eval_async", [](kp::Sequence& self) { return self.eval(); },
                DOC(kp, Sequence, evalAwait))
        .def("eval_async", [](kp::Sequence& self, std::shared_ptr<kp::OpBase> op) { return self.evalAsync(op); },
                DOC(kp, Sequence, evalAsync))
        .def("eval_await", [](kp::Sequence& self) { return self.evalAwait(); },
                DOC(kp, Sequence, evalAwait))
        .def("eval_await", [](kp::Sequence& self, uint32_t wait) { return self.evalAwait(wait); },
                DOC(kp, Sequence, evalAwait))
        .def("is_recording", &kp::Sequence::isRecording,
                DOC(kp, Sequence, isRecording))
        .def("is_running", &kp::Sequence::isRunning,
                DOC(kp, Sequence, isRunning))
        .def("is_init", &kp::Sequence::isInit,
                DOC(kp, Sequence, isInit))
        .def("clear", &kp::Sequence::clear,
                DOC(kp, Sequence, clear))
        .def("rerecord", &kp::Sequence::rerecord,
                DOC(kp, Sequence, rerecord))
        .def("get_timestamps", &kp::Sequence::getTimestamps,
            DOC(kp, Sequence, getTimestamps))
        .def("destroy", &kp::Sequence::destroy,
                DOC(kp, Sequence, destroy));

    py::class_<kp::Manager, std::shared_ptr<kp::Manager>>(m, "Manager", DOC(kp, Manager))
        .def(py::init(), DOC(kp, Manager, Manager))
        .def(py::init<uint32_t>(), DOC(kp, Manager, Manager_2))
        .def(py::init<uint32_t,const std::vector<uint32_t>&,const std::vector<std::string>&>(),
                DOC(kp, Manager, Manager_2),
                py::arg("device") = 0,
                py::arg("family_queue_indices") = std::vector<uint32_t>(),
                py::arg("desired_extensions") = std::vector<std::string>())
        .def("sequence", &kp::Manager::sequence, DOC(kp, Manager, sequence),
                py::arg("queue_index") = 0, py::arg("total_timestamps") = 0)
        .def("tensor", [np](kp::Manager& self,
                            const py::array_t<float>& data,
                            kp::Tensor::TensorTypes tensor_type) {
                const py::array_t<float>& flatdata = np.attr("ravel")(data);
                const py::buffer_info info        = flatdata.request();
                KP_LOG_DEBUG("Kompute Python Manager tensor() creating tensor float with data size {}", flatdata.size());
                return self.tensor(
                        info.ptr,
                        flatdata.size(),
                        sizeof(float),
                        kp::Tensor::TensorDataTypes::eFloat,
                        tensor_type);
            },
            DOC(kp, Manager, tensor),
            py::arg("data"), py::arg("tensor_type") = kp::Tensor::TensorTypes::eDevice)
        .def("tensor_t", [np](kp::Manager& self,
                            const py::array& data,
                            kp::Tensor::TensorTypes tensor_type) {
                // TODO: Suppport strides in numpy format
                const py::array& flatdata = np.attr("ravel")(data);
                const py::buffer_info info        = flatdata.request();
                KP_LOG_DEBUG("Kompute Python Manager creating tensor_T with data size {} dtype {}",
                        flatdata.size(), std::string(py::str(flatdata.dtype())));
                if (flatdata.dtype() == py::dtype::of<std::float_t>()) {
                    return self.tensor(
                            info.ptr, flatdata.size(), sizeof(float), kp::Tensor::TensorDataTypes::eFloat, tensor_type);
                } else if (flatdata.dtype() == py::dtype::of<std::uint32_t>()) {
                    return self.tensor(
                            info.ptr, flatdata.size(), sizeof(uint32_t), kp::Tensor::TensorDataTypes::eUnsignedInt, tensor_type);
                } else if (flatdata.dtype() == py::dtype::of<std::int32_t>()) {
                    return self.tensor(
                            info.ptr, flatdata.size(), sizeof(int32_t), kp::Tensor::TensorDataTypes::eInt, tensor_type);
                } else if (flatdata.dtype() == py::dtype::of<std::double_t>()) {
                    return self.tensor(
                            info.ptr, flatdata.size(), sizeof(double), kp::Tensor::TensorDataTypes::eDouble, tensor_type);
                } else if (flatdata.dtype() == py::dtype::of<bool>()) {
                    return self.tensor(
                            info.ptr, flatdata.size(), sizeof(bool), kp::Tensor::TensorDataTypes::eBool, tensor_type);
                } else {
                    throw std::runtime_error("Kompute Python no valid dtype supported");
                }
            },
            DOC(kp, Manager, tensorT),
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
            DOC(kp, Manager, algorithm),
            py::arg("tensors"),
            py::arg("spirv"),
            py::arg("workgroup") = kp::Workgroup(),
            py::arg("spec_consts") = kp::Constants(),
            py::arg("push_consts") = kp::Constants())
        .def("get_device_properties", [](kp::Manager& self){
            const auto properties = self.getDeviceProperties();
            py::dict py_props(
                "device_name"_a = std::string(properties.deviceName.data()),
                "max_work_group_count"_a       = py::make_tuple(properties.limits.maxComputeWorkGroupCount[0],
                                                                properties.limits.maxComputeWorkGroupCount[1],
                                                                properties.limits.maxComputeWorkGroupCount[2]),
                "max_work_group_invocations"_a = properties.limits.maxComputeWorkGroupInvocations,
                "max_work_group_size"_a        = py::make_tuple(properties.limits.maxComputeWorkGroupSize[0],
                                                                properties.limits.maxComputeWorkGroupSize[1],
                                                                properties.limits.maxComputeWorkGroupSize[2]),
                "timestamps_supported"_a       = (bool)properties.limits.timestampComputeAndGraphics
            );

            return py_props;
        }, "Return a dict containing information about the device");


#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}
