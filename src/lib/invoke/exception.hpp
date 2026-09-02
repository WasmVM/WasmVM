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
    // Exception::interrupted lives in the public <exception.hpp>.
    // Leaves deriving from Trap are the conditions the spec calls traps;
    // the rest are VM invariants validation should have made unreachable.
    struct invalid_argument : public Exception {
        invalid_argument();
    };
    struct invalid_results : public Exception {
        invalid_results();
    };
    struct unreachable : public Trap {
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
    struct invalid_elem_index : public Trap {
        invalid_elem_index();
    };
    struct invalid_reference : public Exception {
        invalid_reference();
    };
    // call_indirect on a table slot holding a null funcref (spec:
    // "uninitialized element").
    struct uninitialized_element : public Trap {
        uninitialized_element();
    };
    struct invalid_value : public Exception {
        invalid_value();
    };
    struct function_type_unmatch : public Trap {
        function_type_unmatch();
    };
    struct length_too_long : public Trap {
        length_too_long();
    };
    struct cast_failure : public Trap {
        cast_failure();
    };
    struct null_reference : public Trap {
        null_reference();
    };
    struct invalid_conversion_to_integer : public Trap {
        invalid_conversion_to_integer();
    };
    // i{32,64}.{div,rem}_{s,u} with a zero divisor.
    struct integer_divide_by_zero : public Trap {
        integer_divide_by_zero();
    };
    // i{32,64}.div_s of INT_MIN by -1, whose quotient is not representable.
    struct integer_overflow : public Trap {
        integer_overflow();
    };
}
}

#endif