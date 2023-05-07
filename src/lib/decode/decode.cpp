// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "decode.hpp"
#include "exception.hpp"

#include <sstream>

using namespace WasmVM;
using namespace Decode;

WasmModule WasmVM::module_decode(std::istream& istream){
    // Check magic & version
    {
        u32_t data;
        istream.read((char*) &data, sizeof(u32_t));
        if(data != Decode::magic){
            throw Exception::incorrect_magic();
        }
        istream.read((char*) &data, sizeof(u32_t));
        if(data != Decode::version){
            throw Exception::incorrect_magic();
        }
    }

    WasmModule module;
    Decode::Stream stream(istream);
    // Types
    stream >> Decode::Type(module.types);
    return module;
}

byte_t Decode::Stream::peek(){
    return (byte_t)istream.peek();
}

size_t Decode::Stream::location(){
    return istream.tellg();
}

Stream& Decode::operator>>(Stream& stream, Decode::Section&& section){
    byte_t id_val = stream.peek();
    if(id_val == section.id){
        return section.read(stream);
    }else if(id_val == (byte_t)0x00){
        return Decode::Section().read(stream);
    }
    return stream;
}

Stream& Decode::Section::read(Stream& stream){
    std::optional<size_t> size = header(stream);
    if(size){
        stream.istream.seekg(size.value(), std::ios::cur);
    }
    return stream;
}

std::optional<size_t> Decode::Section::header(Stream& stream){
    byte_t id_val = stream.get<byte_t>();
    if(id_val == id){
        return stream.get<u32_t>();
    }else{
        return std::nullopt;
    }
}

Exception::incorrect_magic::incorrect_magic() : 
    Decode("incorrect magic & version number", 0) {}
Exception::invalid_functype::invalid_functype(size_t location) : 
    Decode("invalid func type: func type should start with 0x60", location) {}
Exception::invalid_valuetype::invalid_valuetype(size_t location) : 
    Decode("invalid value type", location) {}