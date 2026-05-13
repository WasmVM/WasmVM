// Copyright 2024 Luis Hsu, Jack Lin. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "objdump.hpp"
#include <iomanip>
#include <cmath>
#include <Util.hpp>

using namespace WasmVM;

// constructor for stream
Objdump::Stream::Stream(std::istream& istream) : istream(istream){
}

u32_t Objdump::Stream::peek_section(Objdump::Stream& stream){
    return stream.istream.peek();
}

Objdump::Bytes Objdump::Stream::get_u32(Objdump::Stream& stream){
    Bytes size;
    for(int i = 0; i < 5; i++){
        char bchar = stream.istream.get();
        size.push_back((byte_t)bchar);
        if((bchar & 0x80) == 0){ // end of LEB128
            break;
        }
    }

    return size;
}

Objdump::Bytes Objdump::Stream::get_u64(Objdump::Stream& stream){
    Bytes size;
    for(int i = 0; i < 10; i++){
        char bchar = stream.istream.get();
        size.push_back((byte_t)bchar);
        if((bchar & 0x80) == 0){
            break;
        }
    }

    return size;
}

Objdump::Bytes Objdump::Stream::get_f32(Objdump::Stream& stream){
    // TODO: memcpy implementation
    Bytes f32_bytes(4);
    return f32_bytes;
}

Objdump::Bytes Objdump::Stream::get_f64(Objdump::Stream& stream){
    // TODO: memcpy implementation
    Bytes f64_bytes(8);
    return f64_bytes;
}

u32_t Objdump::Stream::to_u32(Objdump::Bytes& bytes){
    u32_t value = 0;
    for(size_t i = 0; (i < bytes.size()) && (i < 5); ++i){
        value |= ((u32_t) bytes[i] & 0x7F) << (i*7);
    }
    return value;
}

u64_t Objdump::Stream::to_u64(Objdump::Bytes& bytes){
    u64_t value = 0;
    for(size_t i = 0; (i < bytes.size()) && (i < 10); ++i){
        value |= ((u64_t) bytes[i] & 0x7F) << (i*7);
    }
    return value;
}

// ----- INPUT (PARSING) -----
// get simple bytes and address
template<>
Objdump::Stream& Objdump::operator>><Objdump::Bytes>(Stream& stream, Objdump::Bytes& bytes){
    bytes.address = stream.istream.tellg();
    for(size_t i = 0; i < bytes.size(); i++){
        char bchar;
        stream.istream >> bchar; // need to check why istream can output to bchar
        bytes[i] = (byte_t)bchar;
    }

    return stream;
}

// Section entry, every section needs it ^^
template<>
Objdump::Stream& Objdump::operator>><Objdump::Section>(Stream& stream, Objdump::Section& section){
    stream >> section.id;
    section.size_address = stream.istream.tellg();
    section.size = stream.get_u32(stream);

    return stream;
}

// typesection::functype::valuetype
template<>
Objdump::Stream& Objdump::operator>><ValueType>(Stream& stream, ValueType& valuetype){
    Objdump::Bytes value(1);
    stream >> value;
    valuetype = (ValueType)value[0];

    return stream;
}

// typesection::functype
template<>
Objdump::Stream& Objdump::operator>><FuncType>(Stream& stream, FuncType& functype){
    Objdump::Bytes funcMagic(1);
    stream >> funcMagic;

    if((int)funcMagic[0] == 0x60){
        stream >> functype.params;
        stream >> functype.results;
    }
    
    return stream;
}

// typesection
template<>
Objdump::Stream& Objdump::operator>><Objdump::TypeSection>(Stream& stream, Objdump::TypeSection& typesection){
    stream >> (Objdump::Section&)typesection; // call section
    typesection.raw_count = stream.get_u32(stream);
    u32_t count = stream.to_u32(typesection.raw_count);
    typesection.functype.resize(count);
    for(u32_t i = 0; i < count; i++){
        stream >> typesection.functype[i];
    }
    
    return stream;
}

// byte->string
template<>
Objdump::Stream& Objdump::operator>><byte_t>(Stream& stream, byte_t& byte){
    char temp;

    stream.istream >> temp;
    byte = (std::byte)temp;

    return stream;
}

// import
template<>
Objdump::Stream& Objdump::operator>><WasmImport>(Stream& stream, WasmImport& import){
    // module
    std::vector<byte_t> mod;
    stream >> mod;
    import.module.assign((char*)mod.data(), mod.size());

    // name
    std::vector<byte_t> nm;
    stream >> nm;
    import.name.assign((char*)nm.data(), nm.size());

    //desc
    Objdump::Bytes kind(1);
    stream >> kind;
    int int_kind = (int)kind[0];

    switch(int_kind){
        case 0: {
            Objdump::Bytes byteData = stream.get_u32(stream);
            u32_t descData = stream.to_u32(byteData);
            import.desc = (index_t)descData;
            
            break;
        }
        case 1: { 
            TableType table;
            ValueType ref;
            Limits lim;
            Objdump::Bytes maxflag(1);

            stream >> ref;
            stream >> maxflag;

            bool hasMax = (maxflag[0] == byte_t{0b1});
            Objdump::Bytes Data = stream.get_u64(stream);
            lim.min = stream.to_u64(Data);
            if (hasMax) {
                Data = stream.get_u64(stream);
                offset_t max = stream.to_u64(Data);
                lim.max = max;
            }

            table.reftype = (RefType)ref;
            table.limits = lim;
            import.desc = table;
            
            break;
        }
        case 2:{
            Limits lim;
            Objdump::Bytes maxflag(1);
            stream >> maxflag;

            bool hasMax = (maxflag[0] == byte_t{0b1});
            Objdump::Bytes Data = stream.get_u64(stream);
            lim.min = stream.to_u64(Data);
            if (hasMax) {
                Data = stream.get_u64(stream);
                offset_t max = stream.to_u64(Data);
                lim.max = max;
            }

            import.desc = lim;
            break;
        }
        case 3:{
            GlobalType global;
            Objdump::Bytes mut(1);

            stream >> global.type;
            stream >> mut;

            if((int)mut[0] == 0x00){
                global.mut = GlobalType::constant;
            }else if((int)mut[0] == 0x01){
                global.mut = GlobalType::variable;
            }

            import.desc = global;

            break;
        }
        default:{
            Objdump::Bytes dummy(1);
            stream >> dummy;
        }
    }


    return stream;
}

