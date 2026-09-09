// Copyright 2026 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// The struct and array instructions of the GC proposal.
//
// A struct or array lives on the C++ heap behind a shared_ptr, so a reference
// is null exactly when the pointer is. A packed field is held as an i32
// already truncated to its width; the _s and _u forms extend it on the way
// out, which is why an unpacked field must not use them and a packed one must.
// Validation settles all of that, so what is left here is the arithmetic, the
// null checks and the bounds checks -- the three things the spec makes traps.

#include <structures/WasmInstr.hpp>
#include <Util.hpp>
#include "RunVisitor.hpp"
#include "exception.hpp"

#include <algorithm>
#include <cstring>
#include <memory>

using namespace WasmVM;

namespace {

Label& top_label(Stack& stack){
    return stack.frames.top().labels.top();
}

Value pop(Stack& stack){
    Label& label = top_label(stack);
    Value value = label.values.top();
    label.values.pop();
    return value;
}

i32_t pop_i32(Stack& stack){
    return std::get<i32_t>(pop(stack));
}

const SubType& named_type(Stack& stack, index_t typeidx){
    const std::vector<SubType>& types = stack.frames.top().module.types;
    if(typeidx >= types.size()){
        throw Exception::invalid_value();
    }
    return types[typeidx];
}

const StructType& struct_type(Stack& stack, index_t typeidx){
    const SubType& type = named_type(stack, typeidx);
    if(!type.is_struct()){
        throw Exception::invalid_value();
    }
    return std::get<StructType>(type.comp);
}

const ArrayType& array_type(Stack& stack, index_t typeidx){
    const SubType& type = named_type(stack, typeidx);
    if(!type.is_array()){
        throw Exception::invalid_value();
    }
    return std::get<ArrayType>(type.comp);
}

// A packed field keeps only its own width, so that struct.get_u reads back
// what was written and struct.get_s sign-extends from the right bit.
Value store_field(const FieldType& field, Value value){
    if(std::holds_alternative<PackedType>(field.storage)){
        i32_t bits = std::get<i32_t>(value);
        return Value((std::get<PackedType>(field.storage) == PackedType::i8)
                         ? (i32_t)(bits & 0xff)
                         : (i32_t)(bits & 0xffff));
    }
    return value;
}

// `signed_form` is true for the _s reading, false for _u; it is meaningless
// for an unpacked field, which is returned as it is.
Value load_field(const FieldType& field, const Value& value, bool signed_form){
    if(!std::holds_alternative<PackedType>(field.storage)){
        return value;
    }
    i32_t bits = std::get<i32_t>(value);
    if(!signed_form){
        return value;
    }
    return Value((std::get<PackedType>(field.storage) == PackedType::i8)
                     ? (i32_t)(i8_t)bits
                     : (i32_t)(i16_t)bits);
}

Value field_default(Stack& stack, const FieldType& field){
    return default_value(field.storage, stack.frames.top().module.types);
}

structref_t pop_struct(Stack& stack){
    Value value = pop(stack);
    if(!std::holds_alternative<structref_t>(value)){
        throw Exception::invalid_value();
    }
    structref_t ref = std::get<structref_t>(value);
    if(!ref){
        throw Exception::null_reference();
    }
    return ref;
}

arrayref_t pop_array(Stack& stack){
    Value value = pop(stack);
    if(!std::holds_alternative<arrayref_t>(value)){
        throw Exception::invalid_value();
    }
    arrayref_t ref = std::get<arrayref_t>(value);
    if(!ref){
        throw Exception::null_reference();
    }
    return ref;
}

// An access of `count` elements starting at `offset` must lie inside `size`.
// Computed in 64 bits so that an offset near 2^32 cannot wrap into range.
void check_bounds(u64_t offset, u64_t count, size_t size){
    if((offset + count) > (u64_t)size){
        throw Exception::length_too_long();
    }
}

const FieldType& field_at(const StructType& type, index_t fieldidx){
    if(fieldidx >= type.fields.size()){
        throw Exception::invalid_value();
    }
    return type.fields[fieldidx];
}

// struct.get / struct.get_s / struct.get_u
void struct_get(Stack& stack, index_t typeidx, index_t fieldidx, bool signed_form){
    const FieldType& field = field_at(struct_type(stack, typeidx), fieldidx);
    structref_t ref = pop_struct(stack);
    if(fieldidx >= ref->fields.size()){
        throw Exception::invalid_value();
    }
    top_label(stack).values.emplace(load_field(field, ref->fields[fieldidx], signed_form));
}

// array.get / array.get_s / array.get_u
void array_get(Stack& stack, index_t typeidx, bool signed_form){
    const ArrayType& type = array_type(stack, typeidx);
    i32_t index = pop_i32(stack);
    arrayref_t ref = pop_array(stack);
    check_bounds((u64_t)(u32_t)index, 1, ref->elems.size());
    top_label(stack).values.emplace(load_field(type.field, ref->elems[(u32_t)index], signed_form));
}

} // namespace

