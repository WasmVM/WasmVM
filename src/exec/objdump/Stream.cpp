// Copyright 2024 Luis Hsu, Jack Lin. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "objdump.hpp"

using namespace WasmVM;

template<>
Objdump::Stream& Objdump::operator>><Objdump::Bytes>(Stream& stream, Objdump::Bytes& bytes){
    // discect data based on count
    bytes.address = stream.istream.tellg();
    for(size_t i = 0; i < bytes.size(); i++){
        std::cout << i;
        char bchar;
        stream.istream >> bchar;
        bytes.push_back((byte_t)bchar);

    }
    return stream;
}


std::ostream& Objdump::operator<<(std::ostream& os, Objdump::Bytes& bytes){
    // TODO: print byte
    std::ios::fmtflags flags = std::cerr.flags();
    std::cout << std::hex << std::showbase;

    for(size_t i = 0; i < bytes.size(); i++){
        std::cout << (int)bytes[i] << " ";
    }
    std::cout << std::endl;



    std::cerr.setf(flags);
    return os;
}

std::ostream& Objdump::operator<<(std::ostream& os, Stream& stream){
    while(!stream.istream.eof()){
        // sizable byte read
        // int size = 2;
        // for( i = 0; i < size; i++){
            Bytes bytes(3);
            stream >> bytes;
        // }
        std::cout << bytes.address << ": " << bytes << std::endl;
        // std::cout << "count" << count << std::endl;
    }
    return os;
}

void Objdump::printBytes(int count, Objdump::Stream& stream){
    std::cout << "Address: byte" << count << std::endl;
    std::cout << stream;
}





