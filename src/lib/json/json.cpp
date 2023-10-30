// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <json.hpp>
#include <exception.hpp>
#include <Util.hpp>

using namespace WasmVM;
using namespace Json;

std::istream& Json::operator>>(std::istream& fin, Value& val){
    for(int ch = fin.get(); ch != std::istream::traits_type::eof(); ch = fin.get()){
        switch(ch){
            case '\0':
            case ' ':
            case '\t':
            case '\r':
            case '\n': // whitespace
            break;
            case '{': // Object
                // TODO:
            break;
            case '[': // Array
                // TODO:
            break;
            case '"': // String
                // TODO:
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
            case '-':{ // Number
                std::string literal;
                literal += ch; ch = fin.get();
                while((ch >= '0') && (ch <= '9')){ // integer
                    literal += ch; ch = fin.get();
                }
                if(ch == '.'){ // fraction
                    literal += ch; ch = fin.get();
                    while((ch >= '0') && (ch <= '9')){
                        literal += ch; ch = fin.get();
                    }
                }
                if(ch == 'e' || ch == 'E'){ // exponent
                    literal += ch; ch = fin.get();
                    if(ch == '-' || ch == '+'){ // sign
                        literal += ch; ch = fin.get();
                    }
                    while((ch >= '0') && (ch <= '9')){ // digits
                        literal += ch; ch = fin.get();
                    }
                }
                val.value.emplace<Value::Number>(std::stod(literal));
            }break;
            case 't':{ // true
                char remain[4];
                fin.read(remain, 3);
                if(remain[0] == 'r' && remain[1] == 'u' && remain[2] == 'e'){
                    val.value.emplace<Value::Bool>(true);
                }else{
                    throw Exception::Exception(std::string("unknown token 't") + remain + "' while parsing JSON");
                }
            }break;
            case 'f':{ // false
                char remain[5];
                fin.read(remain, 4);
                if(remain[0] == 'a' && remain[1] == 'l' && remain[2] == 's' && remain[3] == 'e'){
                    val.value.emplace<Value::Bool>(false);
                }else{
                    throw Exception::Exception(std::string("unknown token 'f") + remain + "' while parsing JSON");
                }
            }break;
            case 'n':{// null
                char remain[4];
                fin.read(remain, 3);
                if(remain[0] == 'u' && remain[1] == 'l' && remain[2] == 'l'){
                    val.value.emplace<Value::Null>();
                }else{
                    throw Exception::Exception(std::string("unknown token 'n") + remain + "' while parsing JSON");
                }
            }break;
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

std::ostream& Json::operator<<(std::ostream& stream, const Value& val){
    std::visit(overloaded {
        [&](Value::Null){
            stream << "null";
        },
        [&](Value::Bool data){
            stream << (data ? "true" : "false");
        },
        [&](Value::Number data){
            stream << data;
        }
    }, val.value);
    return stream;
}