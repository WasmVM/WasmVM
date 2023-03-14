// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "parse.hpp"

#include <algorithm>

#include "Keywords.hpp"

using namespace WasmVM;
using namespace Token;

TokenBase::TokenBase(Location& loc, std::string value) : location(loc), value(value){}

bool TokenHolder::has_next(TokenIter& it){
    return it != end;
}

TokenIter TokenHolder::next(TokenIter& it){
    if(it == last){
        ++last;
    }
    return it++;
}

Exception::unexpected_token TokenHolder::error(){
    TokenIter it = last;
    Token::Location location;
    std::string message = " '";
    std::visit(overloaded {
        [&message](TokenBase token){
            message += token.value + "' after '";
        },
        [&message](MemArgBase token){
            message += token.key + "=" + token.value + "' after '";
        }
    }, *(it--));
    std::visit(overloaded {
        [&](TokenBase token){
            message += token.value + "'";
            location = token.location;
        },
        [&](MemArgBase token){
            message += token.key + "=" + token.value + "'";
            location = token.location;
        }
    }, *it);
    return Exception::unexpected_token(location, message);
}

ParenL::ParenL(Location loc) : TokenBase(loc, "("){}

ParenR::ParenR(Location loc) : TokenBase(loc, ")"){}

Id::Id(Location loc, std::string value) : TokenBase(loc, value){}

Number::Number(Location loc, std::string value) : TokenBase(loc, value){}

String::String(Location loc, std::string value) : TokenBase(loc, value){}

MemArgBase::MemArgBase(Location loc, std::string key, std::string value) : TokenBase(loc, value), key(key){}

template<> u32_t Token::MemArgBase::unpack<u32_t>(){
    return std::stoul(value, nullptr, (value.find("0x") == std::string::npos) ? 10 : 16);
}

template<> u64_t Token::MemArgBase::unpack<u64_t>(){
    return std::stoul(value, nullptr, (value.find("0x") == std::string::npos) ? 10 : 16);
}

KeywordBase::KeywordBase(Location loc, std::string value) : TokenBase(loc, value){
    if(!keywords.contains(value)){
        throw Exception::unknown_token(loc, value);
    }
}

#define TokenGet(TOKEN) std::optional<TOKEN> TOKEN::get(TokenIter& begin, TokenHolder& holder){ \
    if(holder.has_next(begin) && std::holds_alternative<TOKEN>(*begin)){ \
        return std::get<TOKEN>(*holder.next(begin)); \
    }else{ \
        return std::nullopt; \
    } \
}

TokenGet(Token::ParenL)
TokenGet(Token::ParenR)
TokenGet(Token::Id)
TokenGet(Token::Number)
TokenGet(Token::String)

template<> u32_t Token::Number::unpack<u32_t>(){
    return std::stoul(value, nullptr, (value.find("0x") == std::string::npos) ? 10 : 16);
}

template<> u64_t Token::Number::unpack<u64_t>(){
    return std::stoul(value, nullptr, (value.find("0x") == std::string::npos) ? 10 : 16);
}

template<> i32_t Token::Number::unpack<i32_t>(){
    return std::stoi(value, nullptr, (value.find("0x") == std::string::npos) ? 10 : 16);
}

template<> i64_t Token::Number::unpack<i64_t>(){
    return std::stol(value, nullptr, (value.find("0x") == std::string::npos) ? 10 : 16);
}

template<> f32_t Token::Number::unpack<f32_t>(){
    return std::stof(value);
}

template<> f64_t Token::Number::unpack<f64_t>(){
    return std::stod(value);
}