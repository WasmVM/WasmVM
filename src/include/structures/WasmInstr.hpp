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
    BlockInstr(index_t type): Base(OP), type(type){}
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
struct Call_indirect : public Base {
    Call_indirect() : Base(Opcode::Call_indirect) {}
    Call_indirect(index_t tableidx, index_t typeidx) : Base(Opcode::Call_indirect), tableidx(tableidx), typeidx(typeidx) {}
    index_t tableidx;
    index_t typeidx;
};

// Reference instructions
struct Ref_null : public Base {
    Ref_null() : Base(Opcode::Ref_null), heaptype(RefType::funcref) {}
    Ref_null(RefType heaptype) : Base(Opcode::Ref_null), heaptype(heaptype) {}
    RefType heaptype;
};
using Ref_is_null = Atomic<Opcode::Ref_is_null>;
using Ref_func = OneIndex<Opcode::Ref_func>;

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
using I32_trunc_s_f32 = Atomic<Opcode::I32_trunc_s_f32>;
using I32_trunc_u_f32 = Atomic<Opcode::I32_trunc_u_f32>;
using I32_trunc_s_f64 = Atomic<Opcode::I32_trunc_s_f64>;
using I32_trunc_u_f64 = Atomic<Opcode::I32_trunc_u_f64>;
using I64_extend_s_i32 = Atomic<Opcode::I64_extend_s_i32>;
using I64_extend_u_i32 = Atomic<Opcode::I64_extend_u_i32>;
using I64_trunc_s_f32 = Atomic<Opcode::I64_trunc_s_f32>;
using I64_trunc_u_f32 = Atomic<Opcode::I64_trunc_u_f32>;
using I64_trunc_s_f64 = Atomic<Opcode::I64_trunc_s_f64>;
using I64_trunc_u_f64 = Atomic<Opcode::I64_trunc_u_f64>;
using F32_convert_s_i32 = Atomic<Opcode::F32_convert_s_i32>;
using F32_convert_u_i32 = Atomic<Opcode::F32_convert_u_i32>;
using F32_convert_s_i64 = Atomic<Opcode::F32_convert_s_i64>;
using F32_convert_u_i64 = Atomic<Opcode::F32_convert_u_i64>;
using F32_demote_f64 = Atomic<Opcode::F32_demote_f64>;
using F64_convert_s_i32 = Atomic<Opcode::F64_convert_s_i32>;
using F64_convert_u_i32 = Atomic<Opcode::F64_convert_u_i32>;
using F64_convert_s_i64 = Atomic<Opcode::F64_convert_s_i64>;
using F64_convert_u_i64 = Atomic<Opcode::F64_convert_u_i64>;
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

