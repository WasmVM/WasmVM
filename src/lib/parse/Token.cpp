// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "parse.hpp"

#include <regex>
#include <ranges>
#include <algorithm>

using namespace WasmVM;
using namespace Token;

TokenBase::TokenBase(Location& loc, std::string value) : location(loc), value(value){}

ParenL::ParenL(Location loc) : TokenBase(loc, "("){}

ParenR::ParenR(Location loc) : TokenBase(loc, ")"){}

Id::Id(Location loc, std::string value) : TokenBase(loc, value){}

Number::Number(Location loc, std::string value) : TokenBase(loc, value){}

std::optional<Number> Number::create(Location loc, std::string str){
    static std::regex decimal("([\\+\\-]?[0-9](_?[0-9])*)((\\.([0-9](_?[0-9])*)?)?([eE][\\+\\-]?[0-9](_?[0-9])*)?)");
    static std::regex hexadecimal("([\\+\\-]?0x[0-9a-fA-F](_?[0-9a-fA-F])*)((\\.([0-9a-fA-F](_?[0-9a-fA-F])*)?)?([pP][\\+\\-]?[0-9](_?[0-9])*)?)");
    static std::regex inf_nan("[\\+\\-]?((inf)|(nan(:0x[0-9a-fA-F](_?[0-9a-fA-F])*)?))");

    if(std::regex_match(str, decimal)
        || std::regex_match(str, hexadecimal)
        || std::regex_match(str, inf_nan)
    ){
        auto rem = std::ranges::remove(str, '_');
        str.erase(rem.begin(), rem.end());
        return Number(loc, str);
    }
    return std::nullopt;
}

String::String(Location loc, std::string value) : TokenBase(loc, value){}

Keyword::Keyword(Location loc, std::string value) : TokenBase(loc, value){}

#define TokenGet(TOKEN) std::optional<TOKEN> TOKEN::get(TokenIter& begin, const TokenIter& end){ \
    if(begin != end && std::holds_alternative<TOKEN>(*begin)){ \
        return std::get<TOKEN>(*(begin++)); \
    }else{ \
        return std::nullopt; \
    } \
}

TokenGet(Token::ParenL)
TokenGet(Token::ParenR)
TokenGet(Token::Id)
TokenGet(Token::Number)
TokenGet(Token::String)
TokenGet(Token::Keyword)