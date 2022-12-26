// Copyright 2022 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "Lexer.hpp"

#include <exception.hpp>

using namespace WasmVM;

Stream::Stream(std::istream& stream) : stream(stream){}

static char _get(std::stack<char> &buf, std::istream &stream){
    if(buf.empty()){
        return stream.get();
    }else{
        char ch = buf.top();
        buf.pop();
        return ch;
    }
}

static char _peek(std::stack<char> &buf, std::istream &stream){
    if(buf.empty()){
        return stream.peek();
    }else{  
        return buf.top();
    }
}

static bool _eof(std::stack<char> &buf, std::istream &stream) {
    return buf.empty() && stream.eof();
}

static void skip_comment(std::stack<char> &buf, std::istream &stream){
    char ch;
    while(true){
        switch(_peek(buf, stream)){
            // Line comment
            case ';':
                ch = _get(buf, stream);
                if(_peek(buf, stream) == ';'){
                    while(!_eof(buf, stream) && ((ch = _get(buf, stream)) != '\n'));
                    buf.push(ch);
                }else{
                    buf.push(ch);
                    return;
                }
            break;
            // Block comment
            case '(':
                ch = _get(buf, stream);
                if(_peek(buf, stream) == ';'){
                    ssize_t depth = 1;
                    ch = _get(buf, stream);
                    while(!_eof(buf, stream)){
                        ch = _get(buf, stream);
                        char pe = _peek(buf, stream);
                        if((ch == '(') && (pe == ';')){
                            depth += 1;
                            ch = _get(buf, stream);
                        }else if((ch == ';') && (pe == ')')){
                            depth -= 1;
                            ch = _get(buf, stream);
                            if(depth == 0){
                                return;
                            }
                        }
                    }
                    throw WasmVM::exception::eof("block comment reach EOF without close");
                }else{
                    buf.push(ch);
                    return;
                }
            break;
            // Skip extra spaces
            case ' ':
            case '\n':
            case '\t':
            case '\r':{
                ch = _get(buf, stream);
                char pe = _peek(buf, stream);
                if(pe == ';'){
                    break;
                }
                if(_eof(buf, stream) || ((pe != ' ') && (pe != '\t') && (pe != '\n') && (pe != '\r'))){
                    buf.push((ch == '\n') ? ' ' : ch);
                    return;
                }
                for(; !_eof(buf, stream) && ((pe == ' ') || (pe == '\t') || (pe == '\n') || (pe == '\r')); ch = _get(buf, stream), pe = _peek(buf, stream));
                buf.push((ch == '\n') ? ' ' : ch);
            }break;
            default:
            return;
        }
    }
}

char Stream::get(){
    skip_comment(buf, stream);
    return _get(buf, stream);
}

char Stream::peek(){
    skip_comment(buf, stream);
    return _peek(buf, stream);
}

Slicer::Slicer(std::istream& stream) : Stream(stream){
    operator++();
}

const std::string& Slicer::operator*() const{
    return current;
}

Slicer::operator bool(){
    return !current.empty();
}

Slicer& Slicer::operator++(){
    static const int eof_char = std::char_traits<char>::eof();
    current.clear();
    char ch = get();
    // Trim leading spaces
    while((ch == ' ') || (ch == '\t') || (ch == '\r') || (ch == '\n')){
        ch = get();
    }
    // Parenthesis
    if((ch == '(') || (ch == ')')){
        current = ch;
        return *this;
    }
    // Other characters
    while(ch != eof_char){
        switch(ch){
            case ' ':
            case '\t':
            case '\r':
            case '\n':
            case '(':
            case ')':
                return *this;
            default:
                current += ch;
            break;
        }
        ch = get();
    }
    return *this;
}

Slicer Slicer::operator++(int){
    Slicer res = *this;
    ++(*this);
    return res;
}