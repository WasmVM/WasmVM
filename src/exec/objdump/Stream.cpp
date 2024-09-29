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
    istream.seekg(0, std::ios::end);
    size_t total_bytes = istream.tellg();
    istream.seekg(0, std::ios::beg);
    address_width = std::log2(total_bytes) / 4 + 1;
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

// ----- INPUT -----
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
    stream >> typesection.functype;
    
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


// get limit
template<>
Objdump::Stream& Objdump::operator>><Limits>(Stream& stream, Limits& limits){
    // 1. get 0 or 1
    Objdump::Bytes nums(1);
    stream >> nums;
    Objdump::Bytes byteData;
    if((int)nums[0] == 0x00){
        byteData = stream.get_u64(stream); 
        limits.min = stream.to_u64(byteData);
    }else{ // 0x01
        byteData = stream.get_u64(stream);
        limits.min = stream.to_u64(byteData);
        byteData = stream.get_u64(stream);
        limits.max = stream.to_u64(byteData);
    }

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

            std::cout << "index 1" << std::endl;
            Objdump::Bytes byteData = stream.get_u32(stream);
            u32_t descData = stream.to_u32(byteData);
            import.desc = (index_t)descData;
            
            break;
        }
        case 1: { // need more sample
            std::cout << "index 2" << std::endl;
            TableType table;
            ValueType ref;
            Limits lim;
            Objdump::Bytes maxflag(1);

            stream >> ref;
            stream >> maxflag;

            if((int)maxflag[0] == 1){
                Objdump::Bytes byteData = stream.get_u64(stream);
                offset_t min = stream.to_u64(byteData);
                byteData = stream.get_u64(stream);
                offset_t max = stream.to_u64(byteData);
                lim.min = min;
                lim.max = max;
            }else{
                Objdump::Bytes byteData = stream.get_u64(stream);
                offset_t min = stream.to_u64(byteData);
                lim.min = min;
            }

            table.reftype = (RefType)ref;
            table.limits = lim;
            import.desc = table;

            // std::cout << (int)table.reftype << std::endl;
            // std::cout << table.limits.min << std::endl;
            // std::cout << *table.limits.max << std::endl;
            
            break;
        }
        case 2:
            // TODO:

            break;
        case 3:
            // TODO:

            break;
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


// ------------------

// ----- OUTPUT -----
// Use it when you need to print HEX raw data
std::ostream& Objdump::operator<<(std::ostream& os, Objdump::Bytes& bytes){
    std::ios::fmtflags flags = std::cout.flags();
    std::cout << std::hex;
    for(size_t i = 0; i < bytes.size(); i++){
        std::cout << std::setfill('0') << std::setw(2) << (int)bytes[i] << " ";
    }
    std::cout.setf(flags);

    return os;
}

std::ostream& Objdump::operator<<(std::ostream& os, ValueType& value){
    switch (value) {
        case ValueType::i32:
            os << "i32";
        break;
        case ValueType::i64: 
            os << "i64";
        break;
        case ValueType::f32:
            os << "f32";
        break;
        case ValueType::f64:
            os << "f64";
        break;
        case ValueType::funcref:
            os << "funcref";
        break;
        case ValueType::externref: 
            os << "externref";
        break;
        default: 
            os << "unknown";
        break;
    }

    return os;
}

std::ostream& Objdump::operator<<(std::ostream& os, Objdump::Section& section){

    section.stream.print_address(section.id);
    std::cout << section.id << " ; Section ID" << std::endl;

    u32_t SizeInDec = 0;
    for (int i = 0; i < section.size.size(); i++){
        SizeInDec |= ((int)section.size[i] & 0x7F) << (i*7);
    }
    section.stream.print_address(section.size_address);
    std::cout << section.size << " ; Section size (" << SizeInDec << ")"<< std::endl;

    return os;
}

std::ostream& Objdump::operator<<(std::ostream& os, Objdump::TypeSection& section){
    size_t address = section.size_address;
    std::cout << (Objdump::Section&)section;
    section.stream.print_address(++address);

    std::ios::fmtflags flags = std::cout.flags();
    std::cout << std::hex;
    std::cout << std::setfill('0') << std::setw(2) << section.functype.size() << "  ; Number of types" << std::endl;

    for(FuncType functype : section.functype){
        section.stream.print_address(++address);
        std::cout << "60" << "  ; Function" << std::endl;
        // Params
        section.stream.print_address(++address);
        std::cout << std::setfill('0') << std::setw(2) << functype.params.size() << "  ; num params" << std::endl;
        for(ValueType param : functype.params){
            section.stream.print_address(++address);
            std::cout << (int)param << "  ; " << param << std::endl;
        }
        // Results
        section.stream.print_address(++address);
        std::cout << std::setfill('0') << std::setw(2) << functype.results.size() << "  ; num results" << std::endl;
        for(ValueType result : functype.results){
            section.stream.print_address(++address);
            std::cout << (int)result << "  ; " << result << std::endl;
        }

    }
    std::cout.setf(flags);

    return os;
}

std::ostream& Objdump::operator<<(std::ostream& os, Objdump::ImportSection& importsection){
    size_t address = importsection.size_address;
    std::cout << (Objdump::Section&)importsection;
    importsection.stream.print_address(++address);

    std::ios::fmtflags flags = std::cout.flags();
    std::cout << std::hex;
    std::cout << std::setfill('0') << std::setw(2) << importsection.imports.size() << "  ; Number of imports" << std::endl;
    address++;
    
    for(WasmImport import : importsection.imports){        
        // mod
        importsection.stream.print_address(address);
        std::cout << std::setfill('0') << std::setw(2) << import.module.length() << "  ; mod length" << std::endl;
        importsection.stream.print_address(++address);
        for(int i = 0; i < import.module.size(); i++){
            std::cout << std::hex << (int)import.module[i] << " ";
        }
        std::cout << " ; " << import.module << std::endl;
        address += import.module.length();

        // nm
        importsection.stream.print_address(address);
        std::cout << std::setfill('0') << std::setw(2) << import.name.length() << "  ; name length" << std::endl;
        importsection.stream.print_address(++address);
        for(int i = 0; i < import.name.size(); i++){
            std::cout << std::hex << (int)import.name[i] << " ";
        }
        std::cout << " ; " << import.name << std::endl;
        address += import.name.length();

        //ToDO: print out desc

        // 1. print import kind addrerss
        // 2. print import kind data and explain
        // 3. print signature


        importsection.stream.print_address(++address);


        std::visit(overloaded{
            [](index_t arg)     {  
                                    std::cout << "00  ; import kind: func" << std::endl;
                                    std::cout << "    " << arg << " ; signature index" << std::endl;
                                },
            [](TableType  arg)  {   std::cout << "01  ; import kind: table" << std::endl; 
                                    // std::cout << "    " << (int)arg.limits << " ; signature index" << std::endl;
                                },
            [](MemType arg)     { },
            [](GlobalType arg)  { }
        }, import.desc);


    }

    std::cout.setf(flags);

    return os;
}


// Print function
void Objdump::Stream::print_address(Bytes& bytes){
    std::ios::fmtflags flags = std::cout.flags();
    std::cout << "0x" << std::hex << std::setfill('0') << std::setw(address_width) << bytes.address << ": ";
    std::cout.setf(flags);
}

// Print function
void Objdump::Stream::print_address(size_t& address){
    std::ios::fmtflags flags = std::cout.flags();
    std::cout << "0x" << std::hex << std::setfill('0') << std::setw(address_width) << address << ": ";
    std::cout.setf(flags);
}

std::ostream& Objdump::operator<<(std::ostream& os, Stream& stream){
    while(!stream.istream.eof()){
        Bytes bytes(1);
        stream >> bytes;
        stream.print_address(bytes);
        std::cout << bytes << std::endl;
    }
    return os;
}
// ------------------
