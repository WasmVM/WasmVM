// Copyright 2022 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <WasmVM.hpp>

#include <exception.hpp>
#include "parse.hpp"

#include <iostream>

namespace WasmVM {

struct TokenVisitor {
    template<char C>
    void operator()(Token::Paren<C>& tok){
        std::cout << "[" << tok.location.first << "," << tok.location.second << "] Paren " << tok.value << std::endl; 
    }

    void operator()(Token::Id& tok){
        std::cout << "[" << tok.location.first << "," << tok.location.second << "] Id " << tok.value << std::endl; 
    }

    void operator()(Token::String& tok){
        std::cout << "[" << tok.location.first << "," << tok.location.second << "] String " << tok.value << std::endl; 
    }

    void operator()(Token::Number& tok){
        std::cout << "[" << tok.location.first << "," << tok.location.second << "] Number " << std::endl; 
    }

    void operator()(Token::Keyword& tok){
        std::cout << "[" << tok.location.first << "," << tok.location.second << "] Keyword " << tok.value << std::endl; 
    }
};

WasmModule module_parse(std::string src){
    std::list<TokenVar> tokens = tokenize(src);
    std::cout << "\n--- Tokens ---" << std::endl;
    for(TokenVar v : tokens){
        std::visit(TokenVisitor(), v);
    }
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

std::list<TokenVar> tokenize(std::string_view src){
    std::list<TokenVar> tokens;
    Token::Location current {1, 1};
    for(auto it = src.begin(); it != src.end(); ){
        switch(Token::Location location = current; *it){
            // Line comment
            case ';':
                if(*std::next(it) == ';'){
                    for(next_char(it, current); (it != src.end()) && (*it != '\n'); next_char(it, current));
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
                    tokens.emplace_back(Token::Paren<'('>(location));
                    next_char(it, current);
                }
            break;
            // Id
            case '$':{
                std::string seq {*it};
                for(next_char(it, current); (it != src.end()) && (*it != ' ') && (*it != '(') && (*it != ')'); next_char(it, location)){
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
            default:
                std::cout << *it;
                next_char(it, current);
            break;
        }
    }
    return tokens;
}

}