// importsection
template<>
Objdump::Stream& Objdump::operator>><Objdump::ImportSection>(Stream& stream, Objdump::ImportSection& importsection){
    stream >> (Objdump::Section&)importsection; // call section
    stream >> importsection.imports;

    return stream;
}

// function
template<>
Objdump::Stream& Objdump::operator>><index_t>(Stream& stream, index_t& index){
    Objdump::Bytes byteData = stream.get_u32(stream); // get raw leb128 
    index_t funcID = stream.to_u32(byteData); // transfer to u_32
    index = funcID;

    return stream;
}

// functionsection
template<>
Objdump::Stream& Objdump::operator>><Objdump::FunctionSection>(Stream& stream, Objdump::FunctionSection& functionsection){
    stream >> (Objdump::Section&)functionsection; // call section
    stream >> functionsection.functions; // vec of idex_t

    return stream;
}

// table
template<>
Objdump::Stream& Objdump::operator>><TableType>(Stream& stream, TableType& tt){
    ValueType et;
    Limits lim;
    Objdump::Bytes maxflag(1);

    stream >> et;
    stream >> maxflag;

    bool hasMax = (maxflag[0] == byte_t{0b1});
    Objdump::Bytes Data = stream.get_u64(stream);
    lim.min = stream.to_u64(Data);
    if (hasMax) {
        Data = stream.get_u64(stream);
        offset_t max = stream.to_u64(Data);
        lim.max = max;
    }

    tt.limits = lim;
    tt.reftype = (RefType)et;

    return stream;
}

// tablesection
template<>
Objdump::Stream& Objdump::operator>><Objdump::TableSection>(Stream& stream, Objdump::TableSection& tablesection){
    stream >> (Objdump::Section&)tablesection;
    stream >> tablesection.tables;

    return stream;
}

template<>
Objdump::Stream& Objdump::operator>><MemType>(Stream& stream, MemType& mt){
    MemType mem;
    Objdump::Bytes maxflag(1);
    stream >> maxflag;

    bool hasMax = (maxflag[0] == byte_t{0b01});
    Objdump::Bytes Data = stream.get_u64(stream);
    mem.min = stream.to_u64(Data);
    if (hasMax) {
        Data = stream.get_u64(stream);
        offset_t max = stream.to_u64(Data);
        mem.max = max;
    }
    mt = mem;

    return stream;
}

// memorysection
template<>
Objdump::Stream& Objdump::operator>><Objdump::MemorySection>(Stream& stream, Objdump::MemorySection& memorysection){
    stream >> (Objdump::Section&)memorysection;
    stream >> memorysection.memories;

    return stream;
}

template<>
Objdump::Stream& Objdump::operator>><WasmGlobal>(Stream& stream, WasmGlobal& gt){
    GlobalType gt_data;
    Objdump::Bytes mutation(1);

    stream >> gt_data.type;
    stream >> mutation;

    if(mutation[0] == byte_t{0b00}){
        gt_data.mut = GlobalType::constant;
    }else{
        gt_data.mut = GlobalType::variable;
    }

    gt.type = gt_data;

    // loop until 0b
    // call another func for switch
    Objdump::Bytes next_b(1);
    stream >> next_b;

    switch((int)next_b[0]){
        case 0x41: {
            Objdump::Bytes byteData = stream.get_u32(stream);
            u32_t i32_data = stream.to_u32(byteData);
            Instr::I32_const I32(i32_data);
            gt.init = I32;
            break;
        }
        case 0x42: {
            Objdump::Bytes byteData = stream.get_u64(stream);
            u64_t i64_data = stream.to_u64(byteData);
            Instr::I64_const I64(i64_data);
            gt.init = I64;
            break;
        }
        case 0x43: {
            // f32 initializer - TODO: implement
            break;
        }
        case 0x44: {
            // f64 initializer - TODO: implement
            break;
        }
        case 0xD0: {
            // ref.null - TODO: implement
            break;
        }
        case 0xD2: {
            // ref.func - TODO: implement
            break;
        }
        default:
            break;
    };

    return stream;
}

// globalsection
template<>
Objdump::Stream& Objdump::operator>><Objdump::GlobalSection>(Stream& stream, Objdump::GlobalSection& globalsection){
    stream >> (Objdump::Section&)globalsection;
    stream >> globalsection.globals;

    return stream;
}
