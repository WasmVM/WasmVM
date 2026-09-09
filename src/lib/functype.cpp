// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <Types.hpp>
#include <Util.hpp>
#include <exception.hpp>
#include <optional>
#include <string>
#include <vector>

using namespace WasmVM;

bool WasmVM::value_matches(const Value& value, const ValueType& type){
    switch(type.kind){
        case ValueType::i32:      return std::holds_alternative<i32_t>(value);
        case ValueType::i64:      return std::holds_alternative<i64_t>(value);
        case ValueType::f32:      return std::holds_alternative<f32_t>(value);
        case ValueType::f64:      return std::holds_alternative<f64_t>(value);
        case ValueType::funcref:
        case ValueType::nullfuncref:   return std::holds_alternative<funcref_t>(value);
        case ValueType::externref:
        case ValueType::nullexternref: return std::holds_alternative<externref_t>(value);
        case ValueType::i31ref:   return std::holds_alternative<i31ref_t>(value);
        case ValueType::exnref:   return std::holds_alternative<exnref_t>(value);
        case ValueType::structref: return std::holds_alternative<structref_t>(value);
        case ValueType::arrayref:  return std::holds_alternative<arrayref_t>(value);
        // Several representations inhabit these, and a concrete reference's
        // type index is checked statically, not here.
        case ValueType::anyref:
        case ValueType::eqref:
        case ValueType::nullref:
        case ValueType::ref:
        break;
    }
    return true;
}

std::size_t std::hash<WasmVM::ValueType>::operator()(const WasmVM::ValueType& type) const {
    std::size_t value = (std::size_t)type.kind;
    if(type.is_ref()){
        value = (value << 1) | (type.nullable ? 1 : 0);
    }
    if(type.is_concrete()){
        value = (value << 32) ^ (std::size_t)type.typeidx;
    }
    return value;
};

static void hash_valtypes(std::string& signature, const std::vector<WasmVM::ValueType>& types){
    std::hash<WasmVM::ValueType> hashfunc;
    for(const WasmVM::ValueType& type : types){
        signature += std::to_string(hashfunc(type));
        signature += ',';
    }
}

std::size_t std::hash<WasmVM::FuncType>::operator()(const WasmVM::FuncType& functype) const {
    std::string signature;
    hash_valtypes(signature, functype.params);
    signature += ':';
    hash_valtypes(signature, functype.results);
    return std::hash<std::string>{}(signature);
};

bool WasmVM::operator==(const WasmVM::FuncType& a, const WasmVM::FuncType& b){
    std::hash<WasmVM::FuncType> hashfunc;
    return hashfunc(a) == hashfunc(b);
}

bool WasmVM::operator==(const FieldType& a, const FieldType& b){
    return (a.mut == b.mut) && (a.storage == b.storage);
}
bool WasmVM::operator==(const StructType& a, const StructType& b){
    return a.fields == b.fields;
}
bool WasmVM::operator==(const ArrayType& a, const ArrayType& b){
    return a.field == b.field;
}
bool WasmVM::operator==(const RecGroup& a, const RecGroup& b){
    return (a.first == b.first) && (a.count == b.count);
}
bool WasmVM::operator==(const SubType& a, const SubType& b){
    return (a.final == b.final) && (a.supers == b.supers) && (a.comp == b.comp);
}

const FuncType& SubType::func() const {
    if(!is_func()){
        throw Exception::Exception("type index is not a function type");
    }
    return std::get<FuncType>(comp);
}
FuncType& SubType::func(){
    if(!is_func()){
        throw Exception::Exception("type index is not a function type");
    }
    return std::get<FuncType>(comp);
}

std::size_t std::hash<WasmVM::SubType>::operator()(const WasmVM::SubType& type) const {
    std::string signature;
    signature += type.final ? 'F' : 'f';
    for(index_t super : type.supers){
        signature += std::to_string(super);
        signature += ',';
    }
    signature += ':';
    auto field = [&](const FieldType& f){
        signature += f.mut ? 'm' : 'c';
        std::visit(overloaded {
            [&](ValueType v){ signature += std::to_string(std::hash<ValueType>{}(v)); },
            [&](PackedType p){ signature += (p == PackedType::i8) ? 'b' : 'h'; }
        }, f.storage);
        signature += '.';
    };
    std::visit(overloaded {
        [&](const FuncType& f){ signature += 'F'; signature += std::to_string(std::hash<FuncType>{}(f)); },
        [&](const StructType& s){ signature += 'S'; for(const FieldType& f : s.fields) field(f); },
        [&](const ArrayType& a){ signature += 'A'; field(a.field); }
    }, type.comp);
    return std::hash<std::string>{}(signature);
}
// ── Heap types and subtyping ────────────────────────────────────────────────

