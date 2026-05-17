#ifndef WASMVM_PP_STRUCTURE_INSTR
#define WASMVM_PP_STRUCTURE_INSTR

#include <variant>
#include <opcode.hpp>

namespace WasmVM {

namespace Instr {

struct Base {
    Base(Opcode::opcode_t op) : opcode(op) {}
    Opcode::opcode_t opcode;
};

template<Opcode::Opcode OP>
struct Atomic : public Base {
    Atomic() : Base(OP) {}
};

template<Opcode::Opcode OP>
struct OneIndex : public Base {
    OneIndex() : Base(OP) {}
    OneIndex(index_t index) : Base(OP), index(index){}
    index_t index;
};

template<Opcode::Opcode OP>
struct BlockInstr : public Base {
    BlockInstr(): Base(OP){}
    BlockInstr(std::optional<index_t> type): Base(OP), type(type){}
    std::optional<index_t> type;
};

// Control instructions
using Unreachable = Atomic<Opcode::Unreachable>;
using Nop = Atomic<Opcode::Nop>;
using End = Atomic<Opcode::End>;
using Call = OneIndex<Opcode::Call>;
using Block = BlockInstr<Opcode::Block>;
using Loop = BlockInstr<Opcode::Loop>;
using If = BlockInstr<Opcode::If>;
using Else = Atomic<Opcode::Else>;
using Br = OneIndex<Opcode::Br>;
using Br_if = OneIndex<Opcode::Br_if>;
struct Br_table : public Base {
    Br_table() : Base(Opcode::Br_table) {}
    std::vector<index_t> indices;
};
using Return = Atomic<Opcode::Return>;
using Throw = OneIndex<Opcode::Throw>;
using Throw_ref = Atomic<Opcode::Throw_ref>;
// try_table catch entry (unresolved, module-level tagidx)
struct TryCatchEntry {
    enum Kind { Catch = 0, CatchRef = 1, CatchAll = 2, CatchAllRef = 3 } kind;
    index_t tag_idx;    // only for Catch/CatchRef
    index_t label_idx;
};
struct Try_table : public Base {
    Try_table() : Base(Opcode::Try_table) {}
    std::optional<index_t> type;
    std::vector<TryCatchEntry> catches;
};
struct Call_indirect : public Base {
    Call_indirect() : Base(Opcode::Call_indirect) {}
    Call_indirect(index_t tableidx, index_t typeidx) : Base(Opcode::Call_indirect), tableidx(tableidx), typeidx(typeidx) {}
    index_t tableidx;
    index_t typeidx;
};
using Return_call = OneIndex<Opcode::Return_call>;
struct Return_call_indirect : public Base {
    Return_call_indirect() : Base(Opcode::Return_call_indirect), tableidx(0), typeidx(0) {}
    Return_call_indirect(index_t tableidx, index_t typeidx) : Base(Opcode::Return_call_indirect), tableidx(tableidx), typeidx(typeidx) {}
    index_t tableidx;
    index_t typeidx;
};
using Return_call_ref = OneIndex<Opcode::Return_call_ref>;

// Reference instructions
struct Ref_null : public Base {
    Ref_null() : Base(Opcode::Ref_null), heaptype(RefType::funcref) {}
    Ref_null(RefType heaptype) : Base(Opcode::Ref_null), heaptype(heaptype) {}
    RefType heaptype;
};
using Ref_is_null = Atomic<Opcode::Ref_is_null>;
using Ref_func = OneIndex<Opcode::Ref_func>;
using Ref_eq = Atomic<Opcode::Ref_eq>;
using Ref_as_non_null = Atomic<Opcode::Ref_as_non_null>;
using Br_on_null = OneIndex<Opcode::Br_on_null>;
using Br_on_non_null = OneIndex<Opcode::Br_on_non_null>;
// castop: flags (bit0=src nullable, bit1=dst nullable), two heaptypes
struct Br_on_cast : public Base {
    Br_on_cast() : Base(Opcode::Br_on_cast), labelidx(0), src_nullable(false), dst_nullable(false), src_heaptype(0), dst_heaptype(0) {}
    Br_on_cast(index_t labelidx, bool src_nullable, bool dst_nullable, int32_t src_heaptype, int32_t dst_heaptype)
        : Base(Opcode::Br_on_cast), labelidx(labelidx), src_nullable(src_nullable), dst_nullable(dst_nullable), src_heaptype(src_heaptype), dst_heaptype(dst_heaptype) {}
    index_t labelidx;
    bool src_nullable;
    bool dst_nullable;
    int32_t src_heaptype;
    int32_t dst_heaptype;
};
struct Br_on_cast_fail : public Base {
    Br_on_cast_fail() : Base(Opcode::Br_on_cast_fail), labelidx(0), src_nullable(false), dst_nullable(false), src_heaptype(0), dst_heaptype(0) {}
    Br_on_cast_fail(index_t labelidx, bool src_nullable, bool dst_nullable, int32_t src_heaptype, int32_t dst_heaptype)
        : Base(Opcode::Br_on_cast_fail), labelidx(labelidx), src_nullable(src_nullable), dst_nullable(dst_nullable), src_heaptype(src_heaptype), dst_heaptype(dst_heaptype) {}
    index_t labelidx;
    bool src_nullable;
    bool dst_nullable;
    int32_t src_heaptype;
    int32_t dst_heaptype;
};
using Call_ref = OneIndex<Opcode::Call_ref>;
// heaptype: negative = abstract heap type (e.g. -16=func, -17=extern), non-negative = type index
struct Ref_test : public Base {
    Ref_test() : Base(Opcode::Ref_test), heaptype(0) {}
    Ref_test(int32_t heaptype) : Base(Opcode::Ref_test), heaptype(heaptype) {}
    int32_t heaptype;
};
struct Ref_test_null : public Base {
    Ref_test_null() : Base(Opcode::Ref_test_null), heaptype(0) {}
    Ref_test_null(int32_t heaptype) : Base(Opcode::Ref_test_null), heaptype(heaptype) {}
    int32_t heaptype;
};
struct Ref_cast : public Base {
    Ref_cast() : Base(Opcode::Ref_cast), heaptype(0) {}
    Ref_cast(int32_t heaptype) : Base(Opcode::Ref_cast), heaptype(heaptype) {}
    int32_t heaptype;
};
struct Ref_cast_null : public Base {
    Ref_cast_null() : Base(Opcode::Ref_cast_null), heaptype(0) {}
    Ref_cast_null(int32_t heaptype) : Base(Opcode::Ref_cast_null), heaptype(heaptype) {}
    int32_t heaptype;
};
using Ref_i31 = Atomic<Opcode::Ref_i31>;
using I31_get_s = Atomic<Opcode::I31_get_s>;
using I31_get_u = Atomic<Opcode::I31_get_u>;
using Any_convert_extern = Atomic<Opcode::Any_convert_extern>;
using Extern_convert_any = Atomic<Opcode::Extern_convert_any>;

// GC Struct operations
using Struct_new = OneIndex<Opcode::Struct_new>;
using Struct_new_default = OneIndex<Opcode::Struct_new_default>;
struct Struct_get : public Base {
    Struct_get() : Base(Opcode::Struct_get), typeidx(0), fieldidx(0) {}
    Struct_get(index_t typeidx, index_t fieldidx) : Base(Opcode::Struct_get), typeidx(typeidx), fieldidx(fieldidx) {}
    index_t typeidx;
    index_t fieldidx;
};
struct Struct_get_s : public Base {
    Struct_get_s() : Base(Opcode::Struct_get_s), typeidx(0), fieldidx(0) {}
    Struct_get_s(index_t typeidx, index_t fieldidx) : Base(Opcode::Struct_get_s), typeidx(typeidx), fieldidx(fieldidx) {}
    index_t typeidx;
    index_t fieldidx;
};
struct Struct_get_u : public Base {
    Struct_get_u() : Base(Opcode::Struct_get_u), typeidx(0), fieldidx(0) {}
    Struct_get_u(index_t typeidx, index_t fieldidx) : Base(Opcode::Struct_get_u), typeidx(typeidx), fieldidx(fieldidx) {}
    index_t typeidx;
    index_t fieldidx;
};
struct Struct_set : public Base {
    Struct_set() : Base(Opcode::Struct_set), typeidx(0), fieldidx(0) {}
    Struct_set(index_t typeidx, index_t fieldidx) : Base(Opcode::Struct_set), typeidx(typeidx), fieldidx(fieldidx) {}
    index_t typeidx;
    index_t fieldidx;
};
// GC Array operations
using Array_new = OneIndex<Opcode::Array_new>;
using Array_new_default = OneIndex<Opcode::Array_new_default>;
struct Array_new_fixed : public Base {
    Array_new_fixed() : Base(Opcode::Array_new_fixed), typeidx(0), n(0) {}
    Array_new_fixed(index_t typeidx, u32_t n) : Base(Opcode::Array_new_fixed), typeidx(typeidx), n(n) {}
    index_t typeidx;
    u32_t n;
};
struct Array_new_data : public Base {
    Array_new_data() : Base(Opcode::Array_new_data), typeidx(0), dataidx(0) {}
    Array_new_data(index_t typeidx, index_t dataidx) : Base(Opcode::Array_new_data), typeidx(typeidx), dataidx(dataidx) {}
    index_t typeidx;
    index_t dataidx;
};
struct Array_new_elem : public Base {
    Array_new_elem() : Base(Opcode::Array_new_elem), typeidx(0), elemidx(0) {}
    Array_new_elem(index_t typeidx, index_t elemidx) : Base(Opcode::Array_new_elem), typeidx(typeidx), elemidx(elemidx) {}
    index_t typeidx;
    index_t elemidx;
};
using Array_get = OneIndex<Opcode::Array_get>;
using Array_get_s = OneIndex<Opcode::Array_get_s>;
using Array_get_u = OneIndex<Opcode::Array_get_u>;
using Array_set = OneIndex<Opcode::Array_set>;
using Array_len = Atomic<Opcode::Array_len>;
struct Array_fill : public Base {
    Array_fill() : Base(Opcode::Array_fill), typeidx(0) {}
    Array_fill(index_t typeidx) : Base(Opcode::Array_fill), typeidx(typeidx) {}
    index_t typeidx;
};
struct Array_copy : public Base {
    Array_copy() : Base(Opcode::Array_copy), dst_typeidx(0), src_typeidx(0) {}
    Array_copy(index_t dst, index_t src) : Base(Opcode::Array_copy), dst_typeidx(dst), src_typeidx(src) {}
    index_t dst_typeidx;
    index_t src_typeidx;
};
struct Array_init_data : public Base {
    Array_init_data() : Base(Opcode::Array_init_data), typeidx(0), dataidx(0) {}
    Array_init_data(index_t typeidx, index_t dataidx) : Base(Opcode::Array_init_data), typeidx(typeidx), dataidx(dataidx) {}
    index_t typeidx;
    index_t dataidx;
};
struct Array_init_elem : public Base {
    Array_init_elem() : Base(Opcode::Array_init_elem), typeidx(0), elemidx(0) {}
    Array_init_elem(index_t typeidx, index_t elemidx) : Base(Opcode::Array_init_elem), typeidx(typeidx), elemidx(elemidx) {}
    index_t typeidx;
    index_t elemidx;
};

// Parametric instructions
using Drop = Atomic<Opcode::Drop>;
struct Select : public Base {
    Select() : Base(Opcode::Select) {}
    std::vector<ValueType> valtypes;
};

// Variable instructions
using Local_get = OneIndex<Opcode::Local_get>;
using Local_set = OneIndex<Opcode::Local_set>;
using Local_tee = OneIndex<Opcode::Local_tee>;
using Global_get = OneIndex<Opcode::Global_get>;
using Global_set = OneIndex<Opcode::Global_set>;

// Table instructions
using Table_get = OneIndex<Opcode::Table_get>;
using Table_set = OneIndex<Opcode::Table_set>;
using Table_size = OneIndex<Opcode::Table_size>;
using Table_grow = OneIndex<Opcode::Table_grow>;
using Table_fill = OneIndex<Opcode::Table_fill>;
struct Table_copy : public Base {
    Table_copy(): Base(Opcode::Table_copy), dstidx(0), srcidx(0) {}
    Table_copy(index_t dstidx, index_t srcidx): Base(Opcode::Table_copy), dstidx(dstidx), srcidx(srcidx) {}
    index_t dstidx;
    index_t srcidx;
};
struct Table_init : public Base {
    Table_init(): Base(Opcode::Table_init), tableidx(0), elemidx(0) {}
    Table_init(index_t tableidx, index_t elemidx): Base(Opcode::Table_init), tableidx(tableidx), elemidx(elemidx) {}
    index_t tableidx;
    index_t elemidx;
};
using Elem_drop = OneIndex<Opcode::Elem_drop>;

// Memory Instruction
namespace MemoryInstr {

struct Base : public Instr::Base {
    Base(Opcode::Opcode op, index_t memidx, offset_t offset, align_t align): Instr::Base(op), memidx(memidx), offset(offset), align(align){}
    index_t memidx;
    offset_t offset;
    align_t align;
};

template<Opcode::Opcode OP>
struct Class : public Base {
    Class(): Base(OP, 0, 0, 0){}
    Class(index_t memidx, offset_t offset, align_t align): Base(OP, memidx, offset, align){}
};

}

using I32_load = MemoryInstr::Class<Opcode::I32_load>;
using I64_load = MemoryInstr::Class<Opcode::I64_load>;
using F32_load = MemoryInstr::Class<Opcode::F32_load>;
using F64_load = MemoryInstr::Class<Opcode::F64_load>;
using I32_load8_s = MemoryInstr::Class<Opcode::I32_load8_s>;
using I32_load8_u = MemoryInstr::Class<Opcode::I32_load8_u>;
using I32_load16_s = MemoryInstr::Class<Opcode::I32_load16_s>;
using I32_load16_u = MemoryInstr::Class<Opcode::I32_load16_u>;
using I64_load8_s = MemoryInstr::Class<Opcode::I64_load8_s>;
using I64_load8_u = MemoryInstr::Class<Opcode::I64_load8_u>;
using I64_load16_s = MemoryInstr::Class<Opcode::I64_load16_s>;
using I64_load16_u = MemoryInstr::Class<Opcode::I64_load16_u>;
using I64_load32_s = MemoryInstr::Class<Opcode::I64_load32_s>;
using I64_load32_u = MemoryInstr::Class<Opcode::I64_load32_u>;
using I32_store = MemoryInstr::Class<Opcode::I32_store>;
using I64_store = MemoryInstr::Class<Opcode::I64_store>;
using F32_store = MemoryInstr::Class<Opcode::F32_store>;
using F64_store = MemoryInstr::Class<Opcode::F64_store>;
using I32_store8 = MemoryInstr::Class<Opcode::I32_store8>;
using I32_store16 = MemoryInstr::Class<Opcode::I32_store16>;
using I64_store8 = MemoryInstr::Class<Opcode::I64_store8>;
using I64_store16 = MemoryInstr::Class<Opcode::I64_store16>;
using I64_store32 = MemoryInstr::Class<Opcode::I64_store32>;
using Memory_size = OneIndex<Opcode::Memory_size>;
using Memory_grow = OneIndex<Opcode::Memory_grow>;
using Memory_fill = OneIndex<Opcode::Memory_fill>;
struct Memory_init : public Base {
    Memory_init(): Base(Opcode::Memory_init), memidx(0), dataidx(0) {}
    Memory_init(index_t memidx, index_t dataidx): Base(Opcode::Memory_init), memidx(memidx), dataidx(dataidx) {}
    index_t memidx;
    index_t dataidx;
};
struct Memory_copy : public Base {
    Memory_copy(): Base(Opcode::Memory_copy), dstidx(0), srcidx(0) {}
    Memory_copy(index_t dstidx, index_t srcidx): Base(Opcode::Memory_copy), dstidx(dstidx), srcidx(srcidx) {}
    index_t dstidx;
    index_t srcidx;
};
using Data_drop = OneIndex<Opcode::Data_drop>;

// Numeric
struct I32_const : public Base {
    I32_const(): Base(Opcode::I32_const), value(0) {}
    I32_const(i32_t value): Base(Opcode::I32_const), value(value) {}
    i32_t value;
};
struct I64_const : public Base {
    I64_const(): Base(Opcode::I64_const), value(0) {}
    I64_const(i64_t value): Base(Opcode::I64_const), value(value) {}
    i64_t value;
};
struct F32_const : public Base {
    F32_const(): Base(Opcode::F32_const), value(0) {}
    F32_const(f32_t value): Base(Opcode::F32_const), value(value) {}
    f32_t value;
};
struct F64_const : public Base {
    F64_const(): Base(Opcode::F64_const), value(0) {}
    F64_const(f64_t value): Base(Opcode::F64_const), value(value) {}
    f64_t value;
};
using I32_eqz = Atomic<Opcode::I32_eqz>;
using I32_eq = Atomic<Opcode::I32_eq>;
using I32_ne = Atomic<Opcode::I32_ne>;
using I32_lt_s = Atomic<Opcode::I32_lt_s>;
using I32_lt_u = Atomic<Opcode::I32_lt_u>;
using I32_gt_s = Atomic<Opcode::I32_gt_s>;
using I32_gt_u = Atomic<Opcode::I32_gt_u>;
using I32_le_s = Atomic<Opcode::I32_le_s>;
using I32_le_u = Atomic<Opcode::I32_le_u>;
using I32_ge_s = Atomic<Opcode::I32_ge_s>;
using I32_ge_u = Atomic<Opcode::I32_ge_u>;
using I64_eqz = Atomic<Opcode::I64_eqz>;
using I64_eq = Atomic<Opcode::I64_eq>;
using I64_ne = Atomic<Opcode::I64_ne>;
using I64_lt_s = Atomic<Opcode::I64_lt_s>;
using I64_lt_u = Atomic<Opcode::I64_lt_u>;
using I64_gt_s = Atomic<Opcode::I64_gt_s>;
using I64_gt_u = Atomic<Opcode::I64_gt_u>;
using I64_le_s = Atomic<Opcode::I64_le_s>;
using I64_le_u = Atomic<Opcode::I64_le_u>;
using I64_ge_s = Atomic<Opcode::I64_ge_s>;
using I64_ge_u = Atomic<Opcode::I64_ge_u>;
using F32_eq = Atomic<Opcode::F32_eq>;
using F32_ne = Atomic<Opcode::F32_ne>;
using F32_lt = Atomic<Opcode::F32_lt>;
using F32_gt = Atomic<Opcode::F32_gt>;
using F32_le = Atomic<Opcode::F32_le>;
using F32_ge = Atomic<Opcode::F32_ge>;
using F64_eq = Atomic<Opcode::F64_eq>;
using F64_ne = Atomic<Opcode::F64_ne>;
using F64_lt = Atomic<Opcode::F64_lt>;
using F64_gt = Atomic<Opcode::F64_gt>;
using F64_le = Atomic<Opcode::F64_le>;
using F64_ge = Atomic<Opcode::F64_ge>;
using I32_clz = Atomic<Opcode::I32_clz>;
using I32_ctz = Atomic<Opcode::I32_ctz>;
using I32_popcnt = Atomic<Opcode::I32_popcnt>;
using I32_add = Atomic<Opcode::I32_add>;
using I32_sub = Atomic<Opcode::I32_sub>;
using I32_mul = Atomic<Opcode::I32_mul>;
using I32_div_s = Atomic<Opcode::I32_div_s>;
using I32_div_u = Atomic<Opcode::I32_div_u>;
using I32_rem_s = Atomic<Opcode::I32_rem_s>;
using I32_rem_u = Atomic<Opcode::I32_rem_u>;
using I32_and = Atomic<Opcode::I32_and>;
using I32_or = Atomic<Opcode::I32_or>;
using I32_xor = Atomic<Opcode::I32_xor>;
using I32_shl = Atomic<Opcode::I32_shl>;
using I32_shr_s = Atomic<Opcode::I32_shr_s>;
using I32_shr_u = Atomic<Opcode::I32_shr_u>;
using I32_rotl = Atomic<Opcode::I32_rotl>;
using I32_rotr = Atomic<Opcode::I32_rotr>;
using I64_clz = Atomic<Opcode::I64_clz>;
using I64_ctz = Atomic<Opcode::I64_ctz>;
using I64_popcnt = Atomic<Opcode::I64_popcnt>;
using I64_add = Atomic<Opcode::I64_add>;
using I64_sub = Atomic<Opcode::I64_sub>;
using I64_mul = Atomic<Opcode::I64_mul>;
using I64_div_s = Atomic<Opcode::I64_div_s>;
using I64_div_u = Atomic<Opcode::I64_div_u>;
using I64_rem_s = Atomic<Opcode::I64_rem_s>;
using I64_rem_u = Atomic<Opcode::I64_rem_u>;
using I64_and = Atomic<Opcode::I64_and>;
using I64_or = Atomic<Opcode::I64_or>;
using I64_xor = Atomic<Opcode::I64_xor>;
using I64_shl = Atomic<Opcode::I64_shl>;
using I64_shr_s = Atomic<Opcode::I64_shr_s>;
using I64_shr_u = Atomic<Opcode::I64_shr_u>;
using I64_rotl = Atomic<Opcode::I64_rotl>;
using I64_rotr = Atomic<Opcode::I64_rotr>;
using F32_abs = Atomic<Opcode::F32_abs>;
using F32_neg = Atomic<Opcode::F32_neg>;
using F32_ceil = Atomic<Opcode::F32_ceil>;
using F32_floor = Atomic<Opcode::F32_floor>;
using F32_trunc = Atomic<Opcode::F32_trunc>;
using F32_nearest = Atomic<Opcode::F32_nearest>;
using F32_sqrt = Atomic<Opcode::F32_sqrt>;
using F32_add = Atomic<Opcode::F32_add>;
using F32_sub = Atomic<Opcode::F32_sub>;
using F32_mul = Atomic<Opcode::F32_mul>;
using F32_div = Atomic<Opcode::F32_div>;
using F32_min = Atomic<Opcode::F32_min>;
using F32_max = Atomic<Opcode::F32_max>;
using F32_copysign = Atomic<Opcode::F32_copysign>;
using F64_abs = Atomic<Opcode::F64_abs>;
using F64_neg = Atomic<Opcode::F64_neg>;
using F64_ceil = Atomic<Opcode::F64_ceil>;
using F64_floor = Atomic<Opcode::F64_floor>;
using F64_trunc = Atomic<Opcode::F64_trunc>;
using F64_nearest = Atomic<Opcode::F64_nearest>;
using F64_sqrt = Atomic<Opcode::F64_sqrt>;
using F64_add = Atomic<Opcode::F64_add>;
using F64_sub = Atomic<Opcode::F64_sub>;
using F64_mul = Atomic<Opcode::F64_mul>;
using F64_div = Atomic<Opcode::F64_div>;
using F64_min = Atomic<Opcode::F64_min>;
using F64_max = Atomic<Opcode::F64_max>;
using F64_copysign = Atomic<Opcode::F64_copysign>;
using I32_wrap_i64 = Atomic<Opcode::I32_wrap_i64>;
using I32_trunc_f32_s = Atomic<Opcode::I32_trunc_f32_s>;
using I32_trunc_f32_u = Atomic<Opcode::I32_trunc_f32_u>;
using I32_trunc_f64_s = Atomic<Opcode::I32_trunc_f64_s>;
using I32_trunc_f64_u = Atomic<Opcode::I32_trunc_f64_u>;
using I64_extend_i32_s = Atomic<Opcode::I64_extend_i32_s>;
using I64_extend_i32_u = Atomic<Opcode::I64_extend_i32_u>;
using I64_trunc_f32_s = Atomic<Opcode::I64_trunc_f32_s>;
using I64_trunc_f32_u = Atomic<Opcode::I64_trunc_f32_u>;
using I64_trunc_f64_s = Atomic<Opcode::I64_trunc_f64_s>;
using I64_trunc_f64_u = Atomic<Opcode::I64_trunc_f64_u>;
using F32_convert_i32_s = Atomic<Opcode::F32_convert_i32_s>;
using F32_convert_i32_u = Atomic<Opcode::F32_convert_i32_u>;
using F32_convert_i64_s = Atomic<Opcode::F32_convert_i64_s>;
using F32_convert_i64_u = Atomic<Opcode::F32_convert_i64_u>;
using F32_demote_f64 = Atomic<Opcode::F32_demote_f64>;
using F64_convert_i32_s = Atomic<Opcode::F64_convert_i32_s>;
using F64_convert_i32_u = Atomic<Opcode::F64_convert_i32_u>;
using F64_convert_i64_s = Atomic<Opcode::F64_convert_i64_s>;
using F64_convert_i64_u = Atomic<Opcode::F64_convert_i64_u>;
using F64_promote_f32 = Atomic<Opcode::F64_promote_f32>;
using I32_reinterpret_f32 = Atomic<Opcode::I32_reinterpret_f32>;
using I64_reinterpret_f64 = Atomic<Opcode::I64_reinterpret_f64>;
using F32_reinterpret_i32 = Atomic<Opcode::F32_reinterpret_i32>;
using F64_reinterpret_i64 = Atomic<Opcode::F64_reinterpret_i64>;
using I32_extend8_s = Atomic<Opcode::I32_extend8_s>;
using I32_extend16_s = Atomic<Opcode::I32_extend16_s>;
using I64_extend8_s = Atomic<Opcode::I64_extend8_s>;
using I64_extend16_s = Atomic<Opcode::I64_extend16_s>;
using I64_extend32_s = Atomic<Opcode::I64_extend32_s>;
using I32_trunc_sat_f32_s = Atomic<Opcode::I32_trunc_sat_f32_s>;
using I32_trunc_sat_f32_u = Atomic<Opcode::I32_trunc_sat_f32_u>;
using I32_trunc_sat_f64_s = Atomic<Opcode::I32_trunc_sat_f64_s>;
using I32_trunc_sat_f64_u = Atomic<Opcode::I32_trunc_sat_f64_u>;
using I64_trunc_sat_f32_s = Atomic<Opcode::I64_trunc_sat_f32_s>;
using I64_trunc_sat_f32_u = Atomic<Opcode::I64_trunc_sat_f32_u>;
using I64_trunc_sat_f64_s = Atomic<Opcode::I64_trunc_sat_f64_s>;
using I64_trunc_sat_f64_u = Atomic<Opcode::I64_trunc_sat_f64_u>;

}

// WasmInstr: flat struct with opcode + 15-type immediate variant.
// Reduces template instantiation from ~165 to 15 alternatives.
struct WasmInstr {
    Opcode::opcode_t opcode;

