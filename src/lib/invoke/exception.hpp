#ifndef WASMVM_PP_INVOKE_EXCEPTION_DEF
#define WASMVM_PP_INVOKE_EXCEPTION_DEF

#include <exception.hpp>
#include <Types.hpp>
#include <memory>

namespace WasmVM {

// C++ exception used for Wasm exception propagation
struct WasmThrow {
    std::shared_ptr<WasmException> exn;
    WasmThrow(std::shared_ptr<WasmException> e) : exn(std::move(e)) {}
};

namespace Exception {
    struct invalid_argument : public Exception {
        invalid_argument();
    };
    struct invalid_results : public Exception {
        invalid_results();
    };
    struct unreachable : public Exception {
        unreachable();
    };
    struct no_end_of_func : public Exception {
        no_end_of_func();
    };
    struct func_not_exist : public Exception {
        func_not_exist();
    };
    struct invalid_label : public Exception {
        invalid_label();
    };
    struct invalid_elem_index : public Exception {
        invalid_elem_index();
    };
    struct invalid_reference : public Exception {
        invalid_reference();
    };
    struct invalid_value : public Exception {
        invalid_value();
    };
    struct function_type_unmatch : public Exception {
        function_type_unmatch();
    };
    struct length_too_long : public Exception {
        length_too_long();
    };
    struct cast_failure : public Exception {
        cast_failure();
    };
    struct null_reference : public Exception {
        null_reference();
    };
    struct invalid_conversion_to_integer : public Exception {
        invalid_conversion_to_integer();
    };
}
}

#endif