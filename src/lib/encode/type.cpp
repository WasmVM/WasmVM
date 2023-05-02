// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "encode.hpp"

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

template<> Section::Stream& Section::Stream::operator<< <i32_t>(i32_t value){
    return this->operator<<((u32_t)value);
}

template<> Section::Stream& Section::Stream::operator<< <i64_t>(i64_t value){
    return this->operator<<((u64_t)value);
}

template<> Section::Stream& Section::Stream::operator<< <std::string>(std::string value){
    *this << (u32_t)value.size();
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

Section::Stream& Section::Stream::operator<<(std::stringstream& buffer){
    return *this << buffer.str();
}

Encode::Type::Type(const std::vector<FuncType>& types) : Section((byte_t)0x01){
    Encode::Section::Stream stream(buffer);
    stream << types;
}