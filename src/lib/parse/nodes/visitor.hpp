#ifndef WASMVM_PP_PARSE_NODES_VISITOR_DEF
#define WASMVM_PP_PARSE_NODES_VISITOR_DEF

#include <structures/WasmModule.hpp>
#include "../syntax.hpp"

namespace WasmVM {

struct ModuleVisitor {
    ModuleVisitor(WasmModule& module) : module(module){}
    WasmModule& module;

    void operator()(Syntax::FuncType& type);
    void operator()(Parse::Import& import);
};

struct ImportVisitor {
    ImportVisitor(Parse::Import& import) : import(import){}
    Parse::Import& import;

    void operator()(Syntax::ImportDesc::Func& desc);
};

}

#endif