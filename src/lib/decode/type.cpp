// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "decode.hpp"
#include "exception.hpp"

#include <concepts>
#include <stack>

using namespace WasmVM;
using namespace Decode;

template<> Stream& Decode::operator>> <byte_t>(Stream& stream, byte_t& value){
    if(stream.istream.eof()){
        throw Exception::unexpected_eof();
    }
    value = (byte_t)stream.istream.get();
    return stream;
}

template<typename T>
    requires (std::is_integral_v<T> && std::is_signed_v<T>)
static T read_leb(std::istream& stream){
    std::stack<byte_t> bytes;
    for(size_t i = ((sizeof(T) * 8 + 7) / 7); (i > 0); --i){
        bytes.push((byte_t)stream.get());
        if((((u8_t)bytes.top()) & 0x80) == 0){
            break;
        }
    }
    T value = (((u8_t)bytes.top()) & 0x40) ? -1 : 0;
    while(!bytes.empty()){
        value = (value << 7) & (~0x7f);
        value |= (T)bytes.top() & 0x7f;
        bytes.pop();
    }
    return value;
}

template<typename T>
    requires (std::is_integral_v<T> && std::is_unsigned_v<T>)
static T read_leb(std::istream& stream){
    T value = 0;
    for(size_t i = ((sizeof(T) * 8 + 7) / 7); (i > 0); --i){
        byte_t byte = (byte_t) stream.get();
        value = (value << 7) & (~0x7f);
        value |= (T)byte & 0x7f;
        if((((u8_t)byte) & 0x80) == 0){
            break;
        }
    }
    return value;
}

template<> Stream& Decode::operator>> <u32_t>(Stream& stream, u32_t& value){
    value = read_leb<u32_t>(stream.istream);
    return stream;
}
template<> Stream& Decode::operator>> <i32_t>(Stream& stream, i32_t& value){
    value = read_leb<i32_t>(stream.istream);
    return stream;
}
template<> Stream& Decode::operator>> <u64_t>(Stream& stream, u64_t& value){
    value = read_leb<u64_t>(stream.istream);
    return stream;
}
template<> Stream& Decode::operator>> <i64_t>(Stream& stream, i64_t& value){
    value = read_leb<i64_t>(stream.istream);
    return stream;
}
template<> Stream& Decode::operator>> <f32_t>(Stream& stream, f32_t& value){
    f32_t val;
    stream.istream.read((char*)&val, sizeof(f32_t));
    value = val;
    return stream;
}
template<> Stream& Decode::operator>> <f64_t>(Stream& stream, f64_t& value){
    f64_t val;
    stream.istream.read((char*)&val, sizeof(f64_t));
    value = val;
    return stream;
}

template<> Stream& Decode::operator>> <std::string>(Stream& stream, std::string& value){
    std::vector<byte_t> bytes;
    stream >> bytes;
    value.resize(bytes.size());
    value.assign((const char*)bytes.data(), bytes.size());
    return stream;
}

template<> Stream& Decode::operator>> <ValueType>(Stream& stream, ValueType& type){
    switch(stream.get<byte_t>()){
        case byte_t {0x7f}:
            type = ValueType::i32;
        break;
        case byte_t {0x7e}:
            type = ValueType::i64;
        break;
        case byte_t {0x7d}:
            type = ValueType::f32;
        break;
        case byte_t {0x7c}:
            type = ValueType::f64;
        break;
        case byte_t {0x70}:
            type = ValueType::funcref;
        break;
        case byte_t {0x6f}:
            type = ValueType::externref;
        break;
        default:
            throw Exception::invalid_valuetype(stream.location());
    }
    return stream;
}

template<> Stream& Decode::operator>> <FuncType>(Stream& stream, FuncType& type){
    if(stream.get<byte_t>() != (byte_t)0x60){
        throw Exception::invalid_functype(stream.location());
    }
    return stream >> type.params >> type.results;
}

template<> Stream& Decode::operator>> <RefType>(Stream& stream, RefType& type){
    switch(stream.get<byte_t>()){
        case byte_t {0x70}:
            type = RefType::funcref;
        break;
        case byte_t {0x6f}:
            type = RefType::externref;
        break;
        default:
            throw Exception::invalid_reftype(stream.location());
    }
    return stream;
}

template<> Stream& Decode::operator>> <Limits>(Stream& stream, Limits& limit){
    limit.max.reset();
    switch(stream.get<byte_t>()){
        case byte_t {0x00}:
            return stream >> limit.min;
        break;
        case byte_t {0x01}:
            return stream >> limit.min >> limit.max.emplace();
        break;
        default:
            throw Exception::invalid_reftype(stream.location());
    }
    return stream;
}

Stream& Decode::Type::read(Stream& stream){
    auto size = header(stream);
    if(size){
        stream >> types;
    }
    return stream;
}