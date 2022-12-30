// Copyright 2022 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "Lexer.hpp"

#include <exception.hpp>
#include <sstream>

using namespace WasmVM;

static std::istringstream null_str;

Lexer::SourceIter::SourceIter() : stream(null_str), pos(end_pos), lineno(0), colno(0), newline(false){}

Lexer::SourceIter::SourceIter(std::istream& stream) : stream(stream), pos(0), lineno(1), colno(0), newline(false){
    advance();
}

Lexer::SourceIter::reference Lexer::SourceIter::operator*() const {
    if(pos == end_pos){
        return eof;
    }
    return buf.front().first;
}

Lexer::SourceIter::operator bool(){
    return pos != end_pos;
}

Lexer::SourceIter& Lexer::SourceIter::operator++(){
    if(pos != end_pos){
        pos += 1;
        buf.pop();
        if(buf.empty()){
            advance();
        }
    }
    return *this;
}

std::pair<size_t, size_t>& Lexer::SourceIter::location(){
    return buf.front().second;
}

void Lexer::SourceIter::advance(){
    std::istream& str = stream.get();
    for (traits_type::int_type ch = get(); ch != traits_type::eof(); ){
        switch(ch){
            // String
            case '\"':{
                buf.emplace(ch, std::pair<size_t, size_t>{lineno, colno});
                bool escape = false;
                for(ch = get(); (ch != eof) && (escape || (ch != '\"')); ch = get()){
                    if((ch == '\\') && !escape){
                        escape = true;
                    }else{
                        escape = false;
                    }
                    buf.emplace(ch, std::pair<size_t, size_t>{lineno, colno});
                }
                if(ch != eof){
                    buf.emplace(ch, std::pair<size_t, size_t>{lineno, colno});
                    return;
                }else{
                    throw Exception::string_not_close({lineno, colno});
                }
            }break;
            // Block comment
            case '(':
                if(str.peek() == ';'){
                    get();
                    size_t depth = 1;
                    for(ch = get(); (depth > 0) && (ch != eof); ch = get()){
                        char pe = str.peek();
                        if((ch == '(') && (pe == ';')){
                            depth += 1;
                            get();
                        }else if((ch == ';') && (pe == ')')){
                            depth -= 1;
                            get();
                        }
                    }
                    if(depth != 0){
                        throw Exception::block_comment_not_close({lineno, colno});
                    }
                }else{
                    buf.emplace(ch, std::pair<size_t, size_t>{lineno, colno});
                    ch = get();
                }
            break;
            // Line comment
            case ';':
                if(str.peek() == ';'){
                    get();
                    for(ch = get(); (ch != eof) && (ch != '\n'); ch = get());
                }
            break;
            // Skip extra spaces
            case ' ':
            case '\n':
            case '\t':
            case '\r':
                buf.emplace(' ', std::pair<size_t, size_t>{lineno, colno});
                do{
                    ch = get();
                }while((ch == ' ') || (ch == '\n') || (ch == '\t') || (ch == '\r'));
            break;
            default:
                buf.emplace(ch, std::pair<size_t, size_t>{lineno, colno});
                return;
        }
    }
    pos = end_pos;
}

bool Lexer::SourceIter::operator==(const SourceIter& it) const {
    return pos == it.pos;
}
bool Lexer::SourceIter::operator!=(const SourceIter& it) const {
    return pos != it.pos;
}

Lexer::SourceIter::traits_type::int_type Lexer::SourceIter::get(){
    Lexer::SourceIter::traits_type::int_type ch = stream.get().get();
    if(newline){
        lineno += 1;
        colno = 0;
        newline = false;
    }
    if(ch == '\n'){
        newline = true;
    }
    if(ch != eof){
        colno += 1;
    }
    return ch;
}

// Lexer::Lexer(std::istream& stream) : LexerSlicer(stream){}

// std::optional<std::reference_wrapper<Token>> Lexer::operator[](size_t index) {
//     if(tokens.size() <= index){
//         for()
//     }else{
//         return tokens[index];
//     }
// }

// std::vector<Token> Lexer::consume(size_t index) {
//     return std::vector<Token>();
// }