    // Immediate operand shapes
    struct NoImm    {};
    struct OneIdx   { index_t index; };
    struct TwoIdx   { index_t a, b; };
    struct BlockType{ std::optional<index_t> type; };
    struct MemArg   { index_t memidx; offset_t offset; align_t align; };
    struct ConstI32 { i32_t value; };
    struct ConstI64 { i64_t value; };
    struct ConstF32 { f32_t value; };
    struct ConstF64 { f64_t value; };
    struct HeapRef  { RefType heaptype; };
    struct HeapI32  { int32_t heaptype; };
    struct CastOp   { index_t label; bool src_n, dst_n; int32_t src_ht, dst_ht; };
    struct BrTab    { std::vector<index_t> indices; };
    struct TryTab   { std::optional<index_t> type; std::vector<Instr::TryCatchEntry> catches; };
    struct SelectV  { std::vector<ValueType> valtypes; };

    using Imm = std::variant<
        NoImm, OneIdx, TwoIdx, BlockType, MemArg,
        ConstI32, ConstI64, ConstF32, ConstF64,
        HeapRef, HeapI32, CastOp, BrTab, TryTab, SelectV
    >;
    Imm imm;

    // Default constructor — opcode is set to End as a safe sentinel
    WasmInstr() : opcode(Opcode::End), imm(NoImm{}) {}