std::optional<ValueType::Kind> WasmVM::abstract_heaptype(int32_t heaptype){
    switch(heaptype){
        case heaptype_nofunc:   return ValueType::nullfuncref;
        case heaptype_noextern: return ValueType::nullexternref;
        case heaptype_none:     return ValueType::nullref;
        case heaptype_func:     return ValueType::funcref;
        case heaptype_extern:   return ValueType::externref;
        case heaptype_any:      return ValueType::anyref;
        case heaptype_eq:       return ValueType::eqref;
        case heaptype_i31:      return ValueType::i31ref;
        case heaptype_struct:   return ValueType::structref;
        case heaptype_array:    return ValueType::arrayref;
        case heaptype_exn:      return ValueType::exnref;
        default: break;
    }
    return std::nullopt;
}

int32_t WasmVM::heaptype_of(const ValueType& type){
    switch(type.kind){
        case ValueType::nullfuncref:   return heaptype_nofunc;
        case ValueType::nullexternref: return heaptype_noextern;
        case ValueType::nullref:       return heaptype_none;
        case ValueType::funcref:       return heaptype_func;
        case ValueType::externref:     return heaptype_extern;
        case ValueType::anyref:        return heaptype_any;
        case ValueType::eqref:         return heaptype_eq;
        case ValueType::i31ref:        return heaptype_i31;
        case ValueType::structref:     return heaptype_struct;
        case ValueType::arrayref:      return heaptype_array;
        case ValueType::exnref:        return heaptype_exn;
        case ValueType::ref:           return (int32_t)type.typeidx;
        default: break;
    }
    return 0;
}

ValueType WasmVM::reference_to(int32_t heaptype, bool nullable){
    if(std::optional<ValueType::Kind> kind = abstract_heaptype(heaptype)){
        return ValueType(kind.value(), nullable);
    }
    return ValueType::concrete((index_t)heaptype, nullable);
}

// Which hierarchy an abstract kind belongs to. References only match within
// one: no funcref is ever an anyref.
namespace {
enum class Hierarchy { any, func, extern_, exn, none };

Hierarchy hierarchy_of_kind(ValueType::Kind kind){
    switch(kind){
        case ValueType::anyref: case ValueType::eqref: case ValueType::i31ref:
        case ValueType::structref: case ValueType::arrayref: case ValueType::nullref:
            return Hierarchy::any;
        case ValueType::funcref: case ValueType::nullfuncref:
            return Hierarchy::func;
        case ValueType::externref: case ValueType::nullexternref:
            return Hierarchy::extern_;
        case ValueType::exnref:
            return Hierarchy::exn;
        default:
            return Hierarchy::none;
    }
}

// The abstract kind a concrete type index sits under.
ValueType::Kind kind_of_comp(index_t typeidx, const std::vector<SubType>& types){
    if(typeidx >= types.size()){
        return ValueType::anyref;
    }
    if(types[typeidx].is_struct()) return ValueType::structref;
    if(types[typeidx].is_array())  return ValueType::arrayref;
    return ValueType::funcref;
}

Hierarchy hierarchy_of(const ValueType& type, const std::vector<SubType>& types){
    if(type.is_concrete()){
        return hierarchy_of_kind(kind_of_comp(type.typeidx, types));
    }
    return hierarchy_of_kind(type.kind);
}

// Whether abstract kind `a` is at or below abstract kind `b`.
bool abstract_matches(ValueType::Kind a, ValueType::Kind b){
    if(a == b) return true;
    switch(b){
        case ValueType::anyref:
            return (a == ValueType::eqref) || (a == ValueType::i31ref)
                || (a == ValueType::structref) || (a == ValueType::arrayref)
                || (a == ValueType::nullref);
        case ValueType::eqref:
            return (a == ValueType::i31ref) || (a == ValueType::structref)
                || (a == ValueType::arrayref) || (a == ValueType::nullref);
        case ValueType::i31ref: case ValueType::structref: case ValueType::arrayref:
            return a == ValueType::nullref;
        case ValueType::funcref:
            return a == ValueType::nullfuncref;
        case ValueType::externref:
            return a == ValueType::nullexternref;
        default:
            return false;
    }
}
} // namespace

bool WasmVM::storage_matches(const StorageType& a, const StorageType& b, const std::vector<SubType>& types){
    if(std::holds_alternative<PackedType>(a) || std::holds_alternative<PackedType>(b)){
        return a == b;
    }
    return type_matches(std::get<ValueType>(a), std::get<ValueType>(b), types);
}

