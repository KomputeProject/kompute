#include <pybind11/pybind11.h>
#include "kompute/Kompute.hpp"

namespace py = pybind11;

PYBIND11_MODULE(cmake_example, m) {

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}
