#ifndef WASMVM_PP_DECODE_EXCEPTION_DEF
#define WASMVM_PP_DECODE_EXCEPTION_DEF

#include <exception.hpp>

namespace WasmVM {
namespace Exception {
    struct incorrect_magic : public Decode {
        incorrect_magic();
    };
    struct invalid_functype : public Decode {
        invalid_functype(size_t location);
    };
    struct invalid_valuetype : public Decode {
        invalid_valuetype(size_t location);
    };
} // namespace Exception
} // namespace WasmVM

#endif