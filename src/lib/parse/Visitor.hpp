#ifndef WASMVM_Parse_Visitor
#define WASMVM_Parse_Visitor

#include <parse/WatBaseVisitor.h>
#include <WasmVM.hpp>

#include <map>

namespace WasmVM {

class Visitor : WatBaseVisitor {
protected:
    struct IndexSpace {
        enum class Type {Import, Normal};
        std::vector<Type> records;
        std::map<std::string, index_t> id_map;
    };

    WasmModule module;
    std::map<std::string, index_t> type_map;
    std::map<std::string, index_t> local_map;
    IndexSpace func_map;
    std::vector<std::pair<FuncType, std::map<std::string, index_t>>> types;

    virtual std::any visitModulefield(WatParser::ModulefieldContext *ctx) override;
    virtual std::any visitTypesection(WatParser::TypesectionContext *ctx) override;
    virtual std::any visitValtype(WatParser::ValtypeContext *ctx) override;
    virtual std::any visitFunctype(WatParser::FunctypeContext *ctx) override;
    virtual std::any visitParam(WatParser::ParamContext *ctx) override;
    virtual std::any visitResult(WatParser::ResultContext *ctx) override;
    virtual std::any visitModule(WatParser::ModuleContext *ctx) override;
    virtual std::any visitFuncsection(WatParser::FuncsectionContext *ctx) override;
    virtual std::any visitTypeuse(WatParser::TypeuseContext *ctx) override;
    virtual std::any visitTypeidx(WatParser::TypeidxContext *ctx) override;
    virtual std::any visitU32(WatParser::U32Context *ctx) override;
    virtual std::any visitImportabbr(WatParser::ImportabbrContext *ctx) override;
    virtual std::any visitLocal(WatParser::LocalContext *ctx) override;

public:
    WasmModule visit(WatParser::ModuleContext *ctx);
};

}

#endif