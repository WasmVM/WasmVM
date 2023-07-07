#ifndef WASMVM_PP_INSTANCES_FuncInst_DEF
#define WASMVM_PP_INSTANCES_FuncInst_DEF

#include <Types.hpp>
#include <structures/WasmFunc.hpp>
#include <variant>
#include <list>

#include "ModuleInst.hpp"

namespace WasmVM {

struct Stack;

using hostfunc_t = std::vector<Value>(*)(Stack&);

struct FuncInst {

    struct Body {
        Body(ModuleInst& module) : module(module){}
        ModuleInst& module;
        WasmFunc func;
    };

    FuncType type;
    std::variant<hostfunc_t, Body> body;
};

}

#endif