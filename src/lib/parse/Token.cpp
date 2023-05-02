// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "parse.hpp"

#include <algorithm>
#include <regex>
#include <ranges>
#include <cctype>

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
Exception::invalid_character::invalid_character(Token::Location location, std::string message) :
    Parse(std::string("invalid character") + message, location) {}

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

static int next_char(std::istream& stream, Token::Location& loc){
    int ch = stream.get();
    if(ch == '\n'){
        loc.first += 1;
        loc.second = 1;
    }else{
        loc.second += 1;
    }
    return ch;
}
std::list<TokenType> WasmVM::tokenize(std::istream& stream){
    std::list<TokenType> tokens;
    Token::Location current {1, 1};
    for(int ch = stream.get(); ch != std::istream::traits_type::eof() ; ){
        Token::Location location = current;
        switch(ch){
            // Line comment
            case ';':
                if(stream.get() == ';'){
                    for(ch = next_char(stream, current); (ch != std::istream::traits_type::eof()) && (ch != '\n'); ch = next_char(stream, current));
                }else{
                    throw Exception::unknown_token(location, ";");
                }
            break;
            // Block comment
            case '(':
                if(stream.peek() == ';'){
                    next_char(stream, current);
                    size_t depth = 1;
                    for(ch = next_char(stream, current); (depth > 0) && (ch != std::istream::traits_type::eof()); ch = next_char(stream, current)){
                        char pe = stream.peek();
                        if((ch == '(') && (pe == ';')){
                            depth += 1;
                            next_char(stream, current);
                        }else if((ch == ';') && (pe == ')')){
                            depth -= 1;
                            next_char(stream, current);
                        }
                    }
                    if(depth != 0){
                        throw Exception::block_comment_not_close(location);
                    }
                }else{
                    // Left parenthesis
                    tokens.emplace_back(Token::ParenL(location));
                    ch = next_char(stream, current);
                }
            break;
            // Right parenthesis
            case ')':
                tokens.emplace_back(Token::ParenR(location));
                ch = next_char(stream, current);
            break;
            // Id
            case '$':{
                std::string seq {(char)ch};
                for(ch = next_char(stream, current); (ch != std::istream::traits_type::eof()) && (std::string(" \n\t\r()").find(ch) == std::string::npos); ch = next_char(stream, current)){
                    seq += ch;
                }
                tokens.emplace_back(Token::Id(location, seq));
            }break;
            // String
            case '\"':{
                std::string seq {(char)ch};
                bool escape = false;
                for(ch = next_char(stream, current); (ch != std::istream::traits_type::eof()) && (escape || (ch != '\"')); ch = next_char(stream, current)){
                    if(ch < 0x20){
                        throw Exception::invalid_character(location, ": control characters can only be specified by escape sequence");
                    }
                    if((ch == '\\') && !escape){
                        escape = true;
                        continue;
                    }else if(escape){
                        if(std::isxdigit(ch) && stream.peek() != std::istream::traits_type::eof() && std::isxdigit(stream.peek())){
                            std::string valstr {'0', 'x', (char)ch, (char)stream.peek()};
                            next_char(stream, current);
                            seq += (unsigned char)std::stoul(valstr, nullptr, 16);
                        }else if(ch == 'u' && stream.peek() != std::istream::traits_type::eof() && stream.peek() == '{'){
                            std::string valstr = "0x";
                            for(next_char(stream, current), ch = next_char(stream, current);
                                (ch != std::istream::traits_type::eof()) && (ch != '}'); 
                                ch = next_char(stream, current)
                            ){
                                valstr += ch;
                            }
                            static std::regex hexnum("0x[0-9a-fA-F](_?[0-9a-fA-F])*");
                            if(std::regex_match(valstr, hexnum)){
                                valstr.erase(std::remove(valstr.begin(), valstr.end(), '_'), valstr.end());
                                unsigned long value = std::stoul(valstr, nullptr, 16);
                                if((value >= 0xD800 && value < 0xE000) || value >= 0x110000){
                                    throw Exception::invalid_character(location, ": invalid unicode value");
                                }
                                if(value > 0xffff){
                                    seq += (unsigned char)((value >> 16) & 0xff);
                                }
                                if(value > 0xff){
                                    seq += (unsigned char)((value >> 8) & 0xff);
                                }
                                seq += (unsigned char)(value & 0xff);
                            }else{
                                throw Exception::invalid_character(location, ": invalid unicode format");
                            }
                        }else{
                            switch(ch){
                                case 't':
                                    seq += '\t';
                                break;
                                case 'n':
                                    seq += '\n';
                                break;
                                case 'r':
                                    seq += '\r';
                                break;
                                case '"':
                                    seq += '"';
                                break;
                                default:
                                    seq += ch;
                                break;
                            }
                        }
                        escape = false;
                    }else{
                        seq += ch;
                    }
                }
                if(ch == std::istream::traits_type::eof()){
                    throw Exception::string_not_close(location);
                }else{
                    seq += ch;
                    tokens.emplace_back(Token::String(location, seq));
                    ch = next_char(stream, current);
                }
            } break;
            // White spaces
            case ' ':
            case '\n':
            case '\t':
            case '\r':
                ch = next_char(stream, current);
            break;
            // Others
            default:{
                std::string seq {(char)ch};
                for(ch = next_char(stream, current);
                    (ch != std::istream::traits_type::eof()) && (std::string(" \n\t\r()").find(ch) == std::string::npos);
                    ch = next_char(stream, current)
                ){
                    seq += ch;
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