
// Generated from /Users/luishsu/Desktop/WasmVM/src/lib/parse/Wat.g4 by ANTLR 4.13.2

#pragma once


#include <antlr4-runtime.h>


namespace WasmVM {


class  WatParser : public antlr4::Parser {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, T__6 = 7, 
    T__7 = 8, T__8 = 9, T__9 = 10, T__10 = 11, T__11 = 12, T__12 = 13, T__13 = 14, 
    T__14 = 15, T__15 = 16, T__16 = 17, T__17 = 18, T__18 = 19, T__19 = 20, 
    T__20 = 21, T__21 = 22, T__22 = 23, T__23 = 24, T__24 = 25, T__25 = 26, 
    T__26 = 27, T__27 = 28, T__28 = 29, T__29 = 30, T__30 = 31, T__31 = 32, 
    T__32 = 33, T__33 = 34, T__34 = 35, T__35 = 36, T__36 = 37, T__37 = 38, 
    T__38 = 39, T__39 = 40, T__40 = 41, T__41 = 42, T__42 = 43, T__43 = 44, 
    T__44 = 45, T__45 = 46, T__46 = 47, T__47 = 48, T__48 = 49, T__49 = 50, 
    T__50 = 51, T__51 = 52, T__52 = 53, T__53 = 54, T__54 = 55, T__55 = 56, 
    T__56 = 57, T__57 = 58, T__58 = 59, T__59 = 60, T__60 = 61, T__61 = 62, 
    T__62 = 63, T__63 = 64, T__64 = 65, T__65 = 66, T__66 = 67, T__67 = 68, 
    T__68 = 69, T__69 = 70, T__70 = 71, T__71 = 72, T__72 = 73, T__73 = 74, 
    T__74 = 75, T__75 = 76, T__76 = 77, T__77 = 78, T__78 = 79, T__79 = 80, 
    T__80 = 81, T__81 = 82, T__82 = 83, T__83 = 84, T__84 = 85, T__85 = 86, 
    T__86 = 87, T__87 = 88, T__88 = 89, T__89 = 90, T__90 = 91, T__91 = 92, 
    T__92 = 93, T__93 = 94, T__94 = 95, T__95 = 96, T__96 = 97, T__97 = 98, 
    T__98 = 99, T__99 = 100, T__100 = 101, T__101 = 102, T__102 = 103, T__103 = 104, 
    T__104 = 105, T__105 = 106, T__106 = 107, T__107 = 108, T__108 = 109, 
    T__109 = 110, T__110 = 111, T__111 = 112, T__112 = 113, T__113 = 114, 
    T__114 = 115, T__115 = 116, T__116 = 117, T__117 = 118, T__118 = 119, 
    T__119 = 120, T__120 = 121, T__121 = 122, T__122 = 123, T__123 = 124, 
    T__124 = 125, T__125 = 126, T__126 = 127, T__127 = 128, T__128 = 129, 
    T__129 = 130, T__130 = 131, T__131 = 132, T__132 = 133, T__133 = 134, 
    T__134 = 135, T__135 = 136, T__136 = 137, T__137 = 138, T__138 = 139, 
    T__139 = 140, T__140 = 141, T__141 = 142, T__142 = 143, T__143 = 144, 
    T__144 = 145, T__145 = 146, T__146 = 147, T__147 = 148, T__148 = 149, 
    T__149 = 150, T__150 = 151, T__151 = 152, T__152 = 153, T__153 = 154, 
    T__154 = 155, T__155 = 156, T__156 = 157, T__157 = 158, T__158 = 159, 
    T__159 = 160, T__160 = 161, T__161 = 162, T__162 = 163, T__163 = 164, 
    T__164 = 165, T__165 = 166, T__166 = 167, T__167 = 168, T__168 = 169, 
    T__169 = 170, T__170 = 171, T__171 = 172, T__172 = 173, T__173 = 174, 
    T__174 = 175, T__175 = 176, T__176 = 177, T__177 = 178, T__178 = 179, 
    T__179 = 180, T__180 = 181, T__181 = 182, T__182 = 183, T__183 = 184, 
    T__184 = 185, T__185 = 186, T__186 = 187, T__187 = 188, T__188 = 189, 
    T__189 = 190, T__190 = 191, T__191 = 192, T__192 = 193, T__193 = 194, 
    T__194 = 195, T__195 = 196, T__196 = 197, T__197 = 198, T__198 = 199, 
    T__199 = 200, T__200 = 201, T__201 = 202, T__202 = 203, T__203 = 204, 
    T__204 = 205, T__205 = 206, T__206 = 207, T__207 = 208, T__208 = 209, 
    T__209 = 210, T__210 = 211, T__211 = 212, T__212 = 213, T__213 = 214, 
    T__214 = 215, T__215 = 216, T__216 = 217, T__217 = 218, T__218 = 219, 
    T__219 = 220, BlockComment = 221, LineComment = 222, WhiteSpace = 223, 
    Integer = 224, Float = 225, String = 226, Id = 227, NumType = 228, RefType = 229, 
    RefNull = 230, Offset = 231, Align = 232
  };

