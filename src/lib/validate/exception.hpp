#ifndef WASMVM_PP_VALIDATE_EXCEPTION_DEF
#define WASMVM_PP_VALIDATE_EXCEPTION_DEF

#include <exception.hpp>

namespace WasmVM {

namespace Exception {

struct Validate_index_not_found : public Exception {
    Validate_index_not_found(std::string message) : Exception(message){};
};
struct Validate_empty_stack : public Exception {
    Validate_empty_stack(std::string message) : Exception(message){};
};
struct Validate_type_not_match : public Exception {
    Validate_type_not_match(std::string message) : Exception(message){};
};
struct Validate_invalid_type : public Exception {
    Validate_invalid_type(std::string message) : Exception(message){};
};
struct Validate_invalid_operation : public Exception {
    Validate_invalid_operation(std::string message) : Exception(message){};
};
}
}
#endif