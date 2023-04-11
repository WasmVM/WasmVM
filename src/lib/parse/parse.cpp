// Copyright 2022 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <exception.hpp>
#include "nodes.hpp"

#include <list>
#include <string>
#include <utility>
#include <regex>
#include <ranges>
#include <optional>

using namespace WasmVM;

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

std::optional<Parse::Index> Parse::Index::get(TokenIter& begin, TokenHolder& holder){
    std::list<TokenType>::iterator it = begin;
    auto syntax = OneOf<Token::Number, Token::Id>::get(it, holder);
    if(syntax){
        begin = it;
        return Parse::Index(syntax.value());
    }
    return std::nullopt;
}


using namespace Exception;
string_not_close::string_not_close(Token::Location location) : 
    Parse("string not close", location) {}
block_comment_not_close::block_comment_not_close(Token::Location location) : 
    Parse("block comment not close", location) {}
unknown_token::unknown_token(Token::Location location, std::string token) : 
    Parse(std::string("unknown token '") + token + "'", location) {}
unknown_identifier::unknown_identifier(Token::Location location, std::string message) : 
    Parse(std::string("unknown identifier") + message, location) {}
invalid_functype::invalid_functype(Token::Location location, std::string message) : 
    Parse(std::string("invalid functype") + message, location) {}
invalid_limit::invalid_limit(Token::Location location, std::string message) : 
    Parse(std::string("invalid limit") + message, location) {}
duplicated_identifier::duplicated_identifier(Token::Location location, std::string message) :
    Parse(std::string("duplicated identifier") + message, location) {}
block_id_mismatch::block_id_mismatch(Token::Location location, std::string message) :
    Parse(std::string("block id mismatch") + message, location) {}
index_out_of_range::index_out_of_range(Token::Location location, std::string message) :
    Parse(std::string("index out of range") + message, location) {}
invalid_folded_instruction::invalid_folded_instruction(Token::Location location) : 
    Parse("invalid folded instruction", location) {}
invalid_immediate_value::invalid_immediate_value(Token::Location location, std::string message) :
    Parse(std::string("invalid immediate value") + message, location) {}
unexpected_eof::unexpected_eof() :
    Exception(std::string("unexpected EOF")) {}