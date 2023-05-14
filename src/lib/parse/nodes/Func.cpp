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
        WasmExport& export_ = module.exports.emplace_back();
        export_.name = name;
        export_.desc = WasmExport::DescType::func;
        export_.index = func_indices.records.size();
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
        import.desc.emplace<index_t>(Parse::Type::index(node.typeuse, module, typeid_map, paramid_maps));
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
    func.typeidx = Parse::Type::index(node.typeuse, module, typeid_map, paramid_maps);

    // Local
    for(Parse::ValueType local : node.locals){
        func.locals.emplace_back(local);
    }

    // Body
    std::map<std::string, index_t> labelid_map;
    labelid_map[node.id] = 0;
    std::map<std::string, index_t> localid_map(paramid_maps[func.typeidx]);
    localid_map.merge(node.local_id_map);
    for(Parse::Instr::Instruction instrnode : node.body){
        std::visit(InstrVisitor::Sema(*this, func, labelid_map, localid_map), instrnode);
    }
    func.body.emplace_back(WasmVM::Instr::End());
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
            func.exports.emplace_back(std::get<2>(node).value.substr(1, std::get<2>(node).value.size() - 2));
        }
        // rules
        std::visit(overloaded {
            [&](importrule& node){
                func.import = std::pair<std::string, std::string>(std::get<2>(node).value, std::get<3>(node).value);
                func.typeuse = std::get<5>(node);
            },
            [&](funcrule& node){
                func.typeuse = std::get<0>(node);
                // Local
                index_t local_idx = func.typeuse.functype.params.size();
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