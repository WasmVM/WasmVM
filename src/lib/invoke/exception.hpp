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
}
}

#endif