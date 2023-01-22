// Copyright 2022 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <exception.hpp>
#include "parse.hpp"

#include <list>
#include <string>
#include <utility>

using namespace WasmVM;

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
        switch(Token::Location location = current; *it){
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
                for(next_char(it, current); (it != src.end()) && (std::string(" \n\t\r()").find(*it) == std::string::npos); next_char(it, location)){
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
                std::optional<Token::Number> number = Token::Number::create(location, seq);
                if(number){
                    tokens.emplace_back(*number);
                }else{
                    tokens.emplace_back(Token::KeywordBase(location, seq));
                }
            }break;
        }
    }
    return tokens;
}

using namespace Exception;
string_not_close::string_not_close(Token::Location location) : 
    Parse("string not close", location) {}
block_comment_not_close::block_comment_not_close(Token::Location location) : 
    Parse("block comment not close", location) {}
unknown_token::unknown_token(Token::Location location, std::string token) : 
    Parse(std::string("unknown token '") + token + "'", location) {}
unexpected_keyword::unexpected_keyword(Token::Location location, std::string token, std::string expected) :
    Parse(std::string("unexpected keyword '") + token + "', expected '" + expected + "'", location) {}
invalid_functype::invalid_functype(Token::Location location, std::string message) : 
    Parse(std::string("invalid functype" + message), location) {}
syntax_error::syntax_error(std::string message) : 
    Exception(std::string("syntax error" + message)) {}