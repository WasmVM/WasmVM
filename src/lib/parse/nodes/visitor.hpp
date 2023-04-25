#ifndef WASMVM_PP_PARSE_NODES_VISITOR_DEF
#define WASMVM_PP_PARSE_NODES_VISITOR_DEF

#include <structures/WasmModule.hpp>
#include "../syntax.hpp"

#include <map>
#include <string>
#include <iostream>

namespace WasmVM {

struct ModuleVisitor {

    struct IndexSpace {
        enum class Type {Import, Normal};
        std::vector<Type> records;
        std::map<std::string, index_t> id_map;
    };

    ModuleVisitor(WasmModule& module) : module(module){}
    WasmModule& module;
    IndexSpace func_indices;
    IndexSpace table_indices;
    IndexSpace mem_indices;
    IndexSpace global_indices;
    std::map<std::string, index_t> typeid_map;
    std::map<std::string, index_t> elemid_map;
    std::map<std::string, index_t> dataid_map;
    std::vector<std::map<std::string, index_t>> paramid_maps;

    void operator()(Parse::Type& type);
    void operator()(Parse::Import& import);
    void operator()(Parse::Func& func);
    void operator()(Parse::Table& table);
    void operator()(Parse::Memory& mem);
    void operator()(Parse::Global& global);
    void operator()(Parse::Export& export_);
    void operator()(Parse::Start& start);
    void operator()(Parse::Elem& elem);
    void operator()(Parse::Data& data);
    void operator()(Parse::Rule<Token::ParenL, Token::ParenR>&){}
};

struct ImportVisitor {
    ImportVisitor(Parse::Import& import) : import(import){}
    Parse::Import& import;

    void operator()(Syntax::ImportDesc::Func& desc);
    void operator()(Syntax::ImportDesc::Table& desc);
    void operator()(Syntax::ImportDesc::Memory& desc);
    void operator()(Syntax::ImportDesc::Global& desc);
};

namespace InstrVisitor{

template <typename T>
class Base {
protected:
    Base(ModuleVisitor& module, std::vector<T>& instrs) : module(module), instrs(instrs) {}
    
    ModuleVisitor& module;
    std::vector<T>& instrs;
};

struct ConstSema : public Base<WasmVM::ConstInstr>{
    ConstSema(ModuleVisitor& module, std::vector<WasmVM::ConstInstr>& instrs) : Base<WasmVM::ConstInstr>(module, instrs) {}
    void operator()(Parse::Instr::Ref_null&);
    void operator()(Parse::Instr::Ref_func&);
    void operator()(Parse::Instr::Global_get&);
    void operator()(Parse::Instr::I32_const&);
    void operator()(Parse::Instr::I64_const&);
    void operator()(Parse::Instr::F32_const&);
    void operator()(Parse::Instr::F64_const&);
};

struct Sema : public Base<WasmVM::WasmInstr>{
    Sema(
        ModuleVisitor& module,
        WasmFunc& func,
        std::map<std::string, index_t>& labelid_map, 
        const std::map<std::string, index_t>& localid_map
    ) : Base<WasmVM::WasmInstr>(module, func.body), func(func), labelid_map(labelid_map), localid_map(localid_map) {}

    WasmFunc& func;
    std::map<std::string, index_t>& labelid_map;
    const std::map<std::string, index_t>& localid_map;

    template<typename T> void operator()(T& instr);
};

struct Syntax {
    Syntax(std::vector<Parse::Instr::Instruction>& body) : body(body){}

    void operator()(Parse::Instr::Block&);
    void operator()(Parse::Instr::Loop&);
    void operator()(Parse::Instr::If&);
    void operator()(WasmVM::Syntax::PlainInstr&);
    void operator()(WasmVM::Syntax::FoldedInstr&);
    void operator()(WasmVM::Syntax::ConstInstr&);
private:
    std::vector<Parse::Instr::Instruction>& body;
};

struct ConstSyntax {
    ConstSyntax(std::vector<Parse::Instr::ConstInstr>& body) : body(body){}
    void operator()(WasmVM::Syntax::ConstInstr&);
private:
    std::vector<Parse::Instr::ConstInstr>& body;
};

} // namespace InstrVisitor
} // namespace WasmVM

#endif