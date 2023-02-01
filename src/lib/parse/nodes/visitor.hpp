#ifndef WASMVM_PP_PARSE_NODES_VISITOR_DEF
#define WASMVM_PP_PARSE_NODES_VISITOR_DEF

#include <structures/WasmModule.hpp>
#include "../syntax.hpp"

#include <map>
#include <string>

#include <iostream>

namespace WasmVM {

struct ModuleVisitor {
    ModuleVisitor(WasmModule& module) : module(module){}
    WasmModule& module;
    std::map<std::string, index_t> typeid_map;
    std::map<std::string, index_t> funcid_map;
    std::vector<std::map<std::string, index_t>> paramid_maps;

    // ~ModuleVisitor(){
    //     std::cout << "== typeid map ==" << std::endl;
    //     for(auto type_id : typeid_map){
    //         std::cout << "  " << type_id.first << "(" << type_id.second << ")" << std::endl;
    //     }
    //     std::cout << "== localid map ==" << std::endl;
    //     for(size_t i = 0; i < paramid_maps.size(); ++i){
    //         std::cout << "  " << i << " : ";
    //         for(auto localid : paramid_maps[i]){
    //             std::cout << localid.first << "(" << localid.second << ") ";
    //         }
    //         std::cout << std::endl;
    //     }
    // }

    void operator()(Parse::Type& type);
    void operator()(Parse::Import& import);
};

struct ImportVisitor {
    ImportVisitor(Parse::Import& import) : import(import){}
    Parse::Import& import;

    void operator()(Syntax::ImportDesc::Func& desc);
};

}

#endif