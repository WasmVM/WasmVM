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
    return stream;
}

// valuetype => std::vector<ValueType>
template<>
Objdump::Stream& Objdump::operator>><Objdump::TypeSection.functype.rt1>(Stream& stream, Objdump::TypeSection.functype.rt1& rt1){
    // TODO: get {7f, 7f}, {7f, 7f}

    rt1.push_back(0x7f);
    rt1.push_back(0x7f);

    return stream;

}

// valuetype => std::vector<ValueType>
template<>
Objdump::Stream& Objdump::operator>><Objdump::TypeSection.functype.rt2>(Stream& stream, Objdump::TypeSection.functype.rt2& rt2){
    // TODO: get {7f, 7f}, {7f, 7f}

    rt2.push_back(0x6f);
    rt2.push_back(0x6f);

    return stream;


}



// functype => std::vector<FuncType>
template<>
Objdump::Stream& Objdump::operator>><Objdump::TypeSection.functype>(Stream& stream, Objdump::TypeSection.functype& functype){
    // TODO: get {7f, 7f}, {7f, 7f} get how many for rs1 and rs2
    // construct how many functions

    Bytes funcParam(2);
    stream >> funcParam;

    int curr = functype.size();
    curr+=1;
    functype.resize(curr);
    curr-=1;

    if(funcParam[0] == 0x60){
        for(int i = 0; i < funcParam[1]; i++){
            //param
            stream >> functype[curr].rt1;
        }

        Bytes Res(1);
        stream >> Res;
        for(int i = 0; i < Res[0]; i++){
            // res
            stream >> functype[curr].rt2;
        }
    }

    return stream;
}

template<>
Objdump::Stream& Objdump::operator>><Objdump::TypeSection>(Stream& stream, Objdump::TypeSection& typesection){
    // get num type
    Bytes nums(1);
    stream >> nums;
    stream.print_address(nums);
    std::cout << nums << " ; num types" << std::endl;


    for(int i = 0; i < nums; i++){
        stream >> typesection.functype;
    }


    for(int i = 0; i < typesection.functype.size(); i++){
        std::cerr << "{";
        for(int j = 0, k = 0; j < typesection.functype.rt1.size(), k < typesection.functype.rt2.size(); j++, k++){
            std::cerr << "{" << typesection.functype[0].rt1[j] << ", "
        }

        std::cerr << "}" << std << endl;
    }




    // for(int i = 0; i < nums; i++){
    //     Bytes func(1);
    //     stream >> func;
    //     stream.print_address(func);
    //     std::cout << func << " ; func" <<std::endl;

    //     // check nums of params
    //     Bytes num_params(1);
    //     stream >> num_params;
    //     stream.print_address(num_params);
    //     std::cout << num_params << " ; num_params" <<std::endl;

    //     std::vector<ValueType> params[num_params];
    //     stream >> params;

    //     // check nums of result
    //     Bytes num_results(1);
    //     stream >> num_results;
    //     stream.print_address(num_results);
    //     std::cout << num_results << " ; num_results" <<std::endl;

    //     std::vector<ValueType> result[num_results];
    // }

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

std::ostream& Objdump::operator<<(std::ostream& os, TypeSection& section){
    //TODO:
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
