// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "Token.hpp"

#include <regex>
#include <ranges>
#include <algorithm>

#include <iostream>


using namespace WasmVM;
using namespace Token;

TokenBase::TokenBase(Location& loc) : location(loc) {}

template<> Paren<'('>::Paren(Location loc) : TokenBase(loc){}
template<> Paren<')'>::Paren(Location loc) : TokenBase(loc){}

Id::Id(Location loc, std::string value) : TokenBase(loc), value(value){}

Number::Number(Location loc, std::string value) : TokenBase(loc), value(value){}

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

String::String(Location loc, std::string value) : TokenBase(loc), value(value){}

Keyword::Keyword(Location loc, std::string value) : TokenBase(loc), value(value){}