// ── struct ──────────────────────────────────────────────────────────────────

void RunVisitor::operator()(Instr::Struct_new& instr){
    const StructType& type = struct_type(stack, instr.index);
    structref_t obj = gc_new<StructObj>();
    obj->typeidx = instr.index;
    obj->fields.resize(type.fields.size());
    // The last field is on top of the stack.
    for(size_t i = type.fields.size(); i > 0; --i){
        obj->fields[i - 1] = store_field(type.fields[i - 1], pop(stack));
    }
    top_label(stack).values.emplace(Value(std::move(obj)));
}

void RunVisitor::operator()(Instr::Struct_new_default& instr){
    const StructType& type = struct_type(stack, instr.index);
    structref_t obj = gc_new<StructObj>();
    obj->typeidx = instr.index;
    for(const FieldType& field : type.fields){
        obj->fields.emplace_back(field_default(stack, field));
    }
    top_label(stack).values.emplace(Value(std::move(obj)));
}

void RunVisitor::operator()(Instr::Struct_get& instr){
    struct_get(stack, instr.typeidx, instr.fieldidx, false);
}
void RunVisitor::operator()(Instr::Struct_get_s& instr){
    struct_get(stack, instr.typeidx, instr.fieldidx, true);
}
void RunVisitor::operator()(Instr::Struct_get_u& instr){
    struct_get(stack, instr.typeidx, instr.fieldidx, false);
}

void RunVisitor::operator()(Instr::Struct_set& instr){
    const FieldType& field = field_at(struct_type(stack, instr.typeidx), instr.fieldidx);
    Value value = pop(stack);
    structref_t ref = pop_struct(stack);
    if(instr.fieldidx >= ref->fields.size()){
        throw Exception::invalid_value();
    }
    ref->fields[instr.fieldidx] = store_field(field, value);
}

// ── array ───────────────────────────────────────────────────────────────────

void RunVisitor::operator()(Instr::Array_new& instr){
    const ArrayType& type = array_type(stack, instr.index);
    i32_t count = pop_i32(stack);
    Value init = store_field(type.field, pop(stack));
    arrayref_t obj = gc_new<ArrayObj>();
    obj->typeidx = instr.index;
    obj->elems.assign((size_t)(u32_t)count, init);
    top_label(stack).values.emplace(Value(std::move(obj)));
}

void RunVisitor::operator()(Instr::Array_new_default& instr){
    const ArrayType& type = array_type(stack, instr.index);
    i32_t count = pop_i32(stack);
    arrayref_t obj = gc_new<ArrayObj>();
    obj->typeidx = instr.index;
    obj->elems.assign((size_t)(u32_t)count, field_default(stack, type.field));
    top_label(stack).values.emplace(Value(std::move(obj)));
}

void RunVisitor::operator()(Instr::Array_new_fixed& instr){
    const ArrayType& type = array_type(stack, instr.typeidx);
    arrayref_t obj = gc_new<ArrayObj>();
    obj->typeidx = instr.typeidx;
    obj->elems.resize((size_t)instr.n);
    // The last element is on top of the stack.
    for(size_t i = (size_t)instr.n; i > 0; --i){
        obj->elems[i - 1] = store_field(type.field, pop(stack));
    }
    top_label(stack).values.emplace(Value(std::move(obj)));
}