  enum {
    RuleModule = 0, RuleModulefield = 1, RuleTypesection = 2, RuleImportdesc = 3, 
    RuleImportabbr = 4, RuleImportsection = 5, RuleFuncsection = 6, RuleLocal = 7, 
    RuleTablesection = 8, RuleMemorysection = 9, RuleGlobalsection = 10, 
    RuleExportsection = 11, RuleExportdesc = 12, RuleExportabbr = 13, RuleStartsection = 14, 
    RuleElemsection = 15, RuleElemlist = 16, RuleElemexpr = 17, RuleTableuse = 18, 
    RuleDatasection = 19, RuleMemuse = 20, RuleI32 = 21, RuleI64 = 22, RuleU32 = 23, 
    RuleU64 = 24, RuleF32 = 25, RuleF64 = 26, RuleValtype = 27, RuleParam = 28, 
    RuleResult = 29, RuleFunctype = 30, RuleLimits = 31, RuleMemtype = 32, 
    RuleTabletype = 33, RuleGlobaltype = 34, RuleTypeidx = 35, RuleFuncidx = 36, 
    RuleTableidx = 37, RuleMemidx = 38, RuleGlobalidx = 39, RuleElemidx = 40, 
    RuleDataidx = 41, RuleLocalidx = 42, RuleLabelidx = 43, RuleTypeuse = 44, 
    RuleInstr = 45, RuleLabel = 46, RuleElseinstr = 47, RuleBlockinstr = 48, 
    RuleFoldedinstr = 49, RuleConstexpr = 50, RulePlaininstr = 51, RuleControlinstr = 52, 
    RuleReferenceinstr = 53, RuleParametricinstr = 54, RuleVariableinstr = 55, 
    RuleTableinstr = 56, RuleMemarg = 57, RuleMemoryinstr = 58, RuleNumericinstr = 59
  };

  explicit WatParser(antlr4::TokenStream *input);

  WatParser(antlr4::TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options);

  ~WatParser() override;

  std::string getGrammarFileName() const override;

  const antlr4::atn::ATN& getATN() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;


  class ModuleContext;
  class ModulefieldContext;
  class TypesectionContext;
  class ImportdescContext;
  class ImportabbrContext;
  class ImportsectionContext;
  class FuncsectionContext;
  class LocalContext;
  class TablesectionContext;
  class MemorysectionContext;
  class GlobalsectionContext;
  class ExportsectionContext;
  class ExportdescContext;
  class ExportabbrContext;
  class StartsectionContext;
  class ElemsectionContext;
  class ElemlistContext;
  class ElemexprContext;
  class TableuseContext;
  class DatasectionContext;
  class MemuseContext;
  class I32Context;
  class I64Context;
  class U32Context;
  class U64Context;
  class F32Context;
  class F64Context;
  class ValtypeContext;
  class ParamContext;
  class ResultContext;
  class FunctypeContext;
  class LimitsContext;
  class MemtypeContext;
  class TabletypeContext;
  class GlobaltypeContext;
  class TypeidxContext;
  class FuncidxContext;
  class TableidxContext;
  class MemidxContext;
  class GlobalidxContext;
  class ElemidxContext;
  class DataidxContext;
  class LocalidxContext;
  class LabelidxContext;
  class TypeuseContext;
  class InstrContext;
  class LabelContext;
  class ElseinstrContext;
  class BlockinstrContext;
  class FoldedinstrContext;
  class ConstexprContext;
  class PlaininstrContext;
  class ControlinstrContext;
  class ReferenceinstrContext;
  class ParametricinstrContext;
  class VariableinstrContext;
  class TableinstrContext;
  class MemargContext;
  class MemoryinstrContext;
  class NumericinstrContext; 

