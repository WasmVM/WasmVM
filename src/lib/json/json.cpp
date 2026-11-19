// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <cctype>
#include <charconv>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <optional>
#include <functional>
#include <json.hpp>
#include <Util.hpp>

using namespace Json;
using namespace WasmVM::Exception;

// The four hex digits after "\u", as a UTF-16 code unit.
static uint32_t read_code_unit(std::istream& fin){
    uint32_t unit = 0;
    for(int i = 0; i < 4; ++i){
        int ch = fin.get();
        if(!std::isxdigit(ch)){
            throw Exception("JSON: invalid \\u escape in string");
        }
        unit = (unit << 4) | (uint32_t)(std::isdigit(ch) ? (ch - '0') : (std::tolower(ch) - 'a' + 10));
    }
    return unit;
}

static void append_utf8(std::string& out, uint32_t cp){
    if(cp < 0x80){
        out += (char)cp;
    }else if(cp < 0x800){
        out += (char)(0xC0 | (cp >> 6));
        out += (char)(0x80 | (cp & 0x3F));
    }else if(cp < 0x10000){
        out += (char)(0xE0 | (cp >> 12));
        out += (char)(0x80 | ((cp >> 6) & 0x3F));
        out += (char)(0x80 | (cp & 0x3F));
    }else{
        out += (char)(0xF0 | (cp >> 18));
        out += (char)(0x80 | ((cp >> 12) & 0x3F));
        out += (char)(0x80 | ((cp >> 6) & 0x3F));
        out += (char)(0x80 | (cp & 0x3F));
    }
}

// Quote and escape a string for output: '"', '\\' and the control characters
// must be escaped; every other byte, UTF-8 included, is written as is.
static void write_string(std::ostream& stream, const std::string& data){
    static const char hex[] = "0123456789abcdef";
    stream << '"';
    for(char c : data){
        switch(c){
            case '"': stream << "\\\""; break;
            case '\\': stream << "\\\\"; break;
            case '\b': stream << "\\b"; break;
            case '\f': stream << "\\f"; break;
            case '\n': stream << "\\n"; break;
            case '\r': stream << "\\r"; break;
            case '\t': stream << "\\t"; break;
            default:
                if((unsigned char)c < 0x20){
                    stream << "\\u00" << hex[(c >> 4) & 0xF] << hex[c & 0xF];
                }else{
                    stream << c;
                }
            break;
        }
    }
    stream << '"';
}

// The shortest "%g" form that reads back as the same double. It always keeps
// a '.' or an exponent, so the output parses back as a Number, not an Integer.
static std::string format_number(double data){
    char buf[32];
    for(int precision = 15; precision <= 17; ++precision){
        std::snprintf(buf, sizeof(buf), "%.*g", precision, data);
        if(std::strtod(buf, nullptr) == data){
            break;
        }
    }
    std::string text = buf;
    if(text.find_first_of(".eEn") == std::string::npos){
        text += ".0";
    }
    return text;
}

std::istream& Json::operator>>(std::istream& fin, Value& elem){
    std::function<void(Value&)> parse_value = [&](Value& val){
        int ch = '\0';
        // trim leading whiltespace
        while(((ch = fin.get()) == '\0') || (ch == ' ') || (ch == '\t') || (ch == '\r') || (ch == '\n'));
        switch(ch){
            case '{':{ // Object
                Value::Object& object = val.value.emplace<Value::Object>();
                while(((ch = fin.get()) != '}') && (object.empty() || (ch == ','))){
                    if(object.empty()){
                        fin.unget();
                    }
                    Value obj_key;
                    parse_value(obj_key);
                    if(obj_key.type() != Value::Type::String){
                        throw Exception("JSON: object key should be string");
                    }
                    if(fin.get() != ':'){
                        throw Exception("JSON: expected ':' after object key");
                    }
                    Value& obj_value = object[obj_key.get<std::string>()];
                    parse_value(obj_value);
                };
                if(ch != '}'){
                    throw Exception("JSON: unclosed object");
                }
            }break;
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
                                uint32_t cp = read_code_unit(fin);
                                if(cp >= 0xDC00 && cp <= 0xDFFF){
                                    throw Exception("JSON: unpaired surrogate in string");
                                }
                                if(cp >= 0xD800 && cp <= 0xDBFF){
                                    // A high surrogate must be followed by "\u" and a low one.
                                    if(fin.get() != '\\' || fin.get() != 'u'){
                                        throw Exception("JSON: unpaired surrogate in string");
                                    }
                                    uint32_t low = read_code_unit(fin);
                                    if(low < 0xDC00 || low > 0xDFFF){
                                        throw Exception("JSON: unpaired surrogate in string");
                                    }
                                    cp = 0x10000 + ((cp - 0xD800) << 10) + (low - 0xDC00);
                                }
                                append_utf8(literal, cp);
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
                if(ch == '-'){ // sign
                    literal += ch; ch = fin.get();
                }
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
                const char* first = literal.data();
                const char* last = first + literal.size();
                if(literal.find_first_of(".eE") == std::string::npos){
                    Value::Integer integer = 0;
                    auto [ptr, ec] = std::from_chars(first, last, integer);
                    if(ec == std::errc() && ptr == last){
                        val.value.emplace<Value::Integer>(integer);
                        break;
                    }
                    // Out of 64-bit range: falls through to a double.
                }
                char* end = nullptr;
                double number = std::strtod(first, &end);
                if(literal.empty() || end != last){
                    throw Exception("JSON: invalid number '" + literal + "'");
                }
                if(!std::isfinite(number)){
                    throw Exception("JSON: number out of range '" + literal + "'");
                }
                val.value.emplace<Value::Number>(number);
            }break;
            case 't':{ // true
                char remain[4];
                remain[3] = '\0';
                fin.read(remain, 3);
                if(remain[0] == 'r' && remain[1] == 'u' && remain[2] == 'e'){
                    val.value.emplace<Value::Bool>(true);
                }else{
                    throw Exception(std::string("JSON: unknown token 't") + remain + "'");
                }
            }break;
            case 'f':{ // false
                char remain[5];
                remain[4] = '\0';
                fin.read(remain, 4);
                if(remain[0] == 'a' && remain[1] == 'l' && remain[2] == 's' && remain[3] == 'e'){
                    val.value.emplace<Value::Bool>(false);
                }else{
                    throw Exception(std::string("JSON: unknown token 'f") + remain + "'");
                }
            }break;
            case 'n':{ // null
                char remain[4];
                remain[3] = '\0';
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
            stream << format_number(data);
        },
        [&](Value::Integer data){
            stream << data;
        },
        [&](Value::String data){
            write_string(stream, data);
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
        },
        [&](Value::Object data){
            // Get flags
            static int level_idx = stream.xalloc();
            long &level = stream.iword(level_idx);
            std::string indent_str = use_indent(stream);
            bool has_newline = use_newline(stream);

            // Compose indent
            std::string indent_output = "";
            for(long i = 0; i < level + 1; ++i){
                indent_output += indent_str;
            }
            // Output
            stream << "{";
            if(!data.empty()){
                level += 1;
                for(auto it = data.begin(); it != data.end(); it = std::next(it)){
                    if(it != data.begin()){
                        stream << ',';
                    }
                    if(has_newline){
                        stream << std::endl << indent_output;
                    }
                    write_string(stream, it->first);
                    stream << ": " << it->second;
                }
                if(has_newline){
                    stream << std::endl;
                }
                level -= 1;
            }
            stream << "}";
        },
    }, val.value);
    return stream;
}