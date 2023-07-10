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
    FuncInst(ModuleInst& module) : module(module){}
    FuncType type;
    ModuleInst& module;
    std::variant<hostfunc_t, WasmFunc> body;
};

}

#endif