bool WasmVM::field_matches(const FieldType& a, const FieldType& b, const std::vector<SubType>& types){
    if(a.mut != b.mut){
        return false;
    }
    // A mutable field is read and written, so it is invariant; an immutable
    // one is only read, and may be refined.
    if(b.mut){
        return a.storage == b.storage;
    }
    return storage_matches(a.storage, b.storage, types);
}

bool WasmVM::comp_matches(index_t a, index_t b, const std::vector<SubType>& types){
    if(a == b){
        return true;
    }
    if((a >= types.size()) || (b >= types.size())){
        return false;
    }
    // A declared supertype, followed transitively. The type section's own
    // rules keep this from looping: a supertype is always defined earlier.
    for(index_t super : types[a].supers){
        if(comp_matches(super, b, types)){
            return true;
        }
    }
    return comp_refines(a, b, types);
}

bool WasmVM::comp_refines(index_t a, index_t b, const std::vector<SubType>& types){
    if(a == b){
        return true;
    }
    if((a >= types.size()) || (b >= types.size())){
        return false;
    }
    const SubType& sub = types[a];
    const SubType& sup = types[b];
    if(sub.comp.index() != sup.comp.index()){
        return false;
    }
    if(sub.is_struct()){
        // A struct refines another by keeping its fields, in order, and may
        // add more after them.
        const StructType& x = std::get<StructType>(sub.comp);
        const StructType& y = std::get<StructType>(sup.comp);
        if(x.fields.size() < y.fields.size()){
            return false;
        }
        for(size_t i = 0; i < y.fields.size(); ++i){
            if(!field_matches(x.fields[i], y.fields[i], types)){
                return false;
            }
        }
        return true;
    }
    if(sub.is_array()){
        return field_matches(std::get<ArrayType>(sub.comp).field,
                             std::get<ArrayType>(sup.comp).field, types);
    }
    // A function refines another with contravariant parameters and covariant
    // results.
    const FuncType& x = sub.func();
    const FuncType& y = sup.func();
    if((x.params.size() != y.params.size()) || (x.results.size() != y.results.size())){
        return false;
    }
    for(size_t i = 0; i < x.params.size(); ++i){
        if(!type_matches(y.params[i], x.params[i], types)){
            return false;
        }
    }
    for(size_t i = 0; i < x.results.size(); ++i){
        if(!type_matches(x.results[i], y.results[i], types)){
            return false;
        }
    }
    return true;
}

Value WasmVM::default_value(const ValueType& type, const std::vector<SubType>& types){
    switch(type.kind){
        case ValueType::i32: return Value((i32_t)0);
        case ValueType::i64: return Value((i64_t)0);
        case ValueType::f32: return Value((f32_t)0.0f);
        case ValueType::f64: return Value((f64_t)0.0);
        case ValueType::funcref: case ValueType::nullfuncref:
            return Value(funcref_t(std::nullopt));
        case ValueType::externref: case ValueType::nullexternref:
            return Value((externref_t)nullptr);
        case ValueType::i31ref: return Value(i31ref_t());
        case ValueType::exnref: return Value(exnref_t());
        case ValueType::arrayref: return Value(arrayref_t());
        case ValueType::ref: {
            if(type.typeidx < types.size()){
                if(types[type.typeidx].is_array()) return Value(arrayref_t());
                if(types[type.typeidx].is_func())  return Value(funcref_t(std::nullopt));
            }
            return Value(structref_t());
        }
        // structref, and the abstract types of the any hierarchy whose null is
        // not tied to one representation.
        default: break;
    }
    return Value(structref_t());
}

Value WasmVM::default_value(const StorageType& storage, const std::vector<SubType>& types){
    if(std::holds_alternative<PackedType>(storage)){
        return Value((i32_t)0);
    }
    return default_value(std::get<ValueType>(storage), types);
}

bool WasmVM::type_matches(const ValueType& a, const ValueType& b, const std::vector<SubType>& types){
    if(a == b){
        return true;
    }
    if(!a.is_ref() || !b.is_ref()){
        return a.kind == b.kind;
    }
    // A non-null reference satisfies a nullable expectation, never the reverse.
    if(a.nullable && !b.nullable){
        return false;
    }
    if(hierarchy_of(a, types) != hierarchy_of(b, types)){
        return false;
    }
    // The bottom of a hierarchy is below everything in it.
    if(a.is_bottom()){
        return true;
    }
    if(b.is_bottom()){
        return false;
    }
    if(a.is_concrete() && b.is_concrete()){
        return comp_matches(a.typeidx, b.typeidx, types);
    }
    if(a.is_concrete()){
        return abstract_matches(kind_of_comp(a.typeidx, types), b.kind);
    }
    if(b.is_concrete()){
        return false; // an abstract type is never below a concrete one
    }
    return abstract_matches(a.kind, b.kind);
}
