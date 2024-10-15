#ifndef WASMVM_Parse_Visitor
#define WASMVM_Parse_Visitor

#include <parse/WatBaseVisitor.h>
#include <WasmVM.hpp>

#include <map>

namespace WasmVM {

class Visitor : WatBaseVisitor {
protected:
    WasmModule module;
    std::map<std::string, index_t> type_map;
    std::vector<std::pair<FuncType, std::map<std::string, index_t>>> types;

    virtual std::any visitModulefield(WatParser::ModulefieldContext *ctx) override;
    virtual std::any visitTypesection(WatParser::TypesectionContext *ctx) override;
    virtual std::any visitValtype(WatParser::ValtypeContext *ctx) override;
    virtual std::any visitFunctype(WatParser::FunctypeContext *ctx) override;
    virtual std::any visitParam(WatParser::ParamContext *ctx) override;
    virtual std::any visitResult(WatParser::ResultContext *ctx) override;
    virtual std::any visitModule(WatParser::ModuleContext *ctx) override;

public:
    WasmModule visit(WatParser::ModuleContext *ctx);
};

}

#endif