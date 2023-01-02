// Copyright 2022 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "Lexer.hpp"

#include <exception.hpp>
#include <sstream>
#include <string>

using namespace WasmVM;

static std::istringstream null_str;
static Lexer null_lexer;

Cursor::Cursor() : stream(null_str), pos(end_pos), lineno(0), colno(0), newline(false){}

Cursor::Cursor(std::istream& stream) : stream(stream), pos(0), lineno(1), colno(0), newline(false){
    advance();
}

Cursor::Cursor(const Cursor& cur) : stream(cur.stream), pos(cur.pos), buf(cur.buf), lineno(cur.lineno), colno(cur.colno), newline(cur.newline){}

Cursor::value_type Cursor::operator*() const {
    if(pos == end_pos){
        return Cursor::traits_type::eof();
    }
    return buf.front().first;
}

Cursor::operator bool(){
    return pos != end_pos;
}

Cursor& Cursor::operator++(){
    if(pos != end_pos){
        pos += 1;
        buf.pop();
        if(buf.empty()){
            advance();
        }
    }
    return *this;
}

Cursor Cursor::operator++(int){
    Cursor res(*this);
    operator++();
    return res;
}

std::pair<size_t, size_t>& Cursor::location(){
    return buf.front().second;
}

void Cursor::advance(){
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
            } break;
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
                while((ch == ' ') || (ch == '\n') || (ch == '\t') || (ch == '\r')){
                    ch = get();
                };
                if((ch != '(') && (ch != ';')){
                    buf.emplace(' ', std::pair<size_t, size_t>{lineno, colno});
                }
            break;
            default:
                buf.emplace(ch, std::pair<size_t, size_t>{lineno, colno});
                return;
        }
    }
    pos = end_pos;
}

bool Cursor::operator==(const Cursor& it) const {
    return pos == it.pos;
}
bool Cursor::operator!=(const Cursor& it) const {
    return pos != it.pos;
}

Cursor::traits_type::int_type Cursor::get(){
    Cursor::traits_type::int_type ch = stream.get().get();
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

Lexer::Lexer(std::istream& stream) : cursor(stream){
    std::forward_list<TokenVar>::iterator it = buf.before_begin();
    advance(it);
}

void Lexer::advance(std::forward_list<TokenVar>::iterator& it){
    // Skip leading space
    if(*cursor == ' '){
        ++cursor;
    }
    switch(*cursor){
        case '(':
            buf.emplace_after(it, Token::Paren<'('>(cursor.location()));
            ++cursor;
        break;
        case ')':
            buf.emplace_after(it, Token::Paren<')'>(cursor.location()));
            ++cursor;
        break;
        case '$':{
            Token::Location loc = cursor.location();
            std::string seq;
            for(; cursor && (*cursor != ' ') && (*cursor != '(') && (*cursor != ')'); ++cursor){
                seq += *cursor;
            }
            buf.emplace_after(it, Token::Id(loc, seq));
        }break;
        case '\"':{
            Token::Location loc = cursor.location();
            std::string seq;
            seq += *(cursor++);
            for(bool escape = false; cursor && (escape || (*cursor != '\"')); ++cursor){
                if((*cursor == '\\') && !escape){
                    escape = true;
                }else{
                    escape = false;
                }
                seq += *cursor;
            }
            if(cursor){
                seq += *(cursor++);
            }
            buf.emplace_after(it, Token::String(loc, seq));
        }break;
    }
    it = std::next(it);
}

Lexer::iterator::iterator() : lexer(null_lexer), it(null_lexer.buf.end()){}

Lexer::iterator::iterator(Lexer& lexer, std::forward_list<TokenVar>::iterator it) : lexer(lexer), it(it){}

Lexer::iterator::iterator(const Lexer::iterator& it) : lexer(it.lexer), it(it.it){}

TokenVar Lexer::iterator::operator*() const {
    return *it;
}

Lexer::iterator& Lexer::iterator::operator++() {
    Lexer& lexer = this->lexer.get();
    if(std::next(it) == lexer.buf.end()){
        lexer.advance(it);
    }
    return *this;
}
Lexer::iterator Lexer::iterator::operator++(int) {
    Lexer::iterator res(*this);
    operator++();
    return res;
}

bool Lexer::iterator::operator==(const Lexer::iterator& iter) const {
    return iter.it == it;
}
bool Lexer::iterator::operator!=(const Lexer::iterator& iter) const {
    return iter.it != it;
}

Lexer::iterator Lexer::begin(){
    return Lexer::iterator(*this, buf.begin());
}

Lexer::iterator Lexer::end(){
    return Lexer::iterator(*this, buf.end());
}

bool Lexer::empty(){
    return buf.empty();
}