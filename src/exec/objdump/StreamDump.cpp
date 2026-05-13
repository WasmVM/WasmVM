// Copyright 2024 Luis Hsu, Jack Lin. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "objdump.hpp"
#include <iomanip>
#include <cmath>
#include <Util.hpp>

using namespace WasmVM;

// Dumper constructor: calculates address width based on file size
Objdump::Dumper::Dumper(std::istream& istream){
    istream.seekg(0, std::ios::end);
    size_t total_bytes = istream.tellg();
    istream.seekg(0, std::ios::beg);
    address_width = std::log2(total_bytes) / 4 + 1;
}

// Print address from Bytes
void Objdump::Dumper::print_address(std::ostream& os, const Bytes& bytes) const {
    std::ios::fmtflags flags = os.flags();
    os << "0x" << std::hex << std::setfill('0') << std::setw(address_width) << bytes.address << ": ";
    os.setf(flags);
}

// Print address from position
void Objdump::Dumper::print_address(std::ostream& os, long address) const {
    std::ios::fmtflags flags = os.flags();
    os << "0x" << std::hex << std::setfill('0') << std::setw(address_width) << address << ": ";
    os.setf(flags);
}

// Print hex bytes
void Objdump::Dumper::print_hex(std::ostream& os, const Bytes& bytes) const {
    std::ios::fmtflags flags = os.flags();
    os << std::hex;
    for(size_t i = 0; i < bytes.size(); i++){
        os << std::setfill('0') << std::setw(2) << (int)bytes[i] << " ";
    }
    os.setf(flags);
}

// ----- OUTPUT (DUMPING) -----

// Use it when you need to print HEX raw data
std::ostream& Objdump::operator<<(std::ostream& os, Objdump::Bytes& bytes){
    std::ios::fmtflags flags = os.flags();
    os << std::hex;
    for(size_t i = 0; i < bytes.size(); i++){
        os << std::setfill('0') << std::setw(2) << (int)bytes[i] << " ";
    }
    os.setf(flags);

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
    Dumper dumper(section.stream.get_istream());
    
    dumper.print_address(os, section.id);
    os << section.id << " ; Section ID" << std::endl;

    u32_t SizeInDec = 0;
    for (int i = 0; i < section.size.size(); i++){
        SizeInDec |= ((int)section.size[i] & 0x7F) << (i*7);
    }
    dumper.print_address(os, section.size_address);
    os << section.size << " ; Section size (" << SizeInDec << ")"<< std::endl;

    return os;
}

std::ostream& Objdump::operator<<(std::ostream& os, Objdump::TypeSection& section){
    long address = (long)section.size_address + 1;
    os << (Objdump::Section&)section;
    
    Dumper dumper(section.stream.get_istream());
    dumper.print_address(os, address);

    std::ios::fmtflags flags = os.flags();
    os << std::hex;
    os << std::setfill('0') << std::setw(2) << section.functype.size() << "  ; Number of types" << std::endl;

    for(FuncType functype : section.functype){
        dumper.print_address(os, ++address);
        os << "60" << "  ; Function" << std::endl;
        // Params
        dumper.print_address(os, ++address);
        os << std::setfill('0') << std::setw(2) << functype.params.size() << "  ; num params" << std::endl;
        for(ValueType param : functype.params){
            dumper.print_address(os, ++address);
            os << (int)param << "  ; " << param << std::endl;
        }
        // Results
        dumper.print_address(os, ++address);
        os << std::setfill('0') << std::setw(2) << functype.results.size() << "  ; num results" << std::endl;
        for(ValueType result : functype.results){
            dumper.print_address(os, ++address);
            os << (int)result << "  ; " << result << std::endl;
        }

    }
    os.setf(flags);

    return os;
}

