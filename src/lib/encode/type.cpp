// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "encode.hpp"

#include <deque>

using namespace WasmVM;
using namespace Encode;

template<> Section::Stream& Section::Stream::operator<< <byte_t>(byte_t value){
    stream.put((char)value);
    return *this;
}

template<> Section::Stream& Section::Stream::operator<< <u64_t>(u64_t value){
    do {
        byte_t byte = (byte_t)(value & 0x7f);
        value >>= 7;
        if(value > 0){
            byte |= (byte_t)0x80;
        }
        stream.put((char)byte);
    } while(value > 0);
    return *this;
}

template<> Section::Stream& Section::Stream::operator<< <u32_t>(u32_t value){
    return this->operator<<((u64_t)value);
}

template<> Section::Stream& Section::Stream::operator<< <i64_t>(i64_t value){
    std::deque<byte_t> bytes;
    for(size_t i = 0; i < 10; ++i){
        bytes.emplace_front((byte_t)(value & 0x7f));
        value >>= 7;
    }; 
    while((bytes.size() > 1) && (bytes[0] == bytes[1]) && ((bytes[0] == byte_t {0x00}) || (bytes[0] == byte_t {0x7f}))){
        bytes.pop_front();
    }
    while(bytes.size() > 1){
        *this << (bytes.back() | byte_t {0x80});
        bytes.pop_back();
    }
    return *this << (bytes.back() & byte_t {0x7f});
}

template<> Section::Stream& Section::Stream::operator<< <i32_t>(i32_t value){
    std::deque<byte_t> bytes;
    for(size_t i = 0; i < 5; ++i){
        bytes.emplace_front((byte_t)(value & 0x7f));
        value >>= 7;
    }; 
    while((bytes.size() > 1) && (bytes[0] == bytes[1]) && ((bytes[0] == byte_t {0x00}) || (bytes[0] == byte_t {0x7f}))){
        bytes.pop_front();
    }
    while(bytes.size() > 1){
        *this << (bytes.back() | byte_t {0x80});
        bytes.pop_back();
    }
    return *this << (bytes.back() & byte_t {0x7f});
}

template<> Section::Stream& Section::Stream::operator<< <f32_t>(f32_t value){
    stream.write((const char*)&value, sizeof(f32_t));
    return *this;
}

template<> Section::Stream& Section::Stream::operator<< <f64_t>(f64_t value){
    stream.write((const char*)&value, sizeof(f64_t));
    return *this;
}

template<> Section::Stream& Section::Stream::operator<< <std::string>(std::string value){
    *this << (u64_t)value.size();
    stream.write(value.data(), value.size());
    return *this;
}

template<> Section::Stream& Section::Stream::operator<< <WasmVM::ValueType>(WasmVM::ValueType valtype){
    switch(valtype){
        case ValueType::i32 :
            *this << (byte_t) 0x7f;
        break;
        case ValueType::i64 :
            *this << (byte_t) 0x7e;
        break;
        case ValueType::f32 :
            *this << (byte_t) 0x7d;
        break;
        case ValueType::f64 :
            *this << (byte_t) 0x7c;
        break;
        case ValueType::funcref :
            *this << (byte_t) 0x70;
        break;
        case ValueType::externref :
            *this << (byte_t) 0x6f;
        break;
    }
    return *this;
}

template<> Section::Stream& Section::Stream::operator<< <WasmVM::FuncType>(WasmVM::FuncType functype){
    *this << (byte_t) 0x60 << functype.params << functype.results;
    return *this;
}

template<> Section::Stream& Section::Stream::operator<< <WasmVM::RefType>(WasmVM::RefType reftype){
    return *this << (byte_t)((reftype == RefType::funcref) ? 0x70 : 0x6f);
}

template<> Section::Stream& Section::Stream::operator<< <WasmVM::Limits>(WasmVM::Limits limit){
    if(limit.max){
        return *this << (byte_t)0x01 << (u64_t)limit.min << (u64_t)limit.max.value();
    }else{
        return *this << (byte_t)0x00 << (u64_t)limit.min;
    }
}

Section::Stream& Section::Stream::operator<<(std::stringstream& buffer){
    std::string bytes(buffer.str());
    if(bytes.empty()){
        return *this;
    }
    return *this << bytes;
}

Encode::Type::Type(const std::vector<FuncType>& types) : Section((byte_t)0x01){
    if(!types.empty()){
        Encode::Section::Stream stream(buffer);
        stream << types;
    }
}