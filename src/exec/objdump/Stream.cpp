// Copyright 2024 Luis Hsu, Jack Lin. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "objdump.hpp"

#include <iomanip>

using namespace WasmVM;

Objdump::Stream::Stream(std::istream& istream) : istream(istream){
    istream.seekg(0, std::ios::end);
    size_t total_bytes = istream.tellg();
    istream.seekg(0, std::ios::beg);
    address_width = std::log2(total_bytes) / 4 + 1;
}

template<>
Objdump::Stream& Objdump::operator>><Objdump::Bytes>(Stream& stream, Objdump::Bytes& bytes){
    // discect data based on count
    bytes.address = stream.istream.tellg();
    for(size_t i = 0; i < bytes.size(); i++){
        char bchar;
        stream.istream >> bchar;
        bytes[i] = (byte_t)bchar;
    }
    return stream;
}

template<>
Objdump::Stream& Objdump::operator>><u32_t>(Stream& stream, u32_t& value){
    value = 0;
    for(int i = 0; i < 5; i++){
        char bchar = stream.istream.get();
        value |= (bchar & 0x7F) << (i * 7);
        if( (bchar & 0x80) == 0){
            break;
        }
    }

    return stream;
}

template<>
Objdump::Stream& Objdump::operator>><Objdump::Section>(Stream& stream, Objdump::Section& section){
    stream >> section.id;
    stream >> section.size;
    return stream;
}

std::ostream& Objdump::operator<<(std::ostream& os, Objdump::Bytes& bytes){
    std::ios::fmtflags flags = std::cout.flags();
    std::cout << std::hex;
    for(size_t i = 0; i < bytes.size(); i++){
        std::cout << std::setfill('0') << std::setw(2) << (int)bytes[i] << " ";
    }
    std::cout.setf(flags);
    return os;
}

std::ostream& Objdump::operator<<(std::ostream& os, Objdump::Section& section){
    //TODO:
    section.stream.print_address(section.id);
    std::cout << section.id << " ; Section ID" << std::endl;

    //section.stream.print_address(section.size);
    std::cout << section.size << " ; Section size" << std::endl;





    return os;
}

void Objdump::Stream::print_address(Bytes& bytes){
    std::ios::fmtflags flags = std::cout.flags();
    std::cout << "0x" << std::hex << std::setfill('0') << std::setw(address_width) << bytes.address << ": ";
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
