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
    // The Store-wide identity of this function's declared type. A module
    // function is stamped at instantiation; a host function registered through
    // the CLI's sysenv reg_func is stamped there via canonicalize_functype.
    // `index_npos` means no canonical identity was assigned -- a host function
    // an embedder registers directly, which must
    // canonicalize its own type if it wants canonical matching.
    CanonId canon = index_npos;
};

}

#endif