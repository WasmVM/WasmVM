#ifndef WASMVM_PP_INVOKE_EXCEPTION_DEF
#define WASMVM_PP_INVOKE_EXCEPTION_DEF

#include <exception.hpp>
#include <Types.hpp>
#include <memory>

namespace WasmVM {

// C++ exception used for Wasm exception propagation
struct WasmThrow {
    GcPtr<WasmException> exn;
    WasmThrow(GcPtr<WasmException> e) : exn(std::move(e)) {}
};

namespace Exception {
    // Exception::interrupted lives in the public <exception.hpp>.
    // Leaves deriving from Trap are the conditions the spec calls traps;
    // the rest are VM invariants validation should have made unreachable.
    struct invalid_argument : public Exception {
        invalid_argument();
        ~invalid_argument() override;
    };
    struct invalid_results : public Exception {
        invalid_results();
        ~invalid_results() override;
    };
    struct unreachable : public Trap {
        unreachable();
        ~unreachable() override;
    };
    struct no_end_of_func : public Exception {
        no_end_of_func();
        ~no_end_of_func() override;
    };
    struct func_not_exist : public Exception {
        func_not_exist();
        ~func_not_exist() override;
    };
    struct invalid_label : public Exception {
        invalid_label();
        ~invalid_label() override;
    };
    struct invalid_elem_index : public Trap {
        invalid_elem_index();
        ~invalid_elem_index() override;
    };
    struct invalid_reference : public Exception {
        invalid_reference();
        ~invalid_reference() override;
    };
    // call_indirect on a table slot holding a null funcref (spec:
    // "uninitialized element").
    struct uninitialized_element : public Trap {
        uninitialized_element();
        ~uninitialized_element() override;
    };
    struct invalid_value : public Exception {
        invalid_value();
        ~invalid_value() override;
    };
    struct function_type_unmatch : public Trap {
        function_type_unmatch();
        ~function_type_unmatch() override;
    };
    struct length_too_long : public Trap {
        length_too_long();
        ~length_too_long() override;
    };
    struct cast_failure : public Trap {
        cast_failure();
        ~cast_failure() override;
    };
    struct null_reference : public Trap {
        null_reference();
        ~null_reference() override;
    };
    struct invalid_conversion_to_integer : public Trap {
        invalid_conversion_to_integer();
        ~invalid_conversion_to_integer() override;
    };
    // i{32,64}.{div,rem}_{s,u} with a zero divisor.
    struct integer_divide_by_zero : public Trap {
        integer_divide_by_zero();
        ~integer_divide_by_zero() override;
    };
    // i{32,64}.div_s of INT_MIN by -1, whose quotient is not representable.
    struct integer_overflow : public Trap {
        integer_overflow();
        ~integer_overflow() override;
    };
}
}

#endif