// Copyright 2022 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <WasmVM.hpp>

#include <exception.hpp>
#include "parse.hpp"
#include "Keywords.hpp"

#include <list>
#include <string>
#include <utility>

namespace WasmVM {

WasmModule module_parse(std::string src){
    std::list<TokenType> tokens = tokenize(src);
    return WasmModule();
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

std::list<TokenType> tokenize(std::string_view src){
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
                    tokens.emplace_back(Token::Paren<'('>(location));
                    next_char(it, current);
                }
            break;
            // Right parenthesis
            case ')':
                tokens.emplace_back(Token::Paren<')'>(location));
                next_char(it, current);
            break;
            // Id
            case '$':{
                std::string seq {*it};
                for(next_char(it, current); (it != src.end()) && (std::string(" \n\t\r()").find(*it) != std::string::npos); next_char(it, location)){
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
                    if(keywords.contains(seq)){
                        tokens.emplace_back(Token::Keyword(location, seq));
                    }else{
                        throw Exception::unknown_token(location, seq);
                    }
                }
            }break;
        }
    }
    return tokens;
}

}