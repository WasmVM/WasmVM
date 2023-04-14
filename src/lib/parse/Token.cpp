// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "parse.hpp"

#include <algorithm>
#include <regex>
#include <ranges>

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

Exception::Parse TokenHolder::error(){
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

Exception::unexpected_token::unexpected_token(Token::Location location, std::string message) : 
    Parse(std::string("unexpected token") + message, location) {}
Exception::unexpected_keyword::unexpected_keyword(Token::Location location, std::string token, std::string expected) :
    Parse(std::string("unexpected keyword '") + token + "', expected '" + expected + "'", location) {}

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

namespace TokenCreate {

static const std::regex decimal = std::regex("([\\+\\-]?[0-9](_?[0-9])*)((\\.([0-9](_?[0-9])*)?)?([eE][\\+\\-]?[0-9](_?[0-9])*)?)");
static const std::regex hexadecimal = std::regex("([\\+\\-]?0x[0-9a-fA-F](_?[0-9a-fA-F])*)((\\.([0-9a-fA-F](_?[0-9a-fA-F])*)?)?([pP][\\+\\-]?[0-9](_?[0-9])*)?)");
static const std::regex inf_nan = std::regex("[\\+\\-]?((inf)|(nan(:0x[0-9a-fA-F](_?[0-9a-fA-F])*)?))");

static std::optional<Token::MemArgBase> memarg(Token::Location loc, std::string str){
    static const std::regex keypair("([^=]+)=(.*)");
    std::smatch result;
    if(std::regex_match(str, result, keypair)){
        std::string val(result.str(2));
        if(std::regex_match(val, decimal) || std::regex_match(val, hexadecimal)){
            val.erase(std::remove(val.begin(), val.end(), '_'), val.end());
            return Token::MemArgBase(loc, result.str(1), val);
        }
    }
    return std::nullopt;
}
static std::optional<Token::Number> number(Token::Location loc, std::string str){

    if(std::regex_match(str, decimal)
        || std::regex_match(str, hexadecimal)
        || std::regex_match(str, inf_nan)
    ){
        str.erase(std::remove(str.begin(), str.end(), '_'), str.end());
        return Token::Number(loc, str);
    }
    return std::nullopt;
}

}

static void next_char(std::string_view::const_iterator& it, Token::Location& loc){
    if(*it == '\n'){
        loc.first += 1;
        loc.second = 1;
    }else{
        loc.second += 1;
    }
    ++it;
}
std::list<TokenType> WasmVM::tokenize(std::string_view src){
    std::list<TokenType> tokens;
    Token::Location current {1, 1};
    for(auto it = src.begin(); it != src.end(); ){
        Token::Location location = current;
        switch(*it){
            // Line comment
            case ';':
                if(*std::next(it) == ';'){
                    for(next_char(it, current); (it != src.end()) && (*it != '\n'); next_char(it, current));
                }else{
                    throw Exception::unknown_token(location, ";");
                }
            break;
            // Block comment
            case '(':
                if(*std::next(it) == ';'){
                    next_char(it, current);
                    size_t depth = 1;
                    for(next_char(it, current); (depth > 0) && (it != src.end()); next_char(it, current)){
                        char pe = *std::next(it);
                        if((*it == '(') && (pe == ';')){
                            depth += 1;
                            next_char(it, current);
                        }else if((*it == ';') && (pe == ')')){
                            depth -= 1;
                            next_char(it, current);
                        }
                    }
                    if(depth != 0){
                        throw Exception::block_comment_not_close(location);
                    }
                }else{
                    // Left parenthesis
                    tokens.emplace_back(Token::ParenL(location));
                    next_char(it, current);
                }
            break;
            // Right parenthesis
            case ')':
                tokens.emplace_back(Token::ParenR(location));
                next_char(it, current);
            break;
            // Id
            case '$':{
                std::string seq {*it};
                for(next_char(it, current); (it != src.end()) && (std::string(" \n\t\r()").find(*it) == std::string::npos); next_char(it, current)){
                    seq += *it;
                }
                tokens.emplace_back(Token::Id(location, seq));
            }break;
            // String
            case '\"':{
                std::string seq {*it};
                bool escape = false;
                for(next_char(it, current); (it != src.end()) && (escape || (*it != '\"')); next_char(it, current)){
                    if((*it == '\\') && !escape){
                        escape = true;
                    }else{
                        escape = false;
                    }
                    seq += *it;
                }
                if(it == src.end()){
                    throw Exception::string_not_close(location);
                }else{
                    seq += *it;
                    tokens.emplace_back(Token::String(location, seq));
                    next_char(it, current);
                }
            } break;
            // White spaces
            case ' ':
            case '\n':
            case '\t':
            case '\r':
                next_char(it, current);
            break;
            // Others
            default:{
                std::string seq {*it};
                for(next_char(it, current); (it != src.end()) && (std::string(" \n\t\r()").find(*it) == std::string::npos); next_char(it, current)){
                    seq += *it;
                }
                std::optional<Token::Number> number = TokenCreate::number(location, seq);
                if(number){
                    tokens.emplace_back(*number);
                }else{
                    std::optional<Token::MemArgBase> memarg = TokenCreate::memarg(location, seq);
                    if(memarg){
                        tokens.emplace_back(*memarg);
                    }else{
                        tokens.emplace_back(Token::KeywordBase(location, seq));
                    }
                }
            }break;
        }
    }
    return tokens;
}