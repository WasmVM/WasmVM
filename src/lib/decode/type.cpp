// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <decode.hpp>
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
    std::stack<byte_t> bytes;
    for(size_t i = ((sizeof(T) * 8 + 7) / 7); (i > 0); --i){
        bytes.push((byte_t)stream.get());
        if((((u8_t)bytes.top()) & 0x80) == 0){
            break;
        }
    }
    T value = 0;
    while(!bytes.empty()){
        value = (value << 7) & (~0x7f);
        value |= (T)bytes.top() & 0x7f;
        bytes.pop();
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
    byte_t prefix = stream.get<byte_t>();
    switch(prefix){
        case byte_t {0x7f}:
            type = ValueType::i32;
        return stream;
        case byte_t {0x7e}:
            type = ValueType::i64;
        return stream;
        case byte_t {0x7d}:
            type = ValueType::f32;
        return stream;
        case byte_t {0x7c}:
            type = ValueType::f64;
        return stream;
        // 0x63 / 0x64: a reference with the heap type written out, which is
        // the only way to spell a non-null one or a concrete one.
        case byte_t {0x63}:
        case byte_t {0x64}: {
            i64_t heaptype = stream.get<i64_t>();
            if((heaptype < 0) && !abstract_heaptype((int32_t)heaptype)){
                throw Exception::invalid_valuetype(stream.location());
            }
            type = reference_to((int32_t)heaptype, prefix == byte_t {0x63});
        }
        return stream;
        default:
        break;
    }
    // Otherwise the byte is an abstract heap type standing for the nullable
    // reference to it.
    if(std::optional<ValueType::Kind> kind = abstract_heaptype((int32_t)((u8_t)prefix) - 0x80)){
        type = ValueType(kind.value(), true);
        return stream;
    }
    throw Exception::invalid_valuetype(stream.location());
}

template<> Stream& Decode::operator>> <FuncType>(Stream& stream, FuncType& type){
    if(stream.get<byte_t>() != (byte_t)0x60){
        throw Exception::invalid_functype(stream.location());
    }
    return stream >> type.params >> type.results;
}

template<> Stream& Decode::operator>> <StorageType>(Stream& stream, StorageType& storage){
    switch(stream.peek()){
        case byte_t {0x78}:
            stream.get<byte_t>();
            storage = PackedType::i8;
        break;
        case byte_t {0x77}:
            stream.get<byte_t>();
            storage = PackedType::i16;
        break;
        default:
            storage = stream.get<ValueType>();
        break;
    }
    return stream;
}

template<> Stream& Decode::operator>> <FieldType>(Stream& stream, FieldType& field){
    stream >> field.storage;
    switch(stream.get<byte_t>()){
        case byte_t {0x00}:
            field.mut = false;
        break;
        case byte_t {0x01}:
            field.mut = true;
        break;
        default:
            throw Exception::invalid_fieldtype(stream.location());
    }
    return stream;
}

template<> Stream& Decode::operator>> <CompType>(Stream& stream, CompType& comp){
    switch(stream.peek()){
        case byte_t {0x60}:
            comp = stream.get<FuncType>();
        break;
        case byte_t {0x5f}: {
            stream.get<byte_t>();
            StructType structtype;
            stream >> structtype.fields;
            comp = std::move(structtype);
        }break;
        case byte_t {0x5e}: {
            stream.get<byte_t>();
            ArrayType arraytype;
            stream >> arraytype.field;
            comp = std::move(arraytype);
        }break;
        default:
            throw Exception::invalid_comptype(stream.location());
    }
    return stream;
}

// subtype ::= 0x50 vec(typeidx) comptype   (open to subtyping)
//           | 0x4f vec(typeidx) comptype   (final)
//           | comptype                     (final, no supertype)
template<> Stream& Decode::operator>> <SubType>(Stream& stream, SubType& type){
    byte_t prefix = stream.peek();
    if(prefix == byte_t {0x50} || prefix == byte_t {0x4f}){
        stream.get<byte_t>();
        type.final = (prefix == byte_t {0x4f});
        type.supers.clear();
        stream >> type.supers;
    }else{
        type.final = true;
        type.supers.clear();
    }
    return stream >> type.comp;
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
    limit.is64 = false;
    switch(stream.get<byte_t>()){
        case byte_t {0x00}:
            return stream >> limit.min;
        break;
        case byte_t {0x01}:
            return stream >> limit.min >> limit.max.emplace();
        break;
        case byte_t {0x04}:
            limit.is64 = true;
            return stream >> limit.min;
        break;
        case byte_t {0x05}:
            limit.is64 = true;
            return stream >> limit.min >> limit.max.emplace();
        break;
        default:
            throw Exception::invalid_reftype(stream.location());
    }
    return stream;
}

// The section is a vector of recursion groups: 0x4e vec(subtype), or a bare
// subtype standing for a group of one. Explicit groups are recorded in recs;
// an empty one defines nothing and is dropped.
Stream& Decode::Type::read(Stream& stream){
    auto size = header(stream);
    if(size){
        for(u32_t groups = stream.get<u32_t>(); groups > 0; --groups){
            if(stream.peek() == byte_t {0x4e}){
                stream.get<byte_t>();
                u32_t count = stream.get<u32_t>();
                if(count > 0){
                    recs.push_back(RecGroup{(index_t)types.size(), count});
                }
                for(; count > 0; --count){
                    stream >> types.emplace_back();
                }
            }else{
                stream >> types.emplace_back();
            }
        }
    }
    return stream;
}