void RunVisitor::operator()(Instr::Array_new_data& instr){
    const ArrayType& type = array_type(stack, instr.typeidx);
    i32_t count = pop_i32(stack);
    i32_t offset = pop_i32(stack);
    index_t dataaddr = stack.frames.top().module.dataaddrs[instr.dataidx];
    const DataInst& data = stack.store.datas[dataaddr];
    // Each element takes as many bytes as its storage type is wide.
    const size_t width = std::holds_alternative<PackedType>(type.field.storage)
        ? ((std::get<PackedType>(type.field.storage) == PackedType::i8) ? 1 : 2)
        : ((std::get<ValueType>(type.field.storage).kind == ValueType::i64
            || std::get<ValueType>(type.field.storage).kind == ValueType::f64) ? 8 : 4);
    check_bounds((u64_t)(u32_t)offset, (u64_t)(u32_t)count * width, data.size());
    arrayref_t obj = gc_new<ArrayObj>();
    obj->typeidx = instr.typeidx;
    for(u32_t i = 0; i < (u32_t)count; ++i){
        u64_t at = (u64_t)(u32_t)offset + (u64_t)i * width;
        u64_t bits = 0;
        for(size_t b = 0; b < width; ++b){
            bits |= ((u64_t)(u8_t)data[(size_t)at + b]) << (8 * b);
        }
        if(std::holds_alternative<PackedType>(type.field.storage)){
            obj->elems.emplace_back(Value((i32_t)(u32_t)bits));
        }else{
            switch(std::get<ValueType>(type.field.storage).kind){
                case ValueType::i64: obj->elems.emplace_back(Value((i64_t)bits)); break;
                case ValueType::f32: {
                    u32_t word = (u32_t)bits; f32_t value;
                    std::memcpy(&value, &word, sizeof(value));
                    obj->elems.emplace_back(Value(value));
                }break;
                case ValueType::f64: {
                    f64_t value;
                    std::memcpy(&value, &bits, sizeof(value));
                    obj->elems.emplace_back(Value(value));
                }break;
                default: obj->elems.emplace_back(Value((i32_t)(u32_t)bits)); break;
            }
        }
    }
    top_label(stack).values.emplace(Value(std::move(obj)));
}

void RunVisitor::operator()(Instr::Array_new_elem& instr){
    array_type(stack, instr.typeidx);
    i32_t count = pop_i32(stack);
    i32_t offset = pop_i32(stack);
    index_t elemaddr = stack.frames.top().module.elemaddrs[instr.elemidx];
    const ElemInst& segment = stack.store.elems[elemaddr];
    check_bounds((u64_t)(u32_t)offset, (u64_t)(u32_t)count, segment.elem.size());
    arrayref_t obj = gc_new<ArrayObj>();
    obj->typeidx = instr.typeidx;
    for(u32_t i = 0; i < (u32_t)count; ++i){
        obj->elems.emplace_back(segment.elem[(size_t)((u32_t)offset + i)]);
    }
    top_label(stack).values.emplace(Value(std::move(obj)));
}

void RunVisitor::operator()(Instr::Array_get& instr){
    array_get(stack, instr.index, false);
}
void RunVisitor::operator()(Instr::Array_get_s& instr){
    array_get(stack, instr.index, true);
}
void RunVisitor::operator()(Instr::Array_get_u& instr){
    array_get(stack, instr.index, false);
}

void RunVisitor::operator()(Instr::Array_set& instr){
    const ArrayType& type = array_type(stack, instr.index);
    Value value = pop(stack);
    i32_t index = pop_i32(stack);
    arrayref_t ref = pop_array(stack);
    check_bounds((u64_t)(u32_t)index, 1, ref->elems.size());
    ref->elems[(u32_t)index] = store_field(type.field, value);
}

void RunVisitor::operator()(Instr::Array_len&){
    arrayref_t ref = pop_array(stack);
    top_label(stack).values.emplace(Value((i32_t)(u32_t)ref->elems.size()));
}

void RunVisitor::operator()(Instr::Array_fill& instr){
    const ArrayType& type = array_type(stack, instr.typeidx);
    i32_t count = pop_i32(stack);
    Value value = store_field(type.field, pop(stack));
    i32_t offset = pop_i32(stack);
    arrayref_t ref = pop_array(stack);
    check_bounds((u64_t)(u32_t)offset, (u64_t)(u32_t)count, ref->elems.size());
    std::fill_n(ref->elems.begin() + (u32_t)offset, (size_t)(u32_t)count, value);
}

