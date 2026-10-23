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
    // The Store-wide identity of this function's declared type, stamped at
    // instantiation for a module-defined function. `index_npos` means "no
    // canonical identity": a host function registered by an embedder declares
    // no type index and no supertypes, so a cast against it falls back to
    // structural equality of its FuncType, as before this field existed.
    CanonId canon = index_npos;
};

}

#endif