// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "Token.hpp"

#include <functional>

using namespace WasmVM;
using namespace Token;

TokenBase::TokenBase(Location& loc) : location(loc) {}

template<> Paren<'('>::Paren(Location loc) : TokenBase(loc){}
template<> Paren<')'>::Paren(Location loc) : TokenBase(loc){}

Id::Id(Location loc, std::string value) : TokenBase(loc), value(value){}

Number::Number(Location loc, Value value) : TokenBase(loc), value(value){}

std::optional<Number> Number::create(Location loc, std::string str){
    return std::nullopt;
}

String::String(Location loc, std::string value) : TokenBase(loc), value(value){}

Keyword::Keyword(Location loc, std::string value) : TokenBase(loc), value(std::hash<std::string>{}(value)){}

std::optional<Keyword> Keyword::create(Location loc, std::string str){
    return std::nullopt;
}