using WasmInstr = std::variant<
    // Control
    Instr::Unreachable,
    Instr::Nop,
    Instr::Block,
    Instr::Loop,
    Instr::If,
    Instr::Else,
    Instr::End,
    Instr::Br,
    Instr::Br_if,
    Instr::Br_table,
    Instr::Return,
    Instr::Call,
    Instr::Call_indirect,
    // Reference
    Instr::Ref_null,
    Instr::Ref_is_null,
    Instr::Ref_func,
    // Parametric
    Instr::Drop,
    Instr::Select,
    // Variable
    Instr::Local_get,
    Instr::Local_set,
    Instr::Local_tee,
    Instr::Global_get,
    Instr::Global_set,
    // Table
    Instr::Table_get,
    Instr::Table_set,
    Instr::Table_size,
    Instr::Table_grow,
    Instr::Table_fill,
    Instr::Table_copy,
    Instr::Table_init,
    Instr::Elem_drop,
    // Memory
    Instr::I32_load,
    Instr::I64_load,
    Instr::F32_load,
    Instr::F64_load,
    Instr::I32_load8_s,
    Instr::I32_load8_u,
    Instr::I32_load16_s,
    Instr::I32_load16_u,
    Instr::I64_load8_s,
    Instr::I64_load8_u,
    Instr::I64_load16_s,
    Instr::I64_load16_u,
    Instr::I64_load32_s,
    Instr::I64_load32_u,
    Instr::I32_store,
    Instr::I64_store,
    Instr::F32_store,
    Instr::F64_store,
    Instr::I32_store8,
    Instr::I32_store16,
    Instr::I64_store8,
    Instr::I64_store16,
    Instr::I64_store32,
    Instr::Memory_size,
    Instr::Memory_grow,
    Instr::Memory_fill,
    Instr::Memory_init,
    Instr::Data_drop,
    Instr::Memory_copy,
    // Numeric
    Instr::I32_const,
    Instr::I64_const,
    Instr::F32_const,
    Instr::F64_const,
    Instr::I32_eqz,
    Instr::I32_eq,
    Instr::I32_ne,
    Instr::I32_lt_s,
    Instr::I32_lt_u,
    Instr::I32_gt_s,
    Instr::I32_gt_u,
    Instr::I32_le_s,
    Instr::I32_le_u,
    Instr::I32_ge_s,
    Instr::I32_ge_u,
    Instr::I64_eqz,
    Instr::I64_eq,
    Instr::I64_ne,
    Instr::I64_lt_s,
    Instr::I64_lt_u,
    Instr::I64_gt_s,
    Instr::I64_gt_u,
    Instr::I64_le_s,
    Instr::I64_le_u,
    Instr::I64_ge_s,
    Instr::I64_ge_u,
    Instr::F32_eq,
    Instr::F32_ne,
    Instr::F32_lt,
    Instr::F32_gt,
    Instr::F32_le,
    Instr::F32_ge,
    Instr::F64_eq,
    Instr::F64_ne,
    Instr::F64_lt,
    Instr::F64_gt,
    Instr::F64_le,
    Instr::F64_ge,
    Instr::I32_clz,
    Instr::I32_ctz,
    Instr::I32_popcnt,
    Instr::I32_add,
    Instr::I32_sub,
    Instr::I32_mul,
    Instr::I32_div_s,
    Instr::I32_div_u,
    Instr::I32_rem_s,
    Instr::I32_rem_u,
    Instr::I32_and,
    Instr::I32_or,
    Instr::I32_xor,
    Instr::I32_shl,
    Instr::I32_shr_s,
    Instr::I32_shr_u,
    Instr::I32_rotl,
    Instr::I32_rotr,
    Instr::I64_clz,
    Instr::I64_ctz,
    Instr::I64_popcnt,
    Instr::I64_add,
    Instr::I64_sub,
    Instr::I64_mul,
    Instr::I64_div_s,
    Instr::I64_div_u,
    Instr::I64_rem_s,
    Instr::I64_rem_u,
    Instr::I64_and,
    Instr::I64_or,
    Instr::I64_xor,
    Instr::I64_shl,
    Instr::I64_shr_s,
    Instr::I64_shr_u,
    Instr::I64_rotl,
    Instr::I64_rotr,
    Instr::F32_abs,
    Instr::F32_neg,
    Instr::F32_ceil,
    Instr::F32_floor,
    Instr::F32_trunc,
    Instr::F32_nearest,
    Instr::F32_sqrt,
    Instr::F32_add,
    Instr::F32_sub,
    Instr::F32_mul,
    Instr::F32_div,
    Instr::F32_min,
    Instr::F32_max,
    Instr::F32_copysign,
    Instr::F64_abs,
    Instr::F64_neg,
    Instr::F64_ceil,
    Instr::F64_floor,
    Instr::F64_trunc,
    Instr::F64_nearest,
    Instr::F64_sqrt,
    Instr::F64_add,
    Instr::F64_sub,
    Instr::F64_mul,
    Instr::F64_div,
    Instr::F64_min,
    Instr::F64_max,
    Instr::F64_copysign,
    Instr::I32_wrap_i64,
    Instr::I32_trunc_s_f32,
    Instr::I32_trunc_u_f32,
    Instr::I32_trunc_s_f64,
    Instr::I32_trunc_u_f64,
    Instr::I64_extend_s_i32,
    Instr::I64_extend_u_i32,
    Instr::I64_trunc_s_f32,
    Instr::I64_trunc_u_f32,
    Instr::I64_trunc_s_f64,
    Instr::I64_trunc_u_f64,
    Instr::F32_convert_s_i32,
    Instr::F32_convert_u_i32,
    Instr::F32_convert_s_i64,
    Instr::F32_convert_u_i64,
    Instr::F32_demote_f64,
    Instr::F64_convert_s_i32,
    Instr::F64_convert_u_i32,
    Instr::F64_convert_s_i64,
    Instr::F64_convert_u_i64,
    Instr::F64_promote_f32,
    Instr::I32_reinterpret_f32,
    Instr::I64_reinterpret_f64,
    Instr::F32_reinterpret_i32,
    Instr::F64_reinterpret_i64,
    Instr::I32_extend8_s,
    Instr::I32_extend16_s,
    Instr::I64_extend8_s,
    Instr::I64_extend16_s,
    Instr::I64_extend32_s,
    Instr::I32_trunc_sat_f32_s,
    Instr::I32_trunc_sat_f32_u,
    Instr::I32_trunc_sat_f64_s,
    Instr::I32_trunc_sat_f64_u,
    Instr::I64_trunc_sat_f32_s,
    Instr::I64_trunc_sat_f32_u,
    Instr::I64_trunc_sat_f64_s,
    Instr::I64_trunc_sat_f64_u
>;

}

#endif