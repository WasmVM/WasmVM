// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "encode.hpp"
#include <Util.hpp>

#include <algorithm>
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

// The one-byte form of an abstract heap type. Its own encoding doubles as the
// shorthand for the nullable reference to it, which is how the four legacy
// value types have always been written.
static byte_t abstract_heaptype_byte(ValueType::Kind kind){
    switch(kind){
        case ValueType::nullfuncref:   return (byte_t) 0x73;
        case ValueType::nullexternref: return (byte_t) 0x72;
        case ValueType::nullref:       return (byte_t) 0x71;
        case ValueType::funcref:       return (byte_t) 0x70;
        case ValueType::externref:     return (byte_t) 0x6f;
        case ValueType::anyref:        return (byte_t) 0x6e;
        case ValueType::eqref:         return (byte_t) 0x6d;
        case ValueType::i31ref:        return (byte_t) 0x6c;
        case ValueType::structref:     return (byte_t) 0x6b;
        case ValueType::arrayref:      return (byte_t) 0x6a;
        default:                       return (byte_t) 0x69; // exn
    }
}

template<> Section::Stream& Section::Stream::operator<< <WasmVM::ValueType>(WasmVM::ValueType valtype){
    switch(valtype.kind){
        case ValueType::i32 :
            return *this << (byte_t) 0x7f;
        case ValueType::i64 :
            return *this << (byte_t) 0x7e;
        case ValueType::f32 :
            return *this << (byte_t) 0x7d;
        case ValueType::f64 :
            return *this << (byte_t) 0x7c;
        default:
        break;
    }
    // A nullable reference to an abstract heap type is written with that heap
    // type's own byte; everything else needs the explicit 0x63 / 0x64 form
    // carrying the heap type as a signed value.
    if(valtype.nullable && !valtype.is_concrete()){
        return *this << abstract_heaptype_byte(valtype.kind);
    }
    *this << (byte_t)(valtype.nullable ? 0x63 : 0x64);
    // An abstract heap type's own byte is already the one-byte signed encoding
    // of its negative number, so write it directly: going through the general
    // signed writer would pad it to two bytes.
    if(valtype.is_concrete()){
        return *this << (i64_t)valtype.typeidx;
    }
    return *this << abstract_heaptype_byte(valtype.kind);
}

template<> Section::Stream& Section::Stream::operator<< <WasmVM::FuncType>(WasmVM::FuncType functype){
    *this << (byte_t) 0x60 << functype.params << functype.results;
    return *this;
}

template<> Section::Stream& Section::Stream::operator<< <WasmVM::StorageType>(WasmVM::StorageType storage){
    return std::visit(overloaded {
        [&](ValueType valtype) -> Section::Stream& { return *this << valtype; },
        [&](PackedType packed) -> Section::Stream& {
            return *this << (byte_t)((packed == PackedType::i8) ? 0x78 : 0x77);
        }
    }, storage);
}

template<> Section::Stream& Section::Stream::operator<< <WasmVM::FieldType>(WasmVM::FieldType field){
    return *this << field.storage << (byte_t)(field.mut ? 0x01 : 0x00);
}

template<> Section::Stream& Section::Stream::operator<< <WasmVM::StructType>(WasmVM::StructType type){
    return *this << (byte_t) 0x5f << type.fields;
}

template<> Section::Stream& Section::Stream::operator<< <WasmVM::ArrayType>(WasmVM::ArrayType type){
    return *this << (byte_t) 0x5e << type.field;
}

// A final subtype with no supertype is written as its bare composite type,
// which the spec defines as the same thing.
template<> Section::Stream& Section::Stream::operator<< <WasmVM::SubType>(WasmVM::SubType type){
    if(!type.final || !type.supers.empty()){
        *this << (byte_t)(type.final ? 0x4f : 0x50) << type.supers;
    }
    return std::visit(overloaded {
        [&](const FuncType& functype) -> Section::Stream& { return *this << functype; },
        [&](const StructType& structtype) -> Section::Stream& { return *this << structtype; },
        [&](const ArrayType& arraytype) -> Section::Stream& { return *this << arraytype; }
    }, type.comp);
}

template<> Section::Stream& Section::Stream::operator<< <WasmVM::Limits>(WasmVM::Limits limit){
    byte_t flag;
    if(limit.is64)
        flag = limit.max ? (byte_t)0x05 : (byte_t)0x04;
    else
        flag = limit.max ? (byte_t)0x01 : (byte_t)0x00;
    *this << flag << (u64_t)limit.min;
    if(limit.max) *this << (u64_t)limit.max.value();
    return *this;
}

Section::Stream& Section::Stream::operator<<(std::stringstream& buffer){
    std::string bytes(buffer.str());
    if(bytes.empty()){
        return *this;
    }
    return *this << bytes;
}

// The section is a vector of recursion groups. A type outside every explicit
// (rec ...) group is a group of one and is written bare.
Encode::Type::Type(const std::vector<SubType>& types, const std::vector<RecGroup>& recs) : Section((byte_t)0x01){
    if(!types.empty()){
        std::vector<RecGroup> groups(recs.begin(), recs.end());
        std::sort(groups.begin(), groups.end(), [](const RecGroup& a, const RecGroup& b){ return a.first < b.first; });
        u32_t count = (u32_t)types.size();
        for(const RecGroup& rec : groups){
            count -= rec.count - 1;
        }
        Encode::Section::Stream stream(buffer);
        stream << count;
        size_t k = 0;
        for(index_t i = 0; i < types.size();){
            if(k < groups.size() && groups[k].first == i){
                stream << (byte_t) 0x4e << (u32_t)groups[k].count;
                for(index_t j = 0; j < groups[k].count; ++j){
                    stream << types[i + j];
                }
                i += groups[k].count;
                k += 1;
            }else{
                stream << types[i];
                i += 1;
            }
        }
    }
}