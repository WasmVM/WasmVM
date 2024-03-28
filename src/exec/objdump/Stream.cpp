// Copyright 2024 Luis Hsu, Jack Lin. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "objdump.hpp"

#include <iomanip>

using namespace WasmVM;

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


std::ostream& Objdump::operator<<(std::ostream& os, Objdump::Bytes& bytes){
    // TODO: print byte
    std::ios::fmtflags flags = std::cout.flags();
    std::cout << std::hex << std::setw(2);
    for(size_t i = 0; i < bytes.size(); i++){
        std::cout << (int)bytes[i] << " ";
    }
    std::cout.setf(flags);
    return os;
}

std::ostream& Objdump::operator<<(std::ostream& os, Stream& stream){
    while(!stream.istream.eof()){
        Bytes bytes(1);
        stream >> bytes;
        std::ios::fmtflags flags = std::cout.flags();
        std::cout << std::hex << std::showbase << bytes.address << ": " << bytes << std::endl;
        std::cout.setf(flags);
    }
    return os;
}
