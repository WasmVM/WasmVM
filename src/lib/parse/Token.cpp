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
    size_t len;
    u32_t imm = std::stoul(value, &len, (value.find("0x") == std::string::npos) ? 10 : 16);
    if(len != value.length()){
        throw Exception::invalid_immediate_value(location, " '" + value + "'");
    }
    return imm;
}

template<> u64_t Token::MemArgBase::unpack<u64_t>(){
    size_t len;
    u64_t imm = std::stoul(value, &len, (value.find("0x") == std::string::npos) ? 10 : 16);
    if(len != value.length()){
        throw Exception::invalid_immediate_value(location, " '" + value + "'");
    }
    return imm;
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
    size_t len;
    u32_t imm = std::stoul(value, &len, (value.find("0x") == std::string::npos) ? 10 : 16);
    if(len != value.length()){
        throw Exception::invalid_immediate_value(location, " '" + value + "'");
    }
    return imm;
}

template<> u64_t Token::Number::unpack<u64_t>(){
    size_t len;
    u64_t imm = std::stoul(value, &len, (value.find("0x") == std::string::npos) ? 10 : 16);
    if(len != value.length()){
        throw Exception::invalid_immediate_value(location, " '" + value + "'");
    }
    return imm;
}

template<> i32_t Token::Number::unpack<i32_t>(){
    size_t len;
    i32_t imm = std::stoi(value, &len, (value.find("0x") == std::string::npos) ? 10 : 16);
    if(len != value.length()){
        throw Exception::invalid_immediate_value(location, " '" + value + "'");
    }
    return imm;
}

template<> i64_t Token::Number::unpack<i64_t>(){
    size_t len;
    i64_t imm = std::stol(value, &len, (value.find("0x") == std::string::npos) ? 10 : 16);
    if(len != value.length()){
        throw Exception::invalid_immediate_value(location, " '" + value + "'");
    }
    return imm;
}

template<> f32_t Token::Number::unpack<f32_t>(){
    size_t len;
    f32_t imm = std::stof(value, &len);
    if(len != value.length()){
        throw Exception::invalid_immediate_value(location, " '" + value + "'");
    }
    return imm;
}

template<> f64_t Token::Number::unpack<f64_t>(){
    size_t len;
    f64_t imm = std::stod(value, &len);
    if(len != value.length()){
        throw Exception::invalid_immediate_value(location, " '" + value + "'");
    }
    return imm;
}