std::ostream& Objdump::operator<<(std::ostream& os, Objdump::ImportSection& importsection){
    long address = (long)importsection.size_address + 1;
    os << (Objdump::Section&)importsection;
    
    Dumper dumper(importsection.stream.get_istream());
    dumper.print_address(os, address);

    std::ios::fmtflags flags = os.flags();
    os << std::hex;
    os << std::setfill('0') << std::setw(2) << importsection.imports.size() << "  ; Number of imports" << std::endl;
    
    for(WasmImport import : importsection.imports){     
        // mod
        dumper.print_address(os, ++address);
        os << std::setfill('0') << std::setw(2) << import.module.length() << "  ; mod length" << std::endl;
        dumper.print_address(os, ++address);
        for(int i = 0; i < import.module.size(); i++){
            os << std::hex << (int)import.module[i] << " ";
        }
        os << " ; " << import.module << std::endl;
        address += import.module.length();

        // nm
        dumper.print_address(os, address);
        os << std::setfill('0') << std::setw(2) << import.name.length() << "  ; name length" << std::endl;
        dumper.print_address(os, ++address);
        for(int i = 0; i < import.name.size(); i++){
            os << std::hex << (int)import.name[i] << " ";
        }
        os << " ; " << import.name << std::endl;
        address += import.name.length();

        dumper.print_address(os, address);
        std::visit(overloaded{
            [&dumper, &os, &address](index_t arg){
                os << "00  ; import kind: func" << std::endl;
                dumper.print_address(os, ++address);
                os << std::setfill('0') << std::setw(2);
                os << arg << "  ; signature index" << std::endl;
            },
            [&dumper, &os, &address](TableType  arg){
                os << "01  ; import kind: table" << std::endl;
                dumper.print_address(os, ++address);
                os << (int)arg.reftype << "  ; reftype" << std::endl;
                dumper.print_address(os, ++address);
                if(arg.limits.max.has_value()){
                    os << "01  ; maximum presented" << std::endl;
                    dumper.print_address(os, ++address);
                    os << std::setfill('0') << std::setw(2);
                    os << (int)arg.limits.min << "  ; min" << std::endl;
                    dumper.print_address(os, ++address);
                    os << std::setfill('0') << std::setw(2);
                    os << (int)*arg.limits.max << "  ; max" << std::endl;
                }else{
                    os << "00  ; no maximum presented" << std::endl;
                    dumper.print_address(os, ++address);
                    os << std::setfill('0') << std::setw(2);
                    os << (int)arg.limits.min << "  ; min" << std::endl;
                }
            },
            [&dumper, &os, &address](MemType arg){
                os << "02  ; import kind: memory" << std::endl;
                dumper.print_address(os, ++address);
                if(arg.max.has_value()){
                    os << "01  ; maximum presented" << std::endl;
                    dumper.print_address(os, ++address);
                    os << std::setfill('0') << std::setw(2);
                    os << (int)arg.min << "  ; min" << std::endl;
                    dumper.print_address(os, ++address);
                    os << std::setfill('0') << std::setw(2);
                    os << (int)*arg.max << "  ; max" << std::endl;
                }else{
                    os << "00  ; no maximum presented" << std::endl;
                    dumper.print_address(os, ++address);
                    os << std::setfill('0') << std::setw(2);
                    os << (int)arg.min << "  ; min" << std::endl;
                }

            },
            [&dumper, &os, &address](GlobalType arg){
                os << "03  ; import kind: global" << std::endl;
                dumper.print_address(os, ++address);
                os << (int)arg.type << "  ; valtype: " << arg.type << std::endl;
                dumper.print_address(os, ++address);
                os << std::setfill('0') << std::setw(2);
                os << arg.mut << "  ; mutation" << std::endl;
            }
        }, import.desc);

    }

    os.setf(flags);

    return os;
}

std::ostream& Objdump::operator<<(std::ostream& os, Objdump::FunctionSection& functionsection){
    long address = (long)functionsection.size_address + 1;
    os << (Objdump::Section&)functionsection;
    
    Dumper dumper(functionsection.stream.get_istream());
    dumper.print_address(os, address);

    std::ios::fmtflags flags = os.flags();
    os << std::hex;
    os << std::setfill('0') << std::setw(2) << functionsection.functions.size() << "  ; Number of func" << std::endl;

    int order = 0;
    for(index_t idx : functionsection.functions){
        dumper.print_address(os, ++address);
        os << std::setfill('0') << std::setw(2) << idx << "  ; signature ID for function " << order <<std::endl;
        order++;
    }

    return os;
}

