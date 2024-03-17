// Copyright 2024 Luis Hsu, Jack Lin. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "objdump.hpp"

using namespace WasmVM;

template<>
Objdump::Stream& Objdump::operator>><Objdump::Byte>(Stream& stream, Objdump::Byte& byte){
    byte.address = stream.istream.tellg();
    char bchar;
    stream.istream >> bchar;
    byte.byte = (byte_t)bchar;
    return stream;
}


std::ostream& Objdump::operator<<(std::ostream& os, Objdump::Byte& byte){
    // TODO: print byte
    // std::ios::fmtflags flags = std::cerr.flags();
    // std::cerr << input_path.string() << ":" << std::hex << std::showbase << e.location << " " COLOR_Error ": " << e.what() << std::endl;
    // std::cerr.setf(flags);

    std::cout << std::hex << std::showbase << (int)byte.byte;
    return os;
}

std::ostream& Objdump::operator<<(std::ostream& os, Stream& stream){
    while(!stream.istream.eof()){
        Byte byte;
        stream >> byte;
        std::cout << byte.address << ": " << byte << std::endl;
    }
    return os;
}
