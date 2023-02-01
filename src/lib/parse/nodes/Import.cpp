// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "../syntax.hpp"
#include "visitor.hpp"

using namespace WasmVM;

void ModuleVisitor::operator()(Parse::Import& node){
    WasmImport& import = module.imports.emplace_back();
    std::visit(overloaded {
        [&](Parse::Type type){
            index_t idx = std::visit(overloaded {
                [&](std::string id){
                    if(id.empty()){
                        return (index_t) -1;
                    }else if(typeid_map.contains(id)){
                        return typeid_map[id];
                    }else{
                        throw Exception::unknown_identifier(node.location, std::string(" import type '") + id + "' not found");
                    }
                },
                [&](index_t id){
                    return id;
                }
            }, type.id);

            FuncType functype;
            if(idx != -1){
                if(type.func.params.empty() && type.func.results.empty()){
                    import.desc.emplace<index_t>(idx);
                    return;
                }
                if(idx >= module.types.size()){
                    throw Exception::index_out_of_range(node.location, " : type not found");
                }
                functype = module.types[idx];
            }
            functype.params.insert(functype.params.end(), type.func.params.begin(), type.func.params.end());
            functype.results.insert(functype.results.end(), type.func.results.begin(), type.func.results.end());
            import.desc.emplace<index_t>(module.types.size());
            module.types.emplace_back(functype);
        }
    }, node.desc);
    import.module = node.module;
    import.name = node.name;
    import.id = node.id;
}

std::optional<Parse::Import> Parse::Import::get(TokenIter& begin, const TokenIter& end){
    std::list<TokenType>::iterator it = begin;

    auto syntax = Parse::Rule<
        Token::ParenL, Token::Keyword<"import">, Token::String, Token::String, 
        Parse::OneOf<
            Syntax::ImportDesc::Func
            // Parse::Rule<Token::ParenL, Token::Keyword<"table">, Parse::Optional<Token::Id>, TypeUse, Token::ParenR>,
            // Parse::Rule<Token::ParenL, Token::Keyword<"memory">, Parse::Optional<Token::Id>, TypeUse, Token::ParenR>,
            // Parse::Rule<Token::ParenL, Token::Keyword<"global">, Parse::Optional<Token::Id>, TypeUse, Token::ParenR>
        >,
        Token::ParenR
    >::get(it, end);

    if(syntax){
        Parse::Import import;
        auto rule = syntax.value();
        import.module = std::get<2>(rule).value;
        import.name = std::get<3>(rule).value;
        std::visit(ImportVisitor(import), std::get<4>(rule));
        begin = it;
        return import;
    }

    return std::nullopt;
}

void ImportVisitor::operator()(Syntax::ImportDesc::Func& desc){
    auto id = std::get<2>(desc);
    import.id = id ? id->value : "";
    import.location = id->location;
    auto typeuse = std::get<3>(desc);
    Parse::Type& importdesc = import.desc.emplace<Parse::Type>();
    // index
    auto index = std::get<0>(typeuse);
    if(index){
        auto typeidx = std::get<2>(index.value());
        std::visit(overloaded {
            [&](Token::Number token){
                importdesc.id.emplace<index_t>(token.unpack<index_t>());
            },
            [&](Token::Id token){
                importdesc.id.emplace<std::string>(token.value);
            }
        }, typeidx);
    }

    Parse::FuncType& func_type = importdesc.func;
    // param
    auto params = std::get<1>(typeuse);
    for(index_t i = 0; i < params.size(); ++i){
        // location
        auto param = params[i];
        Token::Location location = std::get<0>(param).location;
        auto id = std::get<2>(param);
        auto types = std::get<3>(param);
        if(id){
            switch(types.size()){
                case 0:
                    throw Exception::invalid_functype(location, ": expected one parameter to an identifier");
                case 1:
                break;
                default:
                    throw Exception::invalid_functype(location, ": an identifier can only bind to one parameter");
            }
            if(importdesc.func.id_map.contains(id->value)){
                throw Exception::duplicated_identifier(id->location, std::string(": param ") + id->value);
            }
            importdesc.func.id_map[id->value] = i;
            importdesc.func.params.emplace_back(types.front());
        }else{
            for(Parse::ValueType type : types){
                importdesc.func.params.emplace_back(type);
            }
        }
    }

    // result
    for(auto result : std::get<2>(typeuse)){
        auto types = std::get<2>(result);
        for(Parse::ValueType type : types){
            importdesc.func.results.emplace_back(type);
        }
    }
}