    // Convenience constructors from Instr::* types

    // --- Atomic (NoImm) ---
    template<Opcode::Opcode OP>
    WasmInstr(Instr::Atomic<OP>) : opcode(OP), imm(NoImm{}) {}

    // --- OneIndex (OneIdx) ---
    template<Opcode::Opcode OP>
    WasmInstr(Instr::OneIndex<OP> i) : opcode(OP), imm(OneIdx{i.index}) {}

    // --- BlockInstr (BlockType) ---
    template<Opcode::Opcode OP>
    WasmInstr(Instr::BlockInstr<OP> b) : opcode(OP), imm(BlockType{b.type}) {}

    // --- MemoryInstr::Class (MemArg) ---
    template<Opcode::Opcode OP>
    WasmInstr(Instr::MemoryInstr::Class<OP> m)
        : opcode(OP), imm(MemArg{m.memidx, m.offset, m.align}) {}

    // --- Control: special structs ---
    WasmInstr(Instr::Br_table i)
        : opcode(Opcode::Br_table), imm(BrTab{i.indices}) {}
    WasmInstr(Instr::Try_table i)
        : opcode(Opcode::Try_table), imm(TryTab{i.type, i.catches}) {}
    WasmInstr(Instr::Call_indirect i)
        : opcode(Opcode::Call_indirect), imm(TwoIdx{i.tableidx, i.typeidx}) {}
    WasmInstr(Instr::Return_call_indirect i)
        : opcode(Opcode::Return_call_indirect), imm(TwoIdx{i.tableidx, i.typeidx}) {}

