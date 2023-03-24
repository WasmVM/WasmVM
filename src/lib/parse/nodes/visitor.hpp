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

    // FIXME:
    ~ModuleVisitor(){
        std::cout << "== type index ==" << std::endl;
        for(auto type_id : typeid_map){
            std::cout << "  " << type_id.first << "(" << type_id.second << ")" << std::endl;
        }
        std::cout << "== param index ==" << std::endl;
        for(size_t i = 0; i < paramid_maps.size(); ++i){
            std::cout << "  " << i << " : ";
            for(auto paramid : paramid_maps[i]){
                std::cout << paramid.first << "(" << paramid.second << ") ";
            }
            std::cout << std::endl;
        }
        std::cout << "== func index ==" << std::endl;
        for(auto func_id : func_indices.id_map){
            std::cout << "  " << func_id.first << "(" << func_id.second << ")" << std::endl;
        }
        std::cout << "== table index ==" << std::endl;
        for(auto table_id : table_indices.id_map){
            std::cout << "  " << table_id.first << "(" << table_id.second << ")" << std::endl;
        }
        std::cout << "== mem index ==" << std::endl;
        for(auto mem_id : mem_indices.id_map){
            std::cout << "  " << mem_id.first << "(" << mem_id.second << ")" << std::endl;
        }
        std::cout << "== global index ==" << std::endl;
        for(auto global_id : global_indices.id_map){
            std::cout << "  " << global_id.first << "(" << global_id.second << ")" << std::endl;
        }
    }

    void operator()(Parse::Type& type);
    void operator()(Parse::Import& import);
    void operator()(Parse::Func& func);
    void operator()(Parse::Table& table);
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
    Sema(
        ModuleVisitor& module,
        WasmFunc& func,
        std::map<std::string, index_t>& labelid_map, 
        const std::map<std::string, index_t>& localid_map
    ) : module(module), func(func), labelid_map(labelid_map), localid_map(localid_map) {}

    ModuleVisitor& module;
    WasmFunc& func;
    std::map<std::string, index_t>& labelid_map;
    const std::map<std::string, index_t>& localid_map;

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
    void operator()(Parse::Instr::Local_get& instr);
    void operator()(Parse::Instr::Local_set& instr);
    void operator()(Parse::Instr::Local_tee& instr);
    void operator()(Parse::Instr::Global_get& instr);
    void operator()(Parse::Instr::Global_set& instr);
    void operator()(Parse::Instr::Table_get& instr);
    void operator()(Parse::Instr::Table_set& instr);
    void operator()(Parse::Instr::Table_size& instr);
    void operator()(Parse::Instr::Table_grow& instr);
    void operator()(Parse::Instr::Table_fill& instr);
    void operator()(Parse::Instr::Table_copy& instr);
    void operator()(Parse::Instr::Table_init& instr);
    void operator()(Parse::Instr::Elem_drop& instr);
    void operator()(Parse::Instr::Memory_size& instr);
    void operator()(Parse::Instr::Memory_grow& instr);
    void operator()(Parse::Instr::Memory_fill& instr);
    void operator()(Parse::Instr::Memory_copy& instr);
    void operator()(Parse::Instr::Memory_init& instr);
    void operator()(Parse::Instr::Data_drop& instr);
    void operator()(Parse::Instr::NumericInstr& instr);
};

struct Syntax {

    Syntax(std::vector<Parse::Instr::Instrction>& body) : body(body){}
    std::vector<Parse::Instr::Instrction>& body;

    template<typename T>
    void operator()(T& instr){
        body.emplace_back(instr);
    }
    void operator()(WasmVM::Syntax::PlainInstr&);
    void operator()(WasmVM::Syntax::FoldedInstr&);
};

}



}

#endif