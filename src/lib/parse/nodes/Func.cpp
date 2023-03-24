// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "../syntax.hpp"
#include "visitor.hpp"

#include <algorithm>

using namespace WasmVM;

void ModuleVisitor::operator()(Parse::Func& node){
    // export
    for(std::string name : node.exports){
        module.exports.emplace_back(name, WasmExport::DescType::func, func_indices.records.size());
    }

    // import
    if(!node.import.first.empty() || !node.import.second.empty()){
        WasmImport& import = module.imports.emplace_back();
        if(!node.id.empty()){
            if(func_indices.id_map.contains(node.id)){
                throw Exception::duplicated_identifier(node.location, std::string(" : func ") + node.id);
            }
            func_indices.id_map[node.id] = func_indices.records.size();
        }
        import.module = node.import.first;
        import.name = node.import.second;
        import.desc.emplace<index_t>(node.type.index(module, typeid_map, paramid_maps));
        func_indices.records.emplace_back(IndexSpace::Type::Import);
        return;
    }

    // id
    if(!node.id.empty()){
        if(func_indices.id_map.contains(node.id)){
            throw Exception::duplicated_identifier(node.location, std::string(" : func ") + node.id);
        }
        func_indices.id_map[node.id] = func_indices.records.size();
    }
    func_indices.records.emplace_back(IndexSpace::Type::Normal);

    WasmFunc& func = module.funcs.emplace_back();
    
    // Type
    std::map<std::string, index_t> paramid_map;
    {
        index_t idx = std::visit(overloaded {
            [&](std::string id){
                if(id.empty()){
                    return index_npos;
                }else if(typeid_map.contains(id)){
                    return typeid_map[id];
                }else{
                    throw Exception::unknown_identifier(node.location, std::string(" func type '") + id + "' not found");
                }
            },
            [&](index_t id){
                return id;
            }
        }, node.type.id);

        FuncType functype;
        bool create_type = true;
        if(idx != index_npos){
            if(idx >= module.types.size()){
                throw Exception::index_out_of_range(node.location, " : type not found");
            }
            if(node.type.func.params.empty() && node.type.func.results.empty()){
                func.typeidx = idx;
                paramid_maps.emplace_back(paramid_maps[idx]);
                create_type = false;
            }else{
                functype = module.types[idx];
                paramid_map = paramid_maps[idx];
            }
        }
        if(create_type){
            size_t param_count = functype.params.size();
            for(auto id_pair : node.type.func.id_map){
                if(paramid_map.contains(id_pair.first)){
                    throw Exception::duplicated_identifier(node.location, std::string(" : func param ") + id_pair.first);
                }
                paramid_map[id_pair.first] = id_pair.second + param_count;
            }
            functype.params.insert(functype.params.end(), node.type.func.params.begin(), node.type.func.params.end());
            functype.results.insert(functype.results.end(), node.type.func.results.begin(), node.type.func.results.end());
            func.typeidx = module.types.size();
            paramid_maps.emplace_back(paramid_map);
            module.types.emplace_back(functype);
        }
    }

    // Local
    for(Parse::ValueType local : node.locals){
        func.locals.emplace_back(local);
    }

    // Body
    std::map<std::string, index_t> labelid_map;
    labelid_map[node.id] = 0;
    std::map<std::string, index_t> localid_map(paramid_map);
    localid_map.merge(node.local_id_map);
    for(Parse::Instr::Instrction instrnode : node.body){
        std::visit(InstrVisitor::Sema(*this, func, labelid_map, localid_map), instrnode);
    }
}

std::optional<Parse::Func> Parse::Func::get(TokenIter& begin, TokenHolder& holder){
    std::list<TokenType>::iterator it = begin;

    using Local = Parse::Rule<
        Token::ParenL, Token::Keyword<"local">, Parse::Optional<Token::Id>, Parse::Repeat<Parse::ValueType>, Token::ParenR
    >;
    using funcrule = Parse::Rule<
        Parse::TypeUse, Parse::Repeat<Local>, Parse::Repeat<Parse::OneOf<Syntax::Instr, Syntax::FoldedInstr>>
    >;
    using importrule = Parse::Rule<
        Token::ParenL, Token::Keyword<"import">, Token::String, Token::String, Token::ParenR, Parse::TypeUse
    >;

    auto syntax = Parse::Rule<
        Token::ParenL, Token::Keyword<"func">, Parse::Optional<Token::Id>, 
        Parse::Repeat<Parse::Rule<Token::ParenL, Token::Keyword<"export">, Token::String, Token::ParenR>>,
        Parse::OneOf<importrule, funcrule>,
        Token::ParenR
    >::get(it, holder);

    if(syntax){
        Parse::Func func;
        auto rule = syntax.value();
        func.location = std::get<0>(rule).location;
        // id
        auto id = std::get<2>(rule);
        func.id = id ? id->value : "";
        // export
        for(auto node : std::get<3>(rule)){
            func.exports.emplace_back(std::get<2>(node).value);
        }
        // rules
        std::visit(overloaded {
            [&](importrule& node){
                func.import = std::pair<std::string, std::string>(std::get<2>(node).value, std::get<3>(node).value);
                func.type = Parse::Type(std::get<5>(node));
            },
            [&](funcrule& node){
                func.type = Parse::Type(std::get<0>(node));
                // Local
                index_t local_idx = func.type.func.params.size();
                for(auto local : std::get<1>(node)){
                    // location
                    Token::Location location = std::get<0>(local).location;
                    auto local_id = std::get<2>(local);
                    auto local_defs = std::get<3>(local);
                    if(local_id){
                        switch(local_defs.size()){
                            case 0:
                                throw Exception::invalid_functype(location, ": expected one local type to an identifier");
                            case 1:
                            break;
                            default:
                                throw Exception::invalid_functype(location, ": an identifier can only bind to one local type");
                        }
                        if(func.local_id_map.contains(local_id->value)){
                            throw Exception::duplicated_identifier(local_id->location, std::string(": local ") + local_id->value);
                        }
                        func.local_id_map[local_id->value] = local_idx;
                        func.locals.emplace_back(local_defs.front());
                    }else{
                        for(Parse::ValueType local : local_defs){
                            func.locals.emplace_back(local);
                        }
                    }
                    local_idx += 1;
                }
                // Instr
                for(auto instr : std::get<2>(node)){
                    std::visit(overloaded {
                        [&](Syntax::Instr& i){
                            std::visit(InstrVisitor::Syntax(func.body), i);
                        },
                        [&](Syntax::FoldedInstr& i){
                            InstrVisitor::Syntax(func.body)(i);
                        }
                    }, instr);
                }
            },
        },  std::get<4>(rule));
        begin = it;
        return func;
    }

    return std::nullopt;
}