    // --- Reference: special structs ---
    WasmInstr(Instr::Ref_null i)
        : opcode(Opcode::Ref_null), imm(HeapRef{i.heaptype}) {}
    WasmInstr(Instr::Br_on_cast i)
        : opcode(Opcode::Br_on_cast), imm(CastOp{i.labelidx, i.src_nullable, i.dst_nullable, i.src_heaptype, i.dst_heaptype}) {}
    WasmInstr(Instr::Br_on_cast_fail i)
        : opcode(Opcode::Br_on_cast_fail), imm(CastOp{i.labelidx, i.src_nullable, i.dst_nullable, i.src_heaptype, i.dst_heaptype}) {}
    WasmInstr(Instr::Ref_test i)
        : opcode(Opcode::Ref_test), imm(HeapI32{i.heaptype}) {}
    WasmInstr(Instr::Ref_test_null i)
        : opcode(Opcode::Ref_test_null), imm(HeapI32{i.heaptype}) {}
    WasmInstr(Instr::Ref_cast i)
        : opcode(Opcode::Ref_cast), imm(HeapI32{i.heaptype}) {}
    WasmInstr(Instr::Ref_cast_null i)
        : opcode(Opcode::Ref_cast_null), imm(HeapI32{i.heaptype}) {}

    // --- GC Struct: two-index ---
    WasmInstr(Instr::Struct_get i)
        : opcode(Opcode::Struct_get), imm(TwoIdx{i.typeidx, i.fieldidx}) {}
    WasmInstr(Instr::Struct_get_s i)
        : opcode(Opcode::Struct_get_s), imm(TwoIdx{i.typeidx, i.fieldidx}) {}
    WasmInstr(Instr::Struct_get_u i)
        : opcode(Opcode::Struct_get_u), imm(TwoIdx{i.typeidx, i.fieldidx}) {}
    WasmInstr(Instr::Struct_set i)
        : opcode(Opcode::Struct_set), imm(TwoIdx{i.typeidx, i.fieldidx}) {}

