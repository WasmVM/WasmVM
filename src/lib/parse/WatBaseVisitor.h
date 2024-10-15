
// Generated from /Users/luishsu/Desktop/WasmVM/src/lib/parse/Wat.g4 by ANTLR 4.13.2

#pragma once


#include <antlr4-runtime.h>
#include "WatVisitor.h"


namespace WasmVM {

/**
 * This class provides an empty implementation of WatVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  WatBaseVisitor : public WatVisitor {
public:

  virtual std::any visitModule(WatParser::ModuleContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitModulefield(WatParser::ModulefieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTypesection(WatParser::TypesectionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitImportdesc(WatParser::ImportdescContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitImportabbr(WatParser::ImportabbrContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitImportsection(WatParser::ImportsectionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFuncsection(WatParser::FuncsectionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLocal(WatParser::LocalContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTablesection(WatParser::TablesectionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMemorysection(WatParser::MemorysectionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitGlobalsection(WatParser::GlobalsectionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExportsection(WatParser::ExportsectionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExportdesc(WatParser::ExportdescContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExportabbr(WatParser::ExportabbrContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStartsection(WatParser::StartsectionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitElemsection(WatParser::ElemsectionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitElemlist(WatParser::ElemlistContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitElemexpr(WatParser::ElemexprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTableuse(WatParser::TableuseContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDatasection(WatParser::DatasectionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMemuse(WatParser::MemuseContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitI32(WatParser::I32Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitI64(WatParser::I64Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitU32(WatParser::U32Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitU64(WatParser::U64Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitF32(WatParser::F32Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitF64(WatParser::F64Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitValtype(WatParser::ValtypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitParam(WatParser::ParamContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitResult(WatParser::ResultContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunctype(WatParser::FunctypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLimits(WatParser::LimitsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMemtype(WatParser::MemtypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTabletype(WatParser::TabletypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitGlobaltype(WatParser::GlobaltypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTypeidx(WatParser::TypeidxContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFuncidx(WatParser::FuncidxContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTableidx(WatParser::TableidxContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMemidx(WatParser::MemidxContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitGlobalidx(WatParser::GlobalidxContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitElemidx(WatParser::ElemidxContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDataidx(WatParser::DataidxContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLocalidx(WatParser::LocalidxContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLabelidx(WatParser::LabelidxContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTypeuse(WatParser::TypeuseContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInstr(WatParser::InstrContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLabel(WatParser::LabelContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBlocktype(WatParser::BlocktypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBlockinstr(WatParser::BlockinstrContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFoldedinstr(WatParser::FoldedinstrContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConstexpr(WatParser::ConstexprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPlaininstr(WatParser::PlaininstrContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitControlinstr(WatParser::ControlinstrContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitReferenceinstr(WatParser::ReferenceinstrContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitParametricinstr(WatParser::ParametricinstrContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVariableinstr(WatParser::VariableinstrContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTableinstr(WatParser::TableinstrContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMemarg(WatParser::MemargContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMemoryinstr(WatParser::MemoryinstrContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNumericinstr(WatParser::NumericinstrContext *ctx) override {
    return visitChildren(ctx);
  }


};

}  // namespace WasmVM
