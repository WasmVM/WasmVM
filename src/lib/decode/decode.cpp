// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <decode.hpp>
#include "exception.hpp"

#include <sstream>

using namespace WasmVM;
using namespace Decode;

WasmModule WasmVM::module_decode(std::istream& istream){
    // Check magic & version
    {
        u32_t data;
        istream.read((char*) &data, sizeof(u32_t));
        if(data != magic){
            throw Exception::incorrect_magic();
        }
        istream.read((char*) &data, sizeof(u32_t));
        if(data != version){
            throw Exception::incorrect_magic();
        }
    }

    WasmModule module;
    Decode::Stream stream(istream, module);
    // Types
    stream >> Decode::Type(module.types);
    // Imports
    stream >> Decode::Import(module.imports);
    // Funcs
    stream >> Decode::Func(module.funcs);
    // Tables
    stream >> Decode::Table(module.tables);
    // Mems
    stream >> Decode::Memory(module.mems);
    // Globals
    stream >> Decode::Global(module.globals);
    // Exports
    stream >> Decode::Export(module.exports);
    // Start
    stream >> Decode::Start(module.start);
    // Elems
    stream >> Decode::Elem(module.elems);
    // Data count
    stream >> Decode::DataCount(module.datas);
    // Code
    stream >> Decode::Code(module.funcs);
    // Datas
    stream >> Decode::Data(module.datas);
    return module;
}

byte_t Decode::Stream::peek(){
    return (byte_t)istream.peek();
}

size_t Decode::Stream::location(){
    auto loc = istream.tellg();
    if(loc == -1){
        istream.seekg(0, std::ios_base::end);
        loc = istream.tellg();
        istream.seekg(-1);
    }
    return loc;
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
Exception::invalid_import::invalid_import(size_t location) : 
    Decode("invalid import", location) {}
Exception::invalid_reftype::invalid_reftype(size_t location) : 
    Decode("invalid reference type", location) {}
Exception::invalid_limit::invalid_limit(size_t location) : 
    Decode("invalid limit", location) {}
Exception::invalid_globaltype::invalid_globaltype(size_t location) : 
    Decode("invalid global type", location) {}
Exception::invalid_export::invalid_export(size_t location) : 
    Decode("invalid export", location) {}
Exception::unknown_instruction::unknown_instruction(size_t location) : 
    Decode("unknown instruction", location) {}
Exception::expression_not_end::expression_not_end(size_t location) : 
    Decode("expected end(0x0B) after expression", location) {}
Exception::invalid_elem::invalid_elem(size_t location) : 
    Decode("invalid element", location) {}
Exception::invalid_data::invalid_data(size_t location) : 
    Decode("invalid data", location) {}
Exception::count_mismatch::count_mismatch(size_t location) : 
    Decode("vector length mismatch explicit defined size", location) {}
Exception::invalid_instruction::invalid_instruction(size_t location) : 
    Decode("invalid instruction", location) {}