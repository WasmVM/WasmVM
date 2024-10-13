#ifndef WASMVM_Parse_Visitor
#define WASMVM_Parse_Visitor

#include <WatVisitor.h>
#include <WasmVM.hpp>

namespace WasmVM {

struct Visitor : WatVisitor {

    WasmModule module;

    virtual std::any visitModule(WatParser::ModuleContext *context);

    virtual std::any visitModulefield(WatParser::ModulefieldContext *context);

    virtual std::any visitType(WatParser::TypeContext *context);

    virtual std::any visitValtype(WatParser::ValtypeContext *context);

    virtual std::any visitParam(WatParser::ParamContext *context);

    virtual std::any visitResult(WatParser::ResultContext *context);

    virtual std::any visitFunctype(WatParser::FunctypeContext *context);

    virtual std::any visitLimits(WatParser::LimitsContext *context);

    virtual std::any visitMemtype(WatParser::MemtypeContext *context);

    virtual std::any visitTabletype(WatParser::TabletypeContext *context);

    virtual std::any visitGlobaltype(WatParser::GlobaltypeContext *context);
};

}

#endif