    // --- GC Array: two-index or special ---
    WasmInstr(Instr::Array_new_fixed i)
        : opcode(Opcode::Array_new_fixed), imm(TwoIdx{i.typeidx, i.n}) {}
    WasmInstr(Instr::Array_new_data i)
        : opcode(Opcode::Array_new_data), imm(TwoIdx{i.typeidx, i.dataidx}) {}
    WasmInstr(Instr::Array_new_elem i)
        : opcode(Opcode::Array_new_elem), imm(TwoIdx{i.typeidx, i.elemidx}) {}
    WasmInstr(Instr::Array_fill i)
        : opcode(Opcode::Array_fill), imm(OneIdx{i.typeidx}) {}
    WasmInstr(Instr::Array_copy i)
        : opcode(Opcode::Array_copy), imm(TwoIdx{i.dst_typeidx, i.src_typeidx}) {}
    WasmInstr(Instr::Array_init_data i)
        : opcode(Opcode::Array_init_data), imm(TwoIdx{i.typeidx, i.dataidx}) {}
    WasmInstr(Instr::Array_init_elem i)
        : opcode(Opcode::Array_init_elem), imm(TwoIdx{i.typeidx, i.elemidx}) {}

    // --- Parametric ---
    WasmInstr(Instr::Select i)
        : opcode(Opcode::Select), imm(SelectV{i.valtypes}) {}

