// Copyright 2022 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "Lexer.hpp"

#include <exception.hpp>
#include <cassert>

using namespace WasmVM;

SourceIter::SourceIter(std::istream& stream) : stream(stream), pos(0), lineno(1), colno(0), newline(false){
    advance();
}

SourceIter::reference SourceIter::operator*() const {
    if(pos == end_pos){
        return eof;
    }
    return buf.front().first;
}

SourceIter::operator bool(){
    return pos != end_pos;
}

SourceIter& SourceIter::operator++(){
    if(pos != end_pos){
        pos += 1;
        buf.pop();
        if(buf.empty()){
            advance();
        }
    }
    return *this;
}

std::pair<size_t, size_t>& SourceIter::location(){
    return buf.front().second;
}

void SourceIter::advance(){
    std::istream& str = stream.get();
    for (traits_type::int_type ch = get(); ch != traits_type::eof(); ){
        switch(ch){
            // Block comment
            case '(':
                if(str.peek() == ';'){
                    get();
                    size_t depth = 1;
                    for(ch = get(); (depth > 0) && (ch != traits_type::eof()); ch = get()){
                        char pe = str.peek();
                        if((ch == '(') && (pe == ';')){
                            depth += 1;
                            get();
                        }else if((ch == ';') && (pe == ')')){
                            depth -= 1;
                            get();
                        }
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

bool SourceIter::operator==(const SourceIter& it) const {
    return pos == it.pos;
}
bool SourceIter::operator!=(const SourceIter& it) const {
    return pos != it.pos;
}

SourceIter::traits_type::int_type SourceIter::get(){
    SourceIter::traits_type::int_type ch = stream.get().get();
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

// SourceIter operator++(int);

// static void _skip_comment(std::stack<char> &buf, std::istream &stream){
//     char ch;
//     while(true){
//         switch(_peek(buf, stream)){
//             // Block comment
//             case '(':
//                 ch = _get(buf, stream);
//                 if(_peek(buf, stream) == ';'){
//                     ssize_t depth = 1;
//                     ch = _get(buf, stream);
//                     while(!_eof(buf, stream)){
//                         ch = _get(buf, stream);
//                         char pe = _peek(buf, stream);
//                         if((ch == '(') && (pe == ';')){
//                             depth += 1;
//                             ch = _get(buf, stream);
//                         }else if((ch == ';') && (pe == ')')){
//                             depth -= 1;
//                             ch = _get(buf, stream);
//                             if(depth == 0){
//                                 return;
//                             }
//                         }
//                     }
//                     throw WasmVM::exception::eof("block comment reach EOF without close");
//                 }else{
//                     buf.push(ch);
//                     return;
//                 }
//             break;
//             // Skip extra spaces
//             case ' ':
//             case '\n':
//             case '\t':
//             case '\r':{
//                 ch = _get(buf, stream);
//                 char pe = _peek(buf, stream);
//                 if(pe == ';'){
//                     break;
//                 }
//                 if(_eof(buf, stream) || ((pe != ' ') && (pe != '\t') && (pe != '\n') && (pe != '\r'))){
//                     buf.push((ch == '\n') ? ' ' : ch);
//                     return;
//                 }
//                 for(; !_eof(buf, stream) && ((pe == ' ') || (pe == '\t') || (pe == '\n') || (pe == '\r')); ch = _get(buf, stream), pe = _peek(buf, stream));
//                 buf.push((ch == '\n') ? ' ' : ch);
//             }break;
//             default:
//             return;
//         }
//     }
// }

// char LexerStream::get(bool skip_comment){
//     if(skip_comment){
//         _skip_comment(buf, stream);
//     }
//     int res = _get(buf, stream);
//     if(res != std::char_traits<std::istream>::eof()){
//         pos += 1;
//     }
//     return res;
// }

// char LexerStream::peek(bool skip_comment){
//     if(skip_comment){
//         _skip_comment(buf, stream);
//     }
//     return _peek(buf, stream);
// }

// void LexerStream::put(char ch){
//     buf.push(ch);
//     pos -= 1;
// }

// std::ptrdiff_t LexerStream::seek() const{
//     return pos;
// }

// LexerSlicer::LexerSlicer(std::istream& stream) : LexerStream(stream){
//     next();
// }

// const std::string& LexerSlicer::operator*() const{
//     return current;
// }

// LexerSlicer::operator bool(){
//     return !current.empty();
// }

// const std::string& LexerSlicer::operator++(){
//     static const int eof_char = std::char_traits<char>::eof();
//     current.clear();
//     char ch = get();
//     // Trim leading spaces
//     while((ch == ' ') || (ch == '\t') || (ch == '\r') || (ch == '\n')){
//         ch = get();
//     }
//     // Parenthesis
//     if((ch == '(') || (ch == ')')){
//         current = ch;
//         return current;
//     }
//     // Other characters
//     while(ch != eof_char){
//         switch(ch){
//             case '(':
//             case ')':
//                 put(ch);
//             case ' ':
//             case '\t':
//             case '\r':
//             case '\n':
//                 return current;
//             default:
//                 current += ch;
//             break;
//         }
//         ch = get();
//     }
//     return current;
// }

// LexerSlicer& LexerSlicer::next(void){
//     ++(*this);
//     return *this;
// }

// std::string LexerSlicer::operator++(int){
//     std::string res = current;
//     next();
//     return res;
// }

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
