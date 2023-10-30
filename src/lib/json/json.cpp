// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <json.hpp>
#include <exception.hpp>
#include <Util.hpp>

using namespace WasmVM;
using namespace Json;

std::istream& Json::operator>>(std::istream& fin, Value& value){
    for(int ch = fin.get(); ch != std::istream::traits_type::eof() ;){
        switch(ch){
            case '\0':
            case ' ':
            case '\t':
            case '\r':
            case '\n': // whitespace
            break;
            case '{': // Object

            break;
            case '[': // Array

            break;
            case '"': // String

            break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '-': // Number
            break;
            case 't': // true
            break;
            case 'f': // false
            break;
            case 'n': // null

            break;
            default:
                throw Exception::Exception(std::string("unknown character '") + (char)ch + "' while parsing JSON");
            break;
        }
    }
    return fin;
}

long Json::newline::index = std::ios_base::xalloc();
std::ostream& Json::operator<<(std::ostream& stream, const newline& value){
    long& flag_no_newline = stream.iword(newline::index);
    flag_no_newline = value.toggle ? 0 : 1;
    return stream;
}

long Json::indent::index = std::ios_base::xalloc();
static void indent_callback(std::ios_base::event type, std::ios_base& ios, int){
    if(type == std::ios_base::event::erase_event){
        std::string* &indent_ptr = (std::string*&)ios.pword(Json::indent::index);
        if(indent_ptr != nullptr){
            delete indent_ptr;
            indent_ptr = nullptr;
        }
    }
}
inline static bool use_newline(std::ostream& stream){
    return stream.iword(Json::newline::index) == 0;
}
inline static std::string use_indent(std::ostream& stream){
    std::string* indent_ptr = (std::string*)stream.pword(Json::indent::index);
    return (indent_ptr == nullptr) ? Json::indent::default_indent : *indent_ptr;
}
std::ostream& Json::operator<<(std::ostream& stream, const indent& val){
    std::string* &indent_ptr = (std::string*&)stream.pword(indent::index);
    if(indent_ptr == nullptr){
        stream.register_callback(indent_callback, indent::index);
        indent_ptr = new std::string();
    }
    *indent_ptr = val.value;
    return stream;
}

std::ostream& Json::operator<<(std::ostream& stream, const Value& value){

    return stream;
}