
// Generated from /Users/luishsu/Desktop/WasmVM/src/lib/parse/Wat.g4 by ANTLR 4.13.2

#pragma once


#include <antlr4-runtime.h>
#include "WatParser.h"


namespace WasmVM {

/**
 * This class defines an abstract visitor for a parse tree
 * produced by WatParser.
 */
class  WatVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by WatParser.
   */
    virtual std::any visitModule(WatParser::ModuleContext *context) = 0;

    virtual std::any visitModulefield(WatParser::ModulefieldContext *context) = 0;

    virtual std::any visitTypesection(WatParser::TypesectionContext *context) = 0;

    virtual std::any visitImportdesc(WatParser::ImportdescContext *context) = 0;

    virtual std::any visitImportabbr(WatParser::ImportabbrContext *context) = 0;

    virtual std::any visitImportsection(WatParser::ImportsectionContext *context) = 0;

    virtual std::any visitFuncsection(WatParser::FuncsectionContext *context) = 0;

    virtual std::any visitLocal(WatParser::LocalContext *context) = 0;

    virtual std::any visitTablesection(WatParser::TablesectionContext *context) = 0;

    virtual std::any visitMemorysection(WatParser::MemorysectionContext *context) = 0;

    virtual std::any visitGlobalsection(WatParser::GlobalsectionContext *context) = 0;

    virtual std::any visitExportsection(WatParser::ExportsectionContext *context) = 0;

    virtual std::any visitExportdesc(WatParser::ExportdescContext *context) = 0;

    virtual std::any visitExportabbr(WatParser::ExportabbrContext *context) = 0;

    virtual std::any visitStartsection(WatParser::StartsectionContext *context) = 0;

    virtual std::any visitElemsection(WatParser::ElemsectionContext *context) = 0;

    virtual std::any visitElemlist(WatParser::ElemlistContext *context) = 0;

    virtual std::any visitElemexpr(WatParser::ElemexprContext *context) = 0;

    virtual std::any visitTableuse(WatParser::TableuseContext *context) = 0;

    virtual std::any visitDatasection(WatParser::DatasectionContext *context) = 0;

    virtual std::any visitMemuse(WatParser::MemuseContext *context) = 0;

    virtual std::any visitI32(WatParser::I32Context *context) = 0;

    virtual std::any visitI64(WatParser::I64Context *context) = 0;

    virtual std::any visitU32(WatParser::U32Context *context) = 0;

    virtual std::any visitU64(WatParser::U64Context *context) = 0;

    virtual std::any visitF32(WatParser::F32Context *context) = 0;

    virtual std::any visitF64(WatParser::F64Context *context) = 0;

    virtual std::any visitValtype(WatParser::ValtypeContext *context) = 0;

    virtual std::any visitParam(WatParser::ParamContext *context) = 0;

    virtual std::any visitResult(WatParser::ResultContext *context) = 0;

    virtual std::any visitFunctype(WatParser::FunctypeContext *context) = 0;

    virtual std::any visitLimits(WatParser::LimitsContext *context) = 0;

    virtual std::any visitMemtype(WatParser::MemtypeContext *context) = 0;

    virtual std::any visitTabletype(WatParser::TabletypeContext *context) = 0;

    virtual std::any visitGlobaltype(WatParser::GlobaltypeContext *context) = 0;

    virtual std::any visitTypeidx(WatParser::TypeidxContext *context) = 0;

    virtual std::any visitFuncidx(WatParser::FuncidxContext *context) = 0;

    virtual std::any visitTableidx(WatParser::TableidxContext *context) = 0;

    virtual std::any visitMemidx(WatParser::MemidxContext *context) = 0;

    virtual std::any visitGlobalidx(WatParser::GlobalidxContext *context) = 0;

    virtual std::any visitElemidx(WatParser::ElemidxContext *context) = 0;

    virtual std::any visitDataidx(WatParser::DataidxContext *context) = 0;

    virtual std::any visitLocalidx(WatParser::LocalidxContext *context) = 0;

    virtual std::any visitLabelidx(WatParser::LabelidxContext *context) = 0;

    virtual std::any visitTypeuse(WatParser::TypeuseContext *context) = 0;

    virtual std::any visitInstr(WatParser::InstrContext *context) = 0;

    virtual std::any visitLabel(WatParser::LabelContext *context) = 0;

    virtual std::any visitBlocktype(WatParser::BlocktypeContext *context) = 0;

    virtual std::any visitBlockinstr(WatParser::BlockinstrContext *context) = 0;

    virtual std::any visitFoldedinstr(WatParser::FoldedinstrContext *context) = 0;

    virtual std::any visitConstexpr(WatParser::ConstexprContext *context) = 0;

    virtual std::any visitPlaininstr(WatParser::PlaininstrContext *context) = 0;

    virtual std::any visitControlinstr(WatParser::ControlinstrContext *context) = 0;

    virtual std::any visitReferenceinstr(WatParser::ReferenceinstrContext *context) = 0;

    virtual std::any visitParametricinstr(WatParser::ParametricinstrContext *context) = 0;

    virtual std::any visitVariableinstr(WatParser::VariableinstrContext *context) = 0;

    virtual std::any visitTableinstr(WatParser::TableinstrContext *context) = 0;

    virtual std::any visitMemarg(WatParser::MemargContext *context) = 0;

    virtual std::any visitMemoryinstr(WatParser::MemoryinstrContext *context) = 0;

    virtual std::any visitNumericinstr(WatParser::NumericinstrContext *context) = 0;


};

}  // namespace WasmVM
