#ifndef WASMVM_PP_INSTANCES_FuncInst_DEF
#define WASMVM_PP_INSTANCES_FuncInst_DEF

#include <Types.hpp>
#include <structures/WasmFunc.hpp>
#include <variant>
#include <optional>
#include <functional>

#include "ModuleInst.hpp"

namespace WasmVM {

struct Stack;

using hostfunc_t = std::vector<Value>(*)(Stack&);

struct FuncInst {
    FuncType type;
    std::optional<std::reference_wrapper<ModuleInst>> module;
    std::variant<hostfunc_t, WasmFunc> body;
};

}

#endif