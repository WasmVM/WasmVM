#ifndef WASMVM_PP_PARSE_NODES_VISITOR_DEF
#define WASMVM_PP_PARSE_NODES_VISITOR_DEF

#include <structures/WasmModule.hpp>
#include "../syntax.hpp"

#include <map>
#include <string>

#include <iostream>

namespace WasmVM {

struct ModuleVisitor {
    ModuleVisitor(WasmModule& module) :
        module(module), tableidx(0), memidx(0), globalidx(0), funcidx(0){}
    WasmModule& module;
    std::map<std::string, index_t> typeid_map;
    std::map<std::string, index_t> funcid_map;
    std::map<std::string, index_t> tableid_map;
    std::map<std::string, index_t> memid_map;
    std::map<std::string, index_t> globalid_map;
    std::vector<std::map<std::string, index_t>> paramid_maps;

    // FIXME:
    ~ModuleVisitor(){
        std::cout << "== typeid map ==" << std::endl;
        for(auto type_id : typeid_map){
            std::cout << "  " << type_id.first << "(" << type_id.second << ")" << std::endl;
        }
        std::cout << "== localid map ==" << std::endl;
        for(size_t i = 0; i < paramid_maps.size(); ++i){
            std::cout << "  " << i << " : ";
            for(auto localid : paramid_maps[i]){
                std::cout << localid.first << "(" << localid.second << ") ";
            }
            std::cout << std::endl;
        }
        std::cout << "== funcid map ==" << std::endl;
        for(auto func_id : funcid_map){
            std::cout << "  " << func_id.first << "(" << func_id.second << ")" << std::endl;
        }
        std::cout << "== tableid map ==" << std::endl;
        for(auto table_id : tableid_map){
            std::cout << "  " << table_id.first << "(" << table_id.second << ")" << std::endl;
        }
        std::cout << "== memid map ==" << std::endl;
        for(auto mem_id : memid_map){
            std::cout << "  " << mem_id.first << "(" << mem_id.second << ")" << std::endl;
        }
        std::cout << "== globalid map ==" << std::endl;
        for(auto global_id : globalid_map){
            std::cout << "  " << global_id.first << "(" << global_id.second << ")" << std::endl;
        }
    }

    void operator()(Parse::Type& type);
    void operator()(Parse::Import& import);
    void operator()(Parse::Func& func);

private:
    index_t tableidx;
    index_t memidx;
    index_t globalidx;
    index_t funcidx;
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

struct Sema {
    Sema(ModuleVisitor& module, WasmFunc& func, std::map<std::string, index_t>& labelid_map)
        : module(module), func(func), labelid_map(labelid_map){}
    ModuleVisitor& module;
    WasmFunc& func;
    std::map<std::string, index_t>& labelid_map;

    template<typename T> void operator()(T& instr){
        func.body.emplace_back(instr);
    }

    void operator()(Parse::Instr::Call& instr);
    void operator()(Parse::Instr::Block& instr);
    void operator()(Parse::Instr::Loop& instr);
    void operator()(Parse::Instr::If& instr);
    void operator()(Parse::Instr::Br& instr);
    void operator()(Parse::Instr::Br_if& instr);
    void operator()(Parse::Instr::Br_table& instr);
    void operator()(Parse::Instr::Call_indirect& instr);
    void operator()(Parse::Instr::Ref_func& instr);
};

struct Syntax {

    Syntax(std::vector<Parse::Instr::Instrction>& body) : body(body){}
    std::vector<Parse::Instr::Instrction>& body;

    template<typename T>
    void operator()(T& instr){
        body.emplace_back(instr);
    }
    void operator()(WasmVM::Syntax::PlainInstr&);
};

}



}

#endif