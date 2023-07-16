#ifndef WASMVM_PP_INVOKE_EXCEPTION_DEF
#define WASMVM_PP_INVOKE_EXCEPTION_DEF

#include <exception.hpp>

namespace WasmVM {
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
}
}

#endif