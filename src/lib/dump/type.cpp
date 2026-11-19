// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "dump.hpp"
#include <Util.hpp>

using namespace WasmVM;

std::ostream& WasmVM::operator<<(std::ostream& stream, const ValueType& type){
    // A non-null reference, and any reference to a concrete type, has no
    // shorthand: it prints as (ref null? heaptype).
    if(type.is_ref() && (!type.nullable || type.is_concrete())){
        stream << "(ref ";
        if(type.nullable){
            stream << "null ";
        }
        if(type.is_concrete()){
            return dump_ref(stream, DumpSpace::type, type.typeidx) << ")";
        }
        switch(type.kind){
            case ValueType::funcref:       stream << "func"; break;
            case ValueType::externref:     stream << "extern"; break;
            case ValueType::anyref:        stream << "any"; break;
            case ValueType::eqref:         stream << "eq"; break;
            case ValueType::i31ref:        stream << "i31"; break;
            case ValueType::structref:     stream << "struct"; break;
            case ValueType::arrayref:      stream << "array"; break;
            case ValueType::nullref:       stream << "none"; break;
            case ValueType::nullfuncref:   stream << "nofunc"; break;
            case ValueType::nullexternref: stream << "noextern"; break;
            default:                       stream << "exn"; break;
        }
        return stream << ")";
    }
    switch(type.kind){
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
        case ValueType::structref:
            stream << "structref";
        break;
        case ValueType::arrayref:
            stream << "arrayref";
        break;
        case ValueType::anyref:
            stream << "anyref";
        break;
        case ValueType::eqref:
            stream << "eqref";
        break;
        case ValueType::nullref:
            stream << "nullref";
        break;
        case ValueType::nullfuncref:
            stream << "nullfuncref";
        break;
        case ValueType::nullexternref:
            stream << "nullexternref";
        break;
        case ValueType::ref:
        break; // handled above
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

// `typeidx` is the definition's own index, which names its parameters and
// fields; a function type with none prints as before.
static std::ostream& dump_comptype(std::ostream& stream, const CompType& comp, std::optional<index_t> typeidx = std::nullopt){
    const DumpNames* names = DumpNames::current();
    std::visit(overloaded {
        [&](const FuncType& type){
            stream << "(func";
            bool named = false;
            if(names && typeidx){
                for(index_t i = 0; i < type.params.size(); ++i){
                    named = named || names->type_param(typeidx.value(), i);
                }
            }
            if(named){
                // A named parameter takes a (param ...) of its own.
                for(index_t i = 0; i < type.params.size(); ++i){
                    stream << " (param";
                    dump_def_name(stream, names->type_param(typeidx.value(), i));
                    stream << " " << type.params[i] << ")";
                }
            }else if(!type.params.empty()){
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
            for(index_t i = 0; i < type.fields.size(); ++i){
                stream << " (field ";
                if(names && typeidx){
                    if(const std::string* name = names->field(typeidx.value(), i)){
                        stream << "$" << *name << " ";
                    }
                }
                dump_field(stream, type.fields[i]);
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

std::ostream& WasmVM::dump_subtype(std::ostream& stream, const SubType& type, const char* indent, std::optional<index_t> typeidx){
    stream << indent << "(type ";
    if(DumpNames* names = DumpNames::current(); names && typeidx){
        // Defined before its body: a type may refer to itself.
        if(const std::string* name = names->define(DumpSpace::type, typeidx.value())){
            stream << "$" << *name << " ";
        }
    }
    // A final subtype with no supertype is its bare composite type.
    bool sub = !type.final || !type.supers.empty();
    if(sub){
        stream << "(sub ";
        if(type.final){
            stream << "final ";
        }
        for(index_t super : type.supers){
            dump_ref(stream, DumpSpace::type, super) << " ";
        }
    }
    dump_comptype(stream, type.comp, typeidx);
    if(sub){
        stream << " )";
    }
    return stream << " )" << std::endl;
}

std::ostream& WasmVM::operator<<(std::ostream& stream, const SubType& type){
    return dump_subtype(stream, type, "  ");
}

std::ostream& WasmVM::operator<<(std::ostream& stream, const TableType& table){
    return dump_table(stream, table, nullptr);
}

std::ostream& WasmVM::dump_table(std::ostream& stream, const TableType& table, const std::string* name){
    stream << "  (table";
    dump_def_name(stream, name) << " " << table.limits.min;
    if(table.limits.max.has_value()){
        stream << " " << table.limits.max.value();
    }
    stream << " " << table.elemtype << ")" << std::endl;
    return stream;
}

std::ostream& WasmVM::operator<<(std::ostream& stream, const MemType& mem){
    return dump_mem(stream, mem, nullptr);
}

std::ostream& WasmVM::dump_mem(std::ostream& stream, const MemType& mem, const std::string* name){
    stream << "  (memory";
    dump_def_name(stream, name) << " ";
    if(mem.is64) stream << "i64 ";
    stream << mem.min;
    if(mem.max.has_value()){
        stream << " " << mem.max.value();
    }
    stream << ")" << std::endl;
    return stream;
}