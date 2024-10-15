#ifndef WASMVM_Parse_Visitor
#define WASMVM_Parse_Visitor

#include <WatBaseVisitor.h>
#include <WasmVM.hpp>

namespace WasmVM {

struct Visitor : WatBaseVisitor {

    WasmModule module;

    virtual std::any visitModule(WatParser::ModuleContext *context);

};

}

#endif