// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "dump.hpp"
#include <Util.hpp>

using namespace WasmVM;

std::ostream& WasmVM::operator<<(std::ostream& stream, const ValueType& type){
    switch(type){
        case ValueType::i32:
            stream << "i32";
        break;
        case ValueType::i64:
            stream << "i64";
        break;
        case ValueType::f32:
            stream << "f32";
        break;
        case ValueType::f64:
            stream << "f64";
        break;
        case ValueType::funcref:
            stream << "funcref";
        break;
        case ValueType::externref:
            stream << "externref";
        break;
        case ValueType::i31ref:
            stream << "i31ref";
        break;
        case ValueType::exnref:
            stream << "exnref";
        break;
    }
    return stream;
}

static std::ostream& dump_storage(std::ostream& stream, const StorageType& storage){
    std::visit(overloaded {
        [&](ValueType valtype){ stream << valtype; },
        [&](PackedType packed){ stream << ((packed == PackedType::i8) ? "i8" : "i16"); }
    }, storage);
    return stream;
}

static std::ostream& dump_field(std::ostream& stream, const FieldType& field){
    if(field.mut){
        stream << "(mut ";
        dump_storage(stream, field.storage);
        return stream << ")";
    }
    return dump_storage(stream, field.storage);
}

static std::ostream& dump_comptype(std::ostream& stream, const CompType& comp){
    std::visit(overloaded {
        [&](const FuncType& type){
            stream << "(func";
            if(!type.params.empty()){
                stream << " (param";
                for(auto& param : type.params){
                    stream << " " << param;
                }
                stream << ")";
            }
            if(!type.results.empty()){
                stream << " (result";
                for(auto& result : type.results){
                    stream << " " << result;
                }
                stream << ")";
            }
            stream << " )";
        },
        [&](const StructType& type){
            stream << "(struct";
            for(const FieldType& field : type.fields){
                stream << " (field ";
                dump_field(stream, field);
                stream << ")";
            }
            stream << " )";
        },
        [&](const ArrayType& type){
            stream << "(array ";
            dump_field(stream, type.field);
            stream << " )";
        }
    }, comp);
    return stream;
}

std::ostream& WasmVM::operator<<(std::ostream& stream, const FuncType& type){
    stream << "  (type ";
    dump_comptype(stream, CompType(type));
    return stream << " )" << std::endl;
}

std::ostream& WasmVM::dump_subtype(std::ostream& stream, const SubType& type, const char* indent){
    stream << indent << "(type ";
    // A final subtype with no supertype is its bare composite type.
    bool sub = !type.final || !type.supers.empty();
    if(sub){
        stream << "(sub ";
        if(type.final){
            stream << "final ";
        }
        for(index_t super : type.supers){
            stream << super << " ";
        }
    }
    dump_comptype(stream, type.comp);
    if(sub){
        stream << " )";
    }
    return stream << " )" << std::endl;
}

std::ostream& WasmVM::operator<<(std::ostream& stream, const SubType& type){
    return dump_subtype(stream, type, "  ");
}

std::ostream& WasmVM::operator<<(std::ostream& stream, const TableType& table){
    stream << "  (table " << table.limits.min;
    if(table.limits.max.has_value()){
        stream << " " << table.limits.max.value();
    }
    switch(table.reftype){
        case RefType::funcref :
            stream << " funcref";
        break;
        case RefType::externref :
            stream << " externref";
        break;
    }
    stream << ")" << std::endl;
    return stream;
}

std::ostream& WasmVM::operator<<(std::ostream& stream, const MemType& mem){
    stream << "  (memory ";
    if(mem.is64) stream << "i64 ";
    stream << mem.min;
    if(mem.max.has_value()){
        stream << " " << mem.max.value();
    }
    stream << ")" << std::endl;
    return stream;
}