void RunVisitor::operator()(Instr::Array_copy& instr){
    array_type(stack, instr.dst_typeidx);
    array_type(stack, instr.src_typeidx);
    i32_t count = pop_i32(stack);
    i32_t src_offset = pop_i32(stack);
    arrayref_t src = pop_array(stack);
    i32_t dst_offset = pop_i32(stack);
    arrayref_t dst = pop_array(stack);
    check_bounds((u64_t)(u32_t)src_offset, (u64_t)(u32_t)count, src->elems.size());
    check_bounds((u64_t)(u32_t)dst_offset, (u64_t)(u32_t)count, dst->elems.size());
    // The two may be the same array, so copy in the direction that does not
    // overwrite what has still to be read.
    if((dst == src) && ((u32_t)dst_offset > (u32_t)src_offset)){
        std::copy_backward(src->elems.begin() + (u32_t)src_offset,
                           src->elems.begin() + (u32_t)src_offset + (u32_t)count,
                           dst->elems.begin() + (u32_t)dst_offset + (u32_t)count);
    }else{
        std::copy(src->elems.begin() + (u32_t)src_offset,
                  src->elems.begin() + (u32_t)src_offset + (u32_t)count,
                  dst->elems.begin() + (u32_t)dst_offset);
    }
}

void RunVisitor::operator()(Instr::Array_init_data& instr){
    const ArrayType& type = array_type(stack, instr.typeidx);
    i32_t count = pop_i32(stack);
    i32_t src_offset = pop_i32(stack);
    i32_t dst_offset = pop_i32(stack);
    arrayref_t ref = pop_array(stack);
    index_t dataaddr = stack.frames.top().module.dataaddrs[instr.dataidx];
    const DataInst& data = stack.store.datas[dataaddr];
    const size_t width = std::holds_alternative<PackedType>(type.field.storage)
        ? ((std::get<PackedType>(type.field.storage) == PackedType::i8) ? 1 : 2)
        : ((std::get<ValueType>(type.field.storage).kind == ValueType::i64
            || std::get<ValueType>(type.field.storage).kind == ValueType::f64) ? 8 : 4);
    check_bounds((u64_t)(u32_t)src_offset, (u64_t)(u32_t)count * width, data.size());
    check_bounds((u64_t)(u32_t)dst_offset, (u64_t)(u32_t)count, ref->elems.size());
    for(u32_t i = 0; i < (u32_t)count; ++i){
        u64_t at = (u64_t)(u32_t)src_offset + (u64_t)i * width;
        u64_t bits = 0;
        for(size_t b = 0; b < width; ++b){
            bits |= ((u64_t)(u8_t)data[(size_t)at + b]) << (8 * b);
        }
        Value value((i32_t)(u32_t)bits);
        if(!std::holds_alternative<PackedType>(type.field.storage)){
            switch(std::get<ValueType>(type.field.storage).kind){
                case ValueType::i64: value = Value((i64_t)bits); break;
                case ValueType::f32: {
                    u32_t word = (u32_t)bits; f32_t f;
                    std::memcpy(&f, &word, sizeof(f));
                    value = Value(f);
                }break;
                case ValueType::f64: {
                    f64_t f;
                    std::memcpy(&f, &bits, sizeof(f));
                    value = Value(f);
                }break;
                default: break;
            }
        }
        ref->elems[(size_t)((u32_t)dst_offset + i)] = value;
    }
}

void RunVisitor::operator()(Instr::Array_init_elem& instr){
    array_type(stack, instr.typeidx);
    i32_t count = pop_i32(stack);
    i32_t src_offset = pop_i32(stack);
    i32_t dst_offset = pop_i32(stack);
    arrayref_t ref = pop_array(stack);
    index_t elemaddr = stack.frames.top().module.elemaddrs[instr.elemidx];
    const ElemInst& segment = stack.store.elems[elemaddr];
    check_bounds((u64_t)(u32_t)src_offset, (u64_t)(u32_t)count, segment.elem.size());
    check_bounds((u64_t)(u32_t)dst_offset, (u64_t)(u32_t)count, ref->elems.size());
    for(u32_t i = 0; i < (u32_t)count; ++i){
        ref->elems[(size_t)((u32_t)dst_offset + i)] = segment.elem[(size_t)((u32_t)src_offset + i)];
    }
}
