// Copyright 2024 Luis Hsu, Jack Lin. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "objdump.hpp"

using namespace WasmVM;

template<>
Objdump::Stream& Objdump::operator>><Objdump::Byte>(Stream& stream, Objdump::Byte& byte){
    // discect data based on count
    byte.address = stream.istream.tellg();
    char bchar;
    stream.istream >> bchar;
    byte.byte.push_back((byte_t)bchar);

    char bchar1;
    stream.istream >> bchar1;
    byte.byte.push_back((byte_t)bchar1);

    return stream;
}


std::ostream& Objdump::operator<<(std::ostream& os, Objdump::Byte& byte){
    // TODO: print byte
    // std::ios::fmtflags flags = std::cerr.flags();
    // std::cerr << input_path.string() << ":" << std::hex << std::showbase << e.location << " " COLOR_Error ": " << e.what() << std::endl;
    // std::cerr.setf(flags);

    std::cout << std::hex << std::showbase << (int)byte.byte[0] << (int)byte.byte[1];
    return os;
}

std::ostream& Objdump::operator<<(std::ostream& os, Stream& stream){
    while(!stream.istream.eof()){
        // sizable byte read
        // int size = 2;
        // for( i = 0; i < size; i++){
            Byte byte;
            stream >> byte;
        // }
        std::cout << byte.address << ": " << byte << std::endl;
        std::cout << "count" << count << std::endl;
    }
    return os;
}

void printBytes(int count){
    std::cout << "Address: byte" << count << std::endl;
    std::cout << stream;
}








