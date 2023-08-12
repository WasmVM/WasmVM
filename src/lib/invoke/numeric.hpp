#ifndef WASMVM_PP_INVOKE_numeric_DEF
#define WASMVM_PP_INVOKE_numeric_DEF

#include <utility>
#include <instances/Stack.hpp>

namespace WasmVM {

template<typename T>
T get_op(Stack& stack);

template<typename T>
std::pair<T, T> get_ops(Stack& stack);

void put_op(Stack& stack, Value&& value);

}

#endif