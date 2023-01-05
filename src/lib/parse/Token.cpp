// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "Token.hpp"

#include <functional>
#include <regex>

using namespace WasmVM;
using namespace Token;

TokenBase::TokenBase(Location& loc) : location(loc) {}

template<> Paren<'('>::Paren(Location loc) : TokenBase(loc){}
template<> Paren<')'>::Paren(Location loc) : TokenBase(loc){}

Id::Id(Location loc, std::string value) : TokenBase(loc), value(value){}

Number::Number(Location loc, std::string str) : TokenBase(loc){

}

bool Number::validate(std::string str) {
    return true; // TODO:
}

String::String(Location loc, std::string value) : TokenBase(loc), value(value){}

Keyword::Keyword(Location loc, std::string value) : TokenBase(loc), value(std::hash<std::string>{}(value)){}

bool Keyword::validate(std::string str) {
    return true; // TODO:
}