// Copyright 2024 Luis Hsu, Jack Lin. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "objdump.hpp"
#include <iomanip>
#include <cmath>

using namespace WasmVM;

// constructor for stream
Objdump::Stream::Stream(std::istream& istream) : istream(istream){
    istream.seekg(0, std::ios::end);
    size_t total_bytes = istream.tellg();
    istream.seekg(0, std::ios::beg);
    address_width = std::log2(total_bytes) / 4 + 1;
}

Objdump::Bytes Objdump::Stream::get_u32(Objdump::Stream& stream){
    Bytes size;
    // Read size's data
    for(int i = 0; i < 5; i++){
        char bchar = stream.istream.get();
        size.push_back((byte_t)bchar);
        if((bchar & 0x80) == 0){
            break;
        }
    }
    return size;
}

// ----- INPUT -----
template<>
Objdump::Stream& Objdump::operator>><Objdump::Bytes>(Stream& stream, Objdump::Bytes& bytes){
    // get simple bytes and address
    bytes.address = stream.istream.tellg();
    for(size_t i = 0; i < bytes.size(); i++){
        char bchar;
        stream.istream >> bchar; // need to check why istream can output to bchar
        bytes[i] = (byte_t)bchar;
    }
    return stream;
}

// Section entry
template<>
Objdump::Stream& Objdump::operator>><Objdump::Section>(Stream& stream, Objdump::Section& section){
    stream >> section.id;
    section.size_address = stream.istream.tellg();
    section.size = stream.get_u32(stream);

    std::cerr << ";; section process" << std::endl;
    return stream;
}


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

template<>
Objdump::Stream& Objdump::operator>><ValueType>(Stream& stream, ValueType& valuetype){
    Objdump::Bytes value(1);
    stream >> value;
    valuetype = (ValueType)value[0];
    return stream;
}

template<>
Objdump::Stream& Objdump::operator>><Objdump::TypeSection>(Stream& stream, Objdump::TypeSection& typesection){
    stream >> (Objdump::Section&)typesection; // call section
    stream >> typesection.functype;
    
    return stream;
}

// ------------------

// ----- OUTPUT -----
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

std::ostream& Objdump::operator<<(std::ostream& os, Objdump::TypeSection& section){
    size_t address = section.size_address;

    std::cout << (Objdump::Section&)section;

    section.stream.print_address(++address);
    std::cout << std::setfill('0') << std::setw(2) << section.functype.size() << "  ; N of func" << std::endl;

    for(FuncType functype : section.functype){
        section.stream.print_address(++address);
        std::cout << "60" << "  ; head" << std::endl;

        // Params
        section.stream.print_address(++address);
        std::cout << std::setfill('0') << std::setw(2) << functype.params.size() << "  ; num params" << std::endl;

        for(ValueType param : functype.params){
            section.stream.print_address(++address);
            std::cout << std::hex << "  ; " << param << std::endl;
        }
        
        // Results
        section.stream.print_address(++address);
        std::cout << std::setfill('0') << std::setw(2) << functype.results.size() << "  ; num results" << std::endl;

        for(ValueType result : functype.results){
            section.stream.print_address(++address);
            std::cout << std::hex << "  ; " << result << std::endl;
        }

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
