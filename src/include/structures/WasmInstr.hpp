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
    Instr::F64_const
>;

}

#endif