// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <cctype>
#include <optional>
#include <functional>
#include <json.hpp>
#include <exception.hpp>
#include <Util.hpp>

using namespace Json;
using namespace WasmVM::Exception;

std::istream& Json::operator>>(std::istream& fin, Value& elem){
    std::function<void(Value&)> parse_value = [&](Value& val){
        int ch = '\0';
        // trim leading whiltespace
        while(((ch = fin.get()) == '\0') || (ch == ' ') || (ch == '\t') || (ch == '\r') || (ch == '\n'));
        switch(ch){
            case '{': // Object
                // TODO:
            break;
            case '[':{ // Array
                Value::Array& array = val.value.emplace<Value::Array>();
                do{
                    parse_value(array.emplace_back());
                    ch = fin.get();
                }while(ch == ',');
                if(ch != ']'){
                    throw Exception("JSON: unclosed array");
                }
            }break;
            case '"':{ // String
                std::string literal;
                for(ch = fin.get(); ch != '"'; ch = fin.get()){
                    if(ch == '\\'){
                        ch = fin.get();
                        switch(ch){
                            case '"':
                            case '\\':
                            case '/':
                                literal += ch;
                            break;
                            case 'b':
                                literal += '\b';
                            break;
                            case 'f':
                                literal += '\f';
                            break;
                            case 'n':
                                literal += '\n';
                            break;
                            case 'r':
                                literal += '\r';
                            break;
                            case 't':
                                literal += '\t';
                            break;
                            case 'u':{
                                std::string udigits(4, '\0');
                                fin.read(udigits.data(), 4);
                                size_t count = 0;
                                wchar_t uchar = std::stoi(udigits, &count, 16);
                                if(count != 4){
                                    throw Exception("JSON: invalid character value in string");
                                }
                                if(uchar > 0xff){
                                    literal += (char)((uchar >> 8) & 0xff);
                                }
                                literal += (char)(uchar & 0xff);
                            }break;
                            default:
                                throw Exception("JSON: unknown escape sequence in string");
                            break;
                        }
                    }else{
                        literal += ch;
                    }
                }
                val.value.emplace<Value::String>(literal);
            }break;
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
                fin.unget();
                val.value.emplace<Value::Number>(std::stod(literal));
            }break;
            case 't':{ // true
                char remain[4];
                fin.read(remain, 3);
                if(remain[0] == 'r' && remain[1] == 'u' && remain[2] == 'e'){
                    val.value.emplace<Value::Bool>(true);
                }else{
                    throw Exception(std::string("JSON: unknown token 't") + remain + "'");
                }
            }break;
            case 'f':{ // false
                char remain[5];
                fin.read(remain, 4);
                if(remain[0] == 'a' && remain[1] == 'l' && remain[2] == 's' && remain[3] == 'e'){
                    val.value.emplace<Value::Bool>(false);
                }else{
                    throw Exception(std::string("JSON: unknown token 'f") + remain + "'");
                }
            }break;
            case 'n':{ // null
                char remain[4];
                fin.read(remain, 3);
                if(remain[0] == 'u' && remain[1] == 'l' && remain[2] == 'l'){
                    val.value.emplace<Value::Null>();
                }else{
                    throw Exception(std::string("JSON: unknown token 'n") + remain + "'");
                }
            }break;
            default:
                throw Exception(std::string("JSON: unknown character '") + (char)ch + "'");
            break;
        }
        // trim trailing whiltespace
        while(((ch = fin.get()) == '\0') || (ch == ' ') || (ch == '\t') || (ch == '\r') || (ch == '\n'));
        fin.unget();
    };
    parse_value(elem);
    if(fin.get() != std::istream::traits_type::eof()){
        throw Exception("JSON: root value only allowed one element");
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
        },
        [&](Value::String data){
            stream << '"' << data << '"';
        },
        [&](Value::Array data){
            stream << "[";
            for(auto it = data.begin(); it != data.end(); it = std::next(it)){
                if(it != data.begin()){
                    stream << ", ";
                }
                stream << *it;
            }
            stream << "]";
        }
    }, val.value);
    return stream;
}