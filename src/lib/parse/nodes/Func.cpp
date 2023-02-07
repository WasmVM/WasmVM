// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "../syntax.hpp"
#include "visitor.hpp"

using namespace WasmVM;

void ModuleVisitor::operator()(Parse::Func& node){
    // id
    if(!node.id.empty()){
        if(funcid_map.contains(node.id)){
            throw Exception::duplicated_identifier(node.location, std::string(" : func ") + node.id);
        }
        funcid_map[node.id] = funcidx;
    }

    WasmFunc& func = module.funcs.emplace_back();
    
    // Type
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
        std::map<std::string, index_t> paramid_map;
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
            typeidx += 1;
        }
    }

    // Local
    for(Parse::ValueType local : node.locals){
        func.locals.emplace_back(local);
    }

    // Body
    for(Parse::Func::Instr instrnode : node.body){
        std::visit(InstrVisitor(*this, func), instrnode);
    }

    funcidx += 1;
}

std::optional<Parse::Func> Parse::Func::get(TokenIter& begin, const TokenIter& end){
    std::list<TokenType>::iterator it = begin;
    auto syntax = Parse::Rule<
        Token::ParenL, Token::Keyword<"func">, Parse::Optional<Token::Id>, Parse::TypeUse,
        Parse::Repeat<Syntax::Local>, Parse::Repeat<Parse::OneOf<Syntax::PlainInstr>>,
        Token::ParenR
    >::get(it, end);

    if(syntax){
        Parse::Func func;
        auto rule = syntax.value();
        func.location = std::get<0>(rule).location;
        auto id = std::get<2>(rule);
        func.id = id ? id->value : "";
        func.type = Parse::Type(std::get<3>(rule));
        // Local
        index_t local_idx = func.type.func.params.size();
        for(auto local : std::get<4>(rule)){
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
        for(auto instr : std::get<5>(rule)){
            std::visit(overloaded {
                [&](Syntax::PlainInstr plain){
                    std::visit([&](auto ins){
                        func.body.emplace_back(ins);
                    }, plain);
                }
            }, instr);
        }
        begin = it;
        return func;
    }

    return std::nullopt;
}