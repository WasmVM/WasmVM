#ifndef WASMVM_PP_DECODE_EXCEPTION_DEF
#define WASMVM_PP_DECODE_EXCEPTION_DEF

#include <exception.hpp>

namespace WasmVM {
namespace Exception {
    struct incorrect_magic : public Decode {
        incorrect_magic();
    };
} // namespace Exception
} // namespace WasmVM

#endif