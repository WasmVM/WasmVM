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
    struct invalid_import : public Decode {
        invalid_import(size_t location);
    };
    struct invalid_reftype : public Decode {
        invalid_reftype(size_t location);
    };
    struct invalid_limit : public Decode {
        invalid_limit(size_t location);
    };
    struct invalid_globaltype : public Decode {
        invalid_globaltype(size_t location);
    };
    struct invalid_export : public Decode {
        invalid_export(size_t location);
    };
    struct unknown_instruction : public Decode {
        unknown_instruction(size_t location);
    };
    struct expression_not_end : public Decode {
        expression_not_end(size_t location);
    };
    struct invalid_elem : public Decode {
        invalid_elem(size_t location);
    };
    struct invalid_data : public Decode {
        invalid_data(size_t location);
    };
    struct count_mismatch : public Decode {
        count_mismatch(size_t location);
    };
    struct invalid_instruction : public Decode {
        invalid_instruction(size_t location);
    };
} // namespace Exception
} // namespace WasmVM

#endif