#ifndef WASMVM_EXCEPTION_DEF
#define WASMVM_EXCEPTION_DEF

#include <ostream>
#include <exception>
#include <utility>

#include <Util.hpp>

namespace WasmVM {
namespace Exception{

    struct Exception : public std::runtime_error {
        Exception(std::string msg) : std::runtime_error(msg) {}
    };

    struct Parse : public Exception {
        Parse(std::string msg, std::pair<size_t, size_t> location) : Exception(msg), location(location) {}
        std::pair<size_t, size_t> location;
    };

}
}

#undef define_exception

#endif