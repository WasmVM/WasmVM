// Copyright 2024 Luis Hsu, Jack Lin. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "objdump.hpp"
#include <iomanip>

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

    switch((int)value[0]){
    case 0x7f: 
        valuetype = i32;
        break;
    case 0x7e: 
        valuetype = i64;
        break;
    case 0x7d: 
        valuetype = f32;
        break;
    case 0x7c: 
        valuetype = f64;
        break;
    case 0x70: 
        valuetype = funcref;
        break;
    case 0x6f: 
        valuetype = externref;
        break;
    }
 
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


// std::ostream& Objdump::operator<<(std::ostream& os, std::vector<FuncType>& functype){

//     std::cout << functype.params << "  ; n params    " << std::endl;

//     std::cout << "Params: ";
//     for (const auto& param : functype.params) {
//         std::cout << param << " ";
//     }
//     std::cout << std::endl;
//     return os;

// }

// Function to convert ValueType to a human-readable string
const char* valueTypeToString(ValueType vt) {
    switch (vt) {
        case ValueType::i32: return "i32";
        case ValueType::i64: return "i64";
        case ValueType::f32: return "f32";
        case ValueType::f64: return "f64";
        case ValueType::funcref: return "funcref";
        case ValueType::externref: return "externref";
        default: return "unknown";
    }
}


std::ostream& Objdump::operator<<(std::ostream& os, Objdump::TypeSection& typesection){
    Section tmp =  (Objdump::Section&)typesection;
    std::cout << tmp;
    std::map<int, int> vb = {
        {0, 0x7f},
        {1, 0x7e},
        {2, 0x7d},
        {3, 0x7c},
        {4, 0x70},
        {5, 0x6f}
    };

    tmp.stream.print_address(++tmp.size_address);
    std::cout << std::setfill('0') << std::setw(2) <<typesection.functype.size() << "  ; N of func" << std::endl;

    for(int F = 0; F < typesection.functype.size(); F++){
        tmp.stream.print_address(++tmp.size_address);
        std::cout << "60" << "  ; head" << std::endl;

        FuncType tmp2 = typesection.functype[F];

        tmp.stream.print_address(++tmp.size_address);
        std::cout << std::setfill('0') << std::setw(2) << tmp2.params.size() << "  ; num params" << std::endl;

        for(int V = 0; V < tmp2.params.size(); V++){
            ValueType val = tmp2.params[V];
            tmp.stream.print_address(++tmp.size_address);
            std::cout << std::hex << vb[val] << "  ; " << valueTypeToString(val) << std::endl;
        }

        tmp.stream.print_address(++tmp.size_address);
        std::cout << std::setfill('0') << std::setw(2) << tmp2.results.size() << "  ; num results" << std::endl;

        for(int V = 0; V < tmp2.results.size(); V++){
            ValueType val = tmp2.results[V];
            tmp.stream.print_address(++tmp.size_address);
            std::cout << std::hex << vb[val] << "  ; " << valueTypeToString(val) << std::endl;
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