std::ostream& Objdump::operator<<(std::ostream& os, Objdump::TableSection& tablesection){
    long address = (long)tablesection.size_address + 1;
    os << (Objdump::Section&)tablesection;
    
    Dumper dumper(tablesection.stream.get_istream());
    dumper.print_address(os, address);

    std::ios::fmtflags flags = os.flags();
    os << std::hex;
    os << std::setfill('0') << std::setw(2) << tablesection.tables.size() << "  ; Number of table" << std::endl;

    for(TableType tb : tablesection.tables){
        dumper.print_address(os, ++address);
        os << (int)tb.reftype << "  ; Reference Type" << std:: endl;
        dumper.print_address(os, ++address);
        if(tb.limits.max.has_value()){
            os << "01  ; maximum presented" << std::endl;
            dumper.print_address(os, ++address);
            os << std::setfill('0') << std::setw(2);
            os << (int)tb.limits.min << "  ; min" << std::endl;
            dumper.print_address(os, ++address);
            os << std::setfill('0') << std::setw(2);
            os << (int)*tb.limits.max << "  ; max" << std::endl;
        }else{
            os << "00  ; no maximum presented" << std::endl;
            dumper.print_address(os, ++address);
            os << std::setfill('0') << std::setw(2);
            os << (int)tb.limits.min << "  ; min" << std::endl;
        }
    }

    return os;
}

std::ostream& Objdump::operator<<(std::ostream& os, Objdump::MemorySection& memorysection){
    long address = (long)memorysection.size_address + 1;
    os << (Objdump::Section&)memorysection;
    
    Dumper dumper(memorysection.stream.get_istream());
    dumper.print_address(os, address);

    std::ios::fmtflags flags = os.flags();
    os << std::hex;
    os << std::setfill('0') << std::setw(2) << memorysection.memories.size() << "  ; Number of memory" << std::endl;

    for(MemType mt : memorysection.memories){
        dumper.print_address(os, ++address);
        if(mt.max.has_value()){
            os << "01  ; maximum presented" << std::endl;
            dumper.print_address(os, ++address);
            os << std::setfill('0') << std::setw(2);
            os << (int)mt.min << "  ; min" << std::endl;
            dumper.print_address(os, ++address);
            os << std::setfill('0') << std::setw(2);
            os << (int)*mt.max << "  ; max" << std::endl;
        }else{
            os << "00  ; no maximum presented" << std::endl;
            dumper.print_address(os, ++address);
            os << std::setfill('0') << std::setw(2);
            os << (int)mt.min << "  ; min" << std::endl;
        }
    }

    return os;
}

std::ostream& Objdump::operator<<(std::ostream& os, Objdump::GlobalSection& globalsection){
    long address = (long)globalsection.size_address + 1;
    os << (Objdump::Section&)globalsection;
    
    Dumper dumper(globalsection.stream.get_istream());
    dumper.print_address(os, address);

    std::ios::fmtflags flags = os.flags();
    os << std::hex;
    os << std::setfill('0') << std::setw(2) << globalsection.globals.size() << "  ; Number of global" << std::endl;

    for(WasmGlobal gt : globalsection.globals){
        os << gt.type.type << std::endl;
        os << (gt.type.mut ? "variable" : "const") << std::endl;
    }

    os.setf(flags);

    return os;
}

// dump raw
std::ostream& Objdump::operator<<(std::ostream& os, Stream& stream){
    Dumper dumper(stream.istream);
    while(!stream.istream.eof()){
        Bytes bytes(1);
        stream >> bytes;
        dumper.print_address(os, bytes);
        os << bytes << std::endl;
    }
    return os;
}