  class  ModuleContext : public antlr4::ParserRuleContext {
  public:
    ModuleContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Id();
    std::vector<ModulefieldContext *> modulefield();
    ModulefieldContext* modulefield(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ModuleContext* module();

  class  ModulefieldContext : public antlr4::ParserRuleContext {
  public:
    ModulefieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypesectionContext *typesection();
    ImportsectionContext *importsection();
    FuncsectionContext *funcsection();
    TablesectionContext *tablesection();
    MemorysectionContext *memorysection();
    GlobalsectionContext *globalsection();
    ExportsectionContext *exportsection();
    StartsectionContext *startsection();
    ElemsectionContext *elemsection();
    DatasectionContext *datasection();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ModulefieldContext* modulefield();

  class  TypesectionContext : public antlr4::ParserRuleContext {
  public:
    TypesectionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    FunctypeContext *functype();
    antlr4::tree::TerminalNode *Id();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TypesectionContext* typesection();

  class  ImportdescContext : public antlr4::ParserRuleContext {
  public:
    ImportdescContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeuseContext *typeuse();
    antlr4::tree::TerminalNode *Id();
    TabletypeContext *tabletype();
    MemtypeContext *memtype();
    GlobaltypeContext *globaltype();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ImportdescContext* importdesc();

  class  ImportabbrContext : public antlr4::ParserRuleContext {
  public:
    ImportabbrContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> String();
    antlr4::tree::TerminalNode* String(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ImportabbrContext* importabbr();

  class  ImportsectionContext : public antlr4::ParserRuleContext {
  public:
    ImportsectionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> String();
    antlr4::tree::TerminalNode* String(size_t i);
    ImportdescContext *importdesc();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ImportsectionContext* importsection();

  class  FuncsectionContext : public antlr4::ParserRuleContext {
  public:
    FuncsectionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ImportabbrContext *importabbr();
    TypeuseContext *typeuse();
    antlr4::tree::TerminalNode *Id();
    std::vector<ExportabbrContext *> exportabbr();
    ExportabbrContext* exportabbr(size_t i);
    std::vector<LocalContext *> local();
    LocalContext* local(size_t i);
    std::vector<InstrContext *> instr();
    InstrContext* instr(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FuncsectionContext* funcsection();

  class  LocalContext : public antlr4::ParserRuleContext {
  public:
    LocalContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<ValtypeContext *> valtype();
    ValtypeContext* valtype(size_t i);
    antlr4::tree::TerminalNode *Id();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  LocalContext* local();

  class  TablesectionContext : public antlr4::ParserRuleContext {
  public:
    TablesectionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TabletypeContext *tabletype();
    antlr4::tree::TerminalNode *RefType();
    antlr4::tree::TerminalNode *Id();
    std::vector<ExportabbrContext *> exportabbr();
    ExportabbrContext* exportabbr(size_t i);
    ImportabbrContext *importabbr();
    std::vector<ElemexprContext *> elemexpr();
    ElemexprContext* elemexpr(size_t i);
    std::vector<FuncidxContext *> funcidx();
    FuncidxContext* funcidx(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TablesectionContext* tablesection();

  class  MemorysectionContext : public antlr4::ParserRuleContext {
  public:
    MemorysectionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MemtypeContext *memtype();
    antlr4::tree::TerminalNode *Id();
    std::vector<ExportabbrContext *> exportabbr();
    ExportabbrContext* exportabbr(size_t i);
    ImportabbrContext *importabbr();
    std::vector<antlr4::tree::TerminalNode *> String();
    antlr4::tree::TerminalNode* String(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  MemorysectionContext* memorysection();

  class  GlobalsectionContext : public antlr4::ParserRuleContext {
  public:
    GlobalsectionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ImportabbrContext *importabbr();
    GlobaltypeContext *globaltype();
    ConstexprContext *constexpr_();
    antlr4::tree::TerminalNode *Id();
    std::vector<ExportabbrContext *> exportabbr();
    ExportabbrContext* exportabbr(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  GlobalsectionContext* globalsection();

  class  ExportsectionContext : public antlr4::ParserRuleContext {
  public:
    ExportsectionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *String();
    ExportdescContext *exportdesc();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ExportsectionContext* exportsection();

  class  ExportdescContext : public antlr4::ParserRuleContext {
  public:
    ExportdescContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    FuncidxContext *funcidx();
    TableidxContext *tableidx();
    MemidxContext *memidx();
    GlobalidxContext *globalidx();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ExportdescContext* exportdesc();

  class  ExportabbrContext : public antlr4::ParserRuleContext {
  public:
    ExportabbrContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *String();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ExportabbrContext* exportabbr();

  class  StartsectionContext : public antlr4::ParserRuleContext {
  public:
    StartsectionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    FuncidxContext *funcidx();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  StartsectionContext* startsection();

  class  ElemsectionContext : public antlr4::ParserRuleContext {
  public:
    ElemsectionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ElemlistContext *elemlist();
    ConstexprContext *constexpr_();
    antlr4::tree::TerminalNode *Id();
    std::vector<FuncidxContext *> funcidx();
    FuncidxContext* funcidx(size_t i);
    TableuseContext *tableuse();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ElemsectionContext* elemsection();

  class  ElemlistContext : public antlr4::ParserRuleContext {
  public:
    ElemlistContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *RefType();
    std::vector<ElemexprContext *> elemexpr();
    ElemexprContext* elemexpr(size_t i);
    std::vector<FuncidxContext *> funcidx();
    FuncidxContext* funcidx(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ElemlistContext* elemlist();

  class  ElemexprContext : public antlr4::ParserRuleContext {
  public:
    ElemexprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ConstexprContext *constexpr_();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ElemexprContext* elemexpr();

  class  TableuseContext : public antlr4::ParserRuleContext {
  public:
    TableuseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TableidxContext *tableidx();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TableuseContext* tableuse();

  class  DatasectionContext : public antlr4::ParserRuleContext {
  public:
    DatasectionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Id();
    ConstexprContext *constexpr_();
    std::vector<antlr4::tree::TerminalNode *> String();
    antlr4::tree::TerminalNode* String(size_t i);
    MemuseContext *memuse();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DatasectionContext* datasection();

  class  MemuseContext : public antlr4::ParserRuleContext {
  public:
    MemuseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MemidxContext *memidx();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  MemuseContext* memuse();

  class  I32Context : public antlr4::ParserRuleContext {
  public:
    I32Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Integer();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  I32Context* i32();

  class  I64Context : public antlr4::ParserRuleContext {
  public:
    I64Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Integer();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  I64Context* i64();

  class  U32Context : public antlr4::ParserRuleContext {
  public:
    U32Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Integer();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  U32Context* u32();

  class  U64Context : public antlr4::ParserRuleContext {
  public:
    U64Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Integer();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  U64Context* u64();

  class  F32Context : public antlr4::ParserRuleContext {
  public:
    F32Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Float();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  F32Context* f32();

  class  F64Context : public antlr4::ParserRuleContext {
  public:
    F64Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Float();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  F64Context* f64();

  class  ValtypeContext : public antlr4::ParserRuleContext {
  public:
    ValtypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NumType();
    antlr4::tree::TerminalNode *RefType();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ValtypeContext* valtype();

  class  ParamContext : public antlr4::ParserRuleContext {
  public:
    ParamContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Id();
    std::vector<ValtypeContext *> valtype();
    ValtypeContext* valtype(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ParamContext* param();

  class  ResultContext : public antlr4::ParserRuleContext {
  public:
    ResultContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<ValtypeContext *> valtype();
    ValtypeContext* valtype(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ResultContext* result();

  class  FunctypeContext : public antlr4::ParserRuleContext {
  public:
    FunctypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<ParamContext *> param();
    ParamContext* param(size_t i);
    std::vector<ResultContext *> result();
    ResultContext* result(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FunctypeContext* functype();

  class  LimitsContext : public antlr4::ParserRuleContext {
  public:
    LimitsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<U64Context *> u64();
    U64Context* u64(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  LimitsContext* limits();

  class  MemtypeContext : public antlr4::ParserRuleContext {
  public:
    MemtypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    LimitsContext *limits();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  MemtypeContext* memtype();

  class  TabletypeContext : public antlr4::ParserRuleContext {
  public:
    TabletypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    LimitsContext *limits();
    antlr4::tree::TerminalNode *RefType();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TabletypeContext* tabletype();

  class  GlobaltypeContext : public antlr4::ParserRuleContext {
  public:
    GlobaltypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ValtypeContext *valtype();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  GlobaltypeContext* globaltype();

  class  TypeidxContext : public antlr4::ParserRuleContext {
  public:
    TypeidxContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    U32Context *u32();
    antlr4::tree::TerminalNode *Id();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TypeidxContext* typeidx();

  class  FuncidxContext : public antlr4::ParserRuleContext {
  public:
    FuncidxContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    U32Context *u32();
    antlr4::tree::TerminalNode *Id();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FuncidxContext* funcidx();

  class  TableidxContext : public antlr4::ParserRuleContext {
  public:
    TableidxContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    U32Context *u32();
    antlr4::tree::TerminalNode *Id();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TableidxContext* tableidx();

  class  MemidxContext : public antlr4::ParserRuleContext {
  public:
    MemidxContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    U32Context *u32();
    antlr4::tree::TerminalNode *Id();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  MemidxContext* memidx();

  class  GlobalidxContext : public antlr4::ParserRuleContext {
  public:
    GlobalidxContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    U32Context *u32();
    antlr4::tree::TerminalNode *Id();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  GlobalidxContext* globalidx();

  class  ElemidxContext : public antlr4::ParserRuleContext {
  public:
    ElemidxContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    U32Context *u32();
    antlr4::tree::TerminalNode *Id();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ElemidxContext* elemidx();

  class  DataidxContext : public antlr4::ParserRuleContext {
  public:
    DataidxContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    U32Context *u32();
    antlr4::tree::TerminalNode *Id();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DataidxContext* dataidx();

  class  LocalidxContext : public antlr4::ParserRuleContext {
  public:
    LocalidxContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    U32Context *u32();
    antlr4::tree::TerminalNode *Id();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  LocalidxContext* localidx();

  class  LabelidxContext : public antlr4::ParserRuleContext {
  public:
    LabelidxContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    U32Context *u32();
    antlr4::tree::TerminalNode *Id();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  LabelidxContext* labelidx();

  class  TypeuseContext : public antlr4::ParserRuleContext {
  public:
    TypeuseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeidxContext *typeidx();
    std::vector<ParamContext *> param();
    ParamContext* param(size_t i);
    std::vector<ResultContext *> result();
    ResultContext* result(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TypeuseContext* typeuse();

  class  InstrContext : public antlr4::ParserRuleContext {
  public:
    InstrContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    PlaininstrContext *plaininstr();
    BlockinstrContext *blockinstr();
    FoldedinstrContext *foldedinstr();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  InstrContext* instr();

  class  LabelContext : public antlr4::ParserRuleContext {
  public:
    LabelContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Id();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  LabelContext* label();

  class  ElseinstrContext : public antlr4::ParserRuleContext {
  public:
    ElseinstrContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    InstrContext *instr();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ElseinstrContext* elseinstr();

  class  BlockinstrContext : public antlr4::ParserRuleContext {
  public:
    BlockinstrContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    LabelContext *label();
    TypeuseContext *typeuse();
    std::vector<InstrContext *> instr();
    InstrContext* instr(size_t i);
    std::vector<antlr4::tree::TerminalNode *> Id();
    antlr4::tree::TerminalNode* Id(size_t i);
    std::vector<ElseinstrContext *> elseinstr();
    ElseinstrContext* elseinstr(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  BlockinstrContext* blockinstr();

  class  FoldedinstrContext : public antlr4::ParserRuleContext {
  public:
    FoldedinstrContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    PlaininstrContext *plaininstr();
    std::vector<FoldedinstrContext *> foldedinstr();
    FoldedinstrContext* foldedinstr(size_t i);
    LabelContext *label();
    TypeuseContext *typeuse();
    std::vector<InstrContext *> instr();
    InstrContext* instr(size_t i);
    std::vector<ElseinstrContext *> elseinstr();
    ElseinstrContext* elseinstr(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FoldedinstrContext* foldedinstr();

  class  ConstexprContext : public antlr4::ParserRuleContext {
  public:
    ConstexprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    I32Context *i32();
    I64Context *i64();
    F32Context *f32();
    F64Context *f64();
    antlr4::tree::TerminalNode *RefNull();
    FuncidxContext *funcidx();
    GlobalidxContext *globalidx();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ConstexprContext* constexpr_();

  class  PlaininstrContext : public antlr4::ParserRuleContext {
  public:
    PlaininstrContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ControlinstrContext *controlinstr();
    ReferenceinstrContext *referenceinstr();
    ParametricinstrContext *parametricinstr();
    VariableinstrContext *variableinstr();
    TableinstrContext *tableinstr();
    MemoryinstrContext *memoryinstr();
    NumericinstrContext *numericinstr();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  PlaininstrContext* plaininstr();

  class  ControlinstrContext : public antlr4::ParserRuleContext {
  public:
    ControlinstrContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<LabelidxContext *> labelidx();
    LabelidxContext* labelidx(size_t i);
    FuncidxContext *funcidx();
    TypeuseContext *typeuse();
    TableidxContext *tableidx();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ControlinstrContext* controlinstr();

  class  ReferenceinstrContext : public antlr4::ParserRuleContext {
  public:
    ReferenceinstrContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *RefNull();
    FuncidxContext *funcidx();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ReferenceinstrContext* referenceinstr();

  class  ParametricinstrContext : public antlr4::ParserRuleContext {
  public:
    ParametricinstrContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<ResultContext *> result();
    ResultContext* result(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ParametricinstrContext* parametricinstr();

  class  VariableinstrContext : public antlr4::ParserRuleContext {
  public:
    VariableinstrContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    LocalidxContext *localidx();
    GlobalidxContext *globalidx();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  VariableinstrContext* variableinstr();

  class  TableinstrContext : public antlr4::ParserRuleContext {
  public:
    TableinstrContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<TableidxContext *> tableidx();
    TableidxContext* tableidx(size_t i);
    ElemidxContext *elemidx();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TableinstrContext* tableinstr();

  class  MemargContext : public antlr4::ParserRuleContext {
  public:
    MemargContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Offset();
    antlr4::tree::TerminalNode *Align();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  MemargContext* memarg();

  class  MemoryinstrContext : public antlr4::ParserRuleContext {
  public:
    MemoryinstrContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<MemidxContext *> memidx();
    MemidxContext* memidx(size_t i);
    DataidxContext *dataidx();
    MemargContext *memarg();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  MemoryinstrContext* memoryinstr();

  class  NumericinstrContext : public antlr4::ParserRuleContext {
  public:
    NumericinstrContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    I32Context *i32();
    I64Context *i64();
    F32Context *f32();
    F64Context *f64();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  NumericinstrContext* numericinstr();


  // By default the static state used to implement the parser is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:
};

}  // namespace WasmVM