    // --- Memory: two-index ---
    WasmInstr(Instr::Memory_init i)
        : opcode(Opcode::Memory_init), imm(TwoIdx{i.memidx, i.dataidx}) {}
    WasmInstr(Instr::Memory_copy i)
        : opcode(Opcode::Memory_copy), imm(TwoIdx{i.dstidx, i.srcidx}) {}

    // --- Table: two-index ---
    WasmInstr(Instr::Table_copy i)
        : opcode(Opcode::Table_copy), imm(TwoIdx{i.dstidx, i.srcidx}) {}
    WasmInstr(Instr::Table_init i)
        : opcode(Opcode::Table_init), imm(TwoIdx{i.tableidx, i.elemidx}) {}

    // --- Numeric: const ---
    WasmInstr(Instr::I32_const i) : opcode(Opcode::I32_const), imm(ConstI32{i.value}) {}
    WasmInstr(Instr::I64_const i) : opcode(Opcode::I64_const), imm(ConstI64{i.value}) {}
    WasmInstr(Instr::F32_const i) : opcode(Opcode::F32_const), imm(ConstF32{i.value}) {}
    WasmInstr(Instr::F64_const i) : opcode(Opcode::F64_const), imm(ConstF64{i.value}) {}
};

using ConstInstr = std::variant<
    Instr::Ref_null,
    Instr::Ref_func,
    Instr::Global_get,
    Instr::I32_const,
    Instr::I64_const,
    Instr::F32_const,
    Instr::F64_const
>;

}

#endif