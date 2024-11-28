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
        case 2:{
            Limits lim;
            Objdump::Bytes maxflag(1);
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
Objdump::Stream& Objdump::operator>><TableType>(Stream& stream, WasmVM::TableType& tt){
    ValueType et;
    Limits lim;
    Objdump::Bytes maxflag(1);

    stream >> et;
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
    
    for(WasmImport import : importsection.imports){     
        // mod
        importsection.stream.print_address(++address);
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
        // p.s. no solution for address


        importsection.stream.print_address(address);
        std::visit(overloaded{
            [&importsection, &address](index_t arg){
                std::cout << "00  ; import kind: func" << std::endl;
                importsection.stream.print_address(++address);
                std::cout << std::setfill('0') << std::setw(2);
                std::cout << arg << "  ; signature index" << std::endl;
            },
            [&importsection, &address](TableType  arg){
                std::cout << "01  ; import kind: table" << std::endl;
                importsection.stream.print_address(++address);
                std::cout << (int)arg.reftype << "  ; reftype" << std::endl;
                    importsection.stream.print_address(++address);
                if(arg.limits.max.has_value()){
                    std::cout << "01  ; maximum presented" << std::endl;
                    importsection.stream.print_address(++address);
                    std::cout << std::setfill('0') << std::setw(2);
                    std::cout << (int)arg.limits.min << "  ; min" << std::endl;
                    importsection.stream.print_address(++address);
                    std::cout << std::setfill('0') << std::setw(2);
                    std::cout << (int)*arg.limits.max << "  ; max" << std::endl;
                }else{
                    std::cout << "00  ; no maximum presented" << std::endl;
                    importsection.stream.print_address(++address);
                    std::cout << std::setfill('0') << std::setw(2);
                    std::cout << (int)arg.limits.min << "  ; min" << std::endl;
                }
            },
            [&importsection, &address](MemType arg){
                std::cout << "02  ; import kind: memory" << std::endl;
                importsection.stream.print_address(++address);
                if(arg.max.has_value()){
                    std::cout << "01  ; maximum presented" << std::endl;
                    importsection.stream.print_address(++address);
                    std::cout << std::setfill('0') << std::setw(2);
                    std::cout << (int)arg.min << "  ; min" << std::endl;
                    importsection.stream.print_address(++address);
                    std::cout << std::setfill('0') << std::setw(2);
                    std::cout << (int)*arg.max << "  ; max" << std::endl;
                }else{
                    std::cout << "00  ; no maximum presented" << std::endl;
                    importsection.stream.print_address(++address);
                    std::cout << std::setfill('0') << std::setw(2);
                    std::cout << (int)arg.min << "  ; min" << std::endl;
                }

            },
            [&importsection, &address](GlobalType arg){
                std::cout << "03  ; import kind: global" << std::endl;
                importsection.stream.print_address(++address);
                std::cout << (int)arg.type << "  ; valtype: " << arg.type << std::endl;
                importsection.stream.print_address(++address);
                std::cout << std::setfill('0') << std::setw(2);
                std::cout << arg.mut << "  ; mutation" << std::endl;
            }
        }, import.desc);

    }

    std::cout.setf(flags);

    return os;
}

std::ostream& Objdump::operator<<(std::ostream& os, Objdump::FunctionSection& functionsection){
    size_t address = functionsection.size_address;
    std::cout << (Objdump::Section&)functionsection;
    functionsection.stream.print_address(++address);

    std::ios::fmtflags flags = std::cout.flags();
    std::cout << std::hex;
    std::cout << std::setfill('0') << std::setw(2) << functionsection.functions.size() << "  ; Number of func" << std::endl;

    int order = 0;
    for(index_t idx : functionsection.functions){
        functionsection.stream.print_address(++address);
        // ToDO: it doesn't prinnt out raw data, the decoded typeidx instead
        std::cout << std::setfill('0') << std::setw(2) << idx << "  ; signature ID for function " << order <<std::endl;
        order++;
    }

    return os;
}

std::ostream& Objdump::operator<<(std::ostream& os, Objdump::TableSection& tablesection){
    size_t address = tablesection.size_address;
    std::cout << (Objdump::Section&)tablesection;
    tablesection.stream.print_address(++address);

    std::ios::fmtflags flags = std::cout.flags();
    std::cout << std::hex;
    std::cout << std::setfill('0') << std::setw(2) << tablesection.tables.size() << "  ; Number of table" << std::endl;

    // ToDO: re design an print limit
    // for(TableType tb : tablesection.tables){
    //     functionsection.stream.print_address(++address);
    //     std::cout << tb.


    // }

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

// dump raw
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
