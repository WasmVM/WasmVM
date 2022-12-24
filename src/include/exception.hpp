#ifndef WASMVM_EXCEPTION_DEF
#define WASMVM_EXCEPTION_DEF

#include <ios>
#include <exception>

namespace WasmVM {
namespace exception{
    using eof = std::ios::failure;
}
}


#endif