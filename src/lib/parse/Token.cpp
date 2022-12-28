// Copyright 2022 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "Token.hpp"

using namespace WasmVM;
using namespace TokenType;

std::map<std::string, Keyword::Type> Keyword::typemap = {
    {"module" , Keyword::Type::module},
};

Keyword::Keyword(std::string str) : type(typemap[str]){}
Keyword::Keyword(Keyword::Type type) : type(type){}
bool Keyword::match(std::string str){
    return typemap.contains(str);
}
Token Keyword::create(std::string str){
    return Keyword(Keyword::typemap[str]);
}

bool LeftParen::match(std::string str){
    return str == "(";
}
Token LeftParen::create(std::string){
    return LeftParen();
}
bool RightParen::match(std::string str){
    return str == ")";
}
Token RightParen::create(std::string){
    return RightParen();
}
