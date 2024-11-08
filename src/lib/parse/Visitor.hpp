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
    std::map<std::string, index_t> elem_map;
    std::map<std::string, index_t> data_map;
    std::map<std::string, index_t> local_map;
    std::map<std::string, index_t> label_map;
    IndexSpace func_map;
    IndexSpace table_map;
    IndexSpace mem_map;
    IndexSpace global_map;
    std::vector<std::pair<FuncType, std::map<std::string, index_t>>> types;
    index_t block_level = 0;

    virtual std::any visitModule(WatParser::ModuleContext *ctx) override;
    virtual std::any visitModulefield(WatParser::ModulefieldContext *ctx) override;
    virtual std::any visitValtype(WatParser::ValtypeContext *ctx) override;
    virtual std::any visitFunctype(WatParser::FunctypeContext *ctx) override;
    virtual std::any visitTabletype(WatParser::TabletypeContext *ctx) override;
    virtual std::any visitMemtype(WatParser::MemtypeContext *ctx) override;
    virtual std::any visitGlobaltype(WatParser::GlobaltypeContext *ctx) override;
    virtual std::any visitParam(WatParser::ParamContext *ctx) override;
    virtual std::any visitResult(WatParser::ResultContext *ctx) override;
    virtual std::any visitLocal(WatParser::LocalContext *ctx) override;
    virtual std::any visitTypeuse(WatParser::TypeuseContext *ctx) override;
    virtual std::any visitTableuse(WatParser::TableuseContext *ctx) override;
    virtual std::any visitMemuse(WatParser::MemuseContext *ctx) override;
    virtual std::any visitLimits(WatParser::LimitsContext *ctx) override;
    virtual std::any visitElemlist(WatParser::ElemlistContext *ctx) override;
    virtual std::any visitElemexpr(WatParser::ElemexprContext *ctx) override;
    virtual std::any visitConstexpr(WatParser::ConstexprContext *ctx) override;
    virtual std::any visitImportabbr(WatParser::ImportabbrContext *ctx) override;
    virtual std::any visitImportdesc(WatParser::ImportdescContext *ctx) override;
    virtual std::any visitExportabbr(WatParser::ExportabbrContext *ctx) override;
    virtual std::any visitExportdesc(WatParser::ExportdescContext *ctx) override;
    virtual std::any visitTypeidx(WatParser::TypeidxContext *ctx) override;
    virtual std::any visitFuncidx(WatParser::FuncidxContext *ctx) override;
    virtual std::any visitTableidx(WatParser::TableidxContext *ctx) override;
    virtual std::any visitMemidx(WatParser::MemidxContext *ctx) override;
    virtual std::any visitGlobalidx(WatParser::GlobalidxContext *ctx) override;
    virtual std::any visitElemidx(WatParser::ElemidxContext *ctx) override;
    virtual std::any visitDataidx(WatParser::DataidxContext *ctx) override;
    virtual std::any visitLocalidx(WatParser::LocalidxContext *ctx) override;
    virtual std::any visitLabelidx(WatParser::LabelidxContext *ctx) override;
    virtual std::any visitI32(WatParser::I32Context *ctx) override;
    virtual std::any visitI64(WatParser::I64Context *ctx) override;
    virtual std::any visitU32(WatParser::U32Context *ctx) override;
    virtual std::any visitU64(WatParser::U64Context *ctx) override;
    virtual std::any visitF32(WatParser::F32Context *ctx) override;
    virtual std::any visitF64(WatParser::F64Context *ctx) override;
    virtual std::any visitTypesection(WatParser::TypesectionContext *ctx) override;
    virtual std::any visitImportsection(WatParser::ImportsectionContext *ctx) override;
    virtual std::any visitFuncsection(WatParser::FuncsectionContext *ctx) override;
    virtual std::any visitTablesection(WatParser::TablesectionContext *ctx) override;
    virtual std::any visitMemorysection(WatParser::MemorysectionContext *ctx) override;
    virtual std::any visitGlobalsection(WatParser::GlobalsectionContext *ctx) override;
    virtual std::any visitExportsection(WatParser::ExportsectionContext *ctx) override;
    virtual std::any visitStartsection(WatParser::StartsectionContext *ctx) override;
    virtual std::any visitElemsection(WatParser::ElemsectionContext *ctx) override;
    virtual std::any visitDatasection(WatParser::DatasectionContext *ctx) override;

    virtual std::any visitInstr(WatParser::InstrContext *ctx) override;
    virtual std::any visitElseinstr(WatParser::ElseinstrContext *ctx) override;
    virtual std::any visitPlaininstr(WatParser::PlaininstrContext *ctx) override;
    virtual std::any visitControlinstr(WatParser::ControlinstrContext *ctx) override;
    virtual std::any visitLabel(WatParser::LabelContext *ctx) override;
    virtual std::any visitBlockinstr(WatParser::BlockinstrContext *ctx) override;

    void post_process();
    std::unordered_map<index_t, index_t> reorder_map(IndexSpace& space);

public:
    WasmModule visit(WatParser::ModuleContext *ctx);
};

std::pair<size_t, size_t> getLocation(antlr4::tree::TerminalNode* node);

}

#endif