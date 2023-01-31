// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "../syntax.hpp"
#include "visitor.hpp"

#include <ranges>

using namespace WasmVM;

void ModuleVisitor::operator()(Parse::Import& import){
    
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
    import.id = id ? id.value().value : "";
    auto typeuse = std::get<3>(desc);
    auto index = std::get<0>(typeuse);
    Parse::FuncType func_type;
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
            func_type.id_map[id->value] = i;
            func_type.params.emplace_back(types.front());
        }else{
            for(Parse::ValueType type : types){
                func_type.params.emplace_back(type);
            }
        }
    }

    // result
    for(auto result : std::get<2>(typeuse)){
        auto types = std::get<2>(result);
        for(Parse::ValueType type : types){
            func_type.results.emplace_back(type);
        }
    }

}