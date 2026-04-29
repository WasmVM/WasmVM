// Copyright 2024 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <harness.hpp>
#include <WasmVM.hpp>
#include <exception.hpp>
#include <sstream>
#include <vector>
#include <string>

using namespace WasmVM;
using namespace Testing;

// Helpers: access WasmInstr immediates by category
static auto& as_one(const WasmInstr& i) { return std::get<WasmInstr::OneIdx>(i.imm); }
static auto& as_two(const WasmInstr& i) { return std::get<WasmInstr::TwoIdx>(i.imm); }
static auto& as_blk(const WasmInstr& i) { return std::get<WasmInstr::BlockType>(i.imm); }
static auto& as_mem(const WasmInstr& i) { return std::get<WasmInstr::MemArg>(i.imm); }
static auto& as_brt(const WasmInstr& i) { return std::get<WasmInstr::BrTab>(i.imm); }
static auto& as_try(const WasmInstr& i) { return std::get<WasmInstr::TryTab>(i.imm); }
static auto& as_href(const WasmInstr& i) { return std::get<WasmInstr::HeapRef>(i.imm); }
static auto& as_hi32(const WasmInstr& i) { return std::get<WasmInstr::HeapI32>(i.imm); }
static auto& as_cast(const WasmInstr& i) { return std::get<WasmInstr::CastOp>(i.imm); }
static auto& as_sel(const WasmInstr& i)  { return std::get<WasmInstr::SelectV>(i.imm); }
static auto& as_ci32(const WasmInstr& i) { return std::get<WasmInstr::ConstI32>(i.imm); }
static auto& as_ci64(const WasmInstr& i) { return std::get<WasmInstr::ConstI64>(i.imm); }
static auto& as_cf32(const WasmInstr& i) { return std::get<WasmInstr::ConstF32>(i.imm); }
static auto& as_cf64(const WasmInstr& i) { return std::get<WasmInstr::ConstF64>(i.imm); }

Suite instruction {
    Test("control", {
        ParseFile(test_module, "control.wat");
        std::vector<WasmInstr>& instrs = test_module.funcs[0].body;
        // unreachable
        Expect(instrs[0].opcode == Opcode::Unreachable);
        // nop
        Expect(instrs[1].opcode == Opcode::Nop);
        // return
        Expect(instrs[2].opcode == Opcode::Return);
        // br
        Expect(instrs[3].opcode == Opcode::Br);
        Expect(as_one(instrs[3]).index == 1);
        // br_if
        Expect(instrs[4].opcode == Opcode::Br_if);
        Expect(as_one(instrs[4]).index == 2);
        // br_table
        Expect(instrs[5].opcode == Opcode::Br_table);
        Expect(as_brt(instrs[5]).indices.size() == 3);
        Expect(as_brt(instrs[5]).indices[0] == 3);
        Expect(as_brt(instrs[5]).indices[1] == 4);
        Expect(as_brt(instrs[5]).indices[2] == 5);
        // call
        Expect(instrs[6].opcode == Opcode::Call);
        Expect(as_one(instrs[6]).index == 6);
        // call_indirect (tableidx=a, typeidx=b)
        Expect(instrs[7].opcode == Opcode::Call_indirect);
        Expect(as_two(instrs[7]).a == 7);  // tableidx
        Expect(as_two(instrs[7]).b == 8);  // typeidx
        Expect(instrs[8].opcode == Opcode::Call_indirect);
        Expect(as_two(instrs[8]).a == 0);  // tableidx
        Expect(as_two(instrs[8]).b == 9);  // typeidx
    })
    Test("block", {
        ParseFile(test_module, "block.wat");
        std::vector<WasmInstr>& instrs = test_module.funcs[0].body;
        // block with id
        Expect(instrs[0].opcode == Opcode::Block);
        Expect(as_blk(instrs[0]).type.has_value() == false);
        Expect(instrs[1].opcode == Opcode::Nop);
        Expect(instrs[2].opcode == Opcode::End);
        // block with result type
        Expect(instrs[3].opcode == Opcode::Block);
        Expect(as_blk(instrs[3]).type.value() == 1);
        Expect(instrs[4].opcode == Opcode::Nop);
        Expect(instrs[5].opcode == Opcode::End);
        // block with typeuse
        Expect(instrs[6].opcode == Opcode::Block);
        Expect(as_blk(instrs[6]).type.value() == 0);
        Expect(instrs[7].opcode == Opcode::Nop);
        Expect(instrs[8].opcode == Opcode::End);
        // loop
        Expect(instrs[9].opcode == Opcode::Loop);
        Expect(as_blk(instrs[9]).type.has_value() == false);
        Expect(instrs[10].opcode == Opcode::Nop);
        Expect(instrs[11].opcode == Opcode::End);
        // if then else
        Expect(instrs[12].opcode == Opcode::If);
        Expect(instrs[13].opcode == Opcode::Unreachable);
        Expect(instrs[14].opcode == Opcode::Else);
        Expect(instrs[15].opcode == Opcode::Nop);
        Expect(instrs[16].opcode == Opcode::End);
        // if else
        Expect(instrs[17].opcode == Opcode::If);
        Expect(instrs[18].opcode == Opcode::Unreachable);
        Expect(instrs[19].opcode == Opcode::Else);
        Expect(instrs[20].opcode == Opcode::End);
        // nested
        Expect(instrs[21].opcode == Opcode::Loop);
        Expect(instrs[22].opcode == Opcode::Block);
        Expect(instrs[23].opcode == Opcode::Unreachable);
        Expect(instrs[24].opcode == Opcode::End);
        Expect(instrs[25].opcode == Opcode::Block);
        Expect(instrs[26].opcode == Opcode::Nop);
        Expect(instrs[27].opcode == Opcode::End);
        Expect(instrs[28].opcode == Opcode::End);
    })
    Test("reference", {
        ParseFile(test_module, "reference.wat");
        std::vector<WasmInstr>& instrs = test_module.funcs[1].body;
        Expect(instrs[0].opcode == Opcode::Ref_null);
        Expect(as_href(instrs[0]).heaptype == RefType::funcref);
        Expect(instrs[1].opcode == Opcode::Ref_null);
        Expect(as_href(instrs[1]).heaptype == RefType::externref);
        Expect(instrs[2].opcode == Opcode::Ref_is_null);
        Expect(instrs[3].opcode == Opcode::Ref_func);
        Expect(as_one(instrs[3]).index == 0);
        Expect(instrs[4].opcode == Opcode::Ref_func);
        Expect(as_one(instrs[4]).index == 1);
        Expect(instrs[5].opcode == Opcode::Ref_eq);
        Expect(instrs[6].opcode == Opcode::Ref_as_non_null);
        Expect(instrs[7].opcode == Opcode::Ref_i31);
        Expect(instrs[8].opcode == Opcode::I31_get_s);
        Expect(instrs[9].opcode == Opcode::I31_get_u);
        // ref.test (ref func) -> Ref_test(-16)
        Expect(instrs[10].opcode == Opcode::Ref_test);
        Expect(as_hi32(instrs[10]).heaptype == -16);
        // ref.test (ref null extern) -> Ref_test_null(-17)
        Expect(instrs[11].opcode == Opcode::Ref_test_null);
        Expect(as_hi32(instrs[11]).heaptype == -17);
        // ref.cast (ref func) -> Ref_cast(-16)
        Expect(instrs[12].opcode == Opcode::Ref_cast);
        Expect(as_hi32(instrs[12]).heaptype == -16);
        // ref.cast (ref null extern) -> Ref_cast_null(-17)
        Expect(instrs[13].opcode == Opcode::Ref_cast_null);
        Expect(as_hi32(instrs[13]).heaptype == -17);
        Expect(instrs[14].opcode == Opcode::Any_convert_extern);
        Expect(instrs[15].opcode == Opcode::Extern_convert_any);
    })
    Test("typed_ref", {
        ParseFile(test_module, "typed_ref.wat");
        std::vector<WasmInstr>& instrs = test_module.funcs[0].body;
        // call_ref 0
        Expect(instrs[0].opcode == Opcode::Call_ref);
        Expect(as_one(instrs[0]).index == 0);
        // return_call_ref 0
        Expect(instrs[1].opcode == Opcode::Return_call_ref);
        Expect(as_one(instrs[1]).index == 0);
        // ref.null func; br_on_null 0
        Expect(instrs[3].opcode == Opcode::Br_on_null);
        Expect(as_one(instrs[3]).index == 0);
        // ref.null func; br_on_non_null 0
        Expect(instrs[5].opcode == Opcode::Br_on_non_null);
        Expect(as_one(instrs[5]).index == 0);
        // br_on_cast 0 (ref null func) (ref func)
        Expect(instrs[7].opcode == Opcode::Br_on_cast);
        Expect(as_cast(instrs[7]).label == 0);
        Expect(as_cast(instrs[7]).src_n == true);
        Expect(as_cast(instrs[7]).dst_n == false);
        Expect(as_cast(instrs[7]).src_ht == -16);
        Expect(as_cast(instrs[7]).dst_ht == -16);
        // br_on_cast_fail 0 (ref null func) (ref func)
        Expect(instrs[10].opcode == Opcode::Br_on_cast_fail);
        Expect(as_cast(instrs[10]).label == 0);
        Expect(as_cast(instrs[10]).src_n == true);
        Expect(as_cast(instrs[10]).dst_n == false);
    })
    Test("tail_call", {
        ParseFile(test_module, "tail_call.wat");
        std::vector<WasmInstr>& instrs = test_module.funcs[0].body;
        // return_call 0
        Expect(instrs[0].opcode == Opcode::Return_call);
        Expect(as_one(instrs[0]).index == 0);
        // return_call_indirect (type 0) - tableidx=0, typeidx=0
        Expect(instrs[1].opcode == Opcode::Return_call_indirect);
        Expect(as_two(instrs[1]).a == 0);  // tableidx
        Expect(as_two(instrs[1]).b == 0);  // typeidx
        // return_call_indirect 1 (type 0) - tableidx=1, typeidx=0
        Expect(instrs[2].opcode == Opcode::Return_call_indirect);
        Expect(as_two(instrs[2]).a == 1);  // tableidx
        Expect(as_two(instrs[2]).b == 0);  // typeidx
    })
    Test("exception", {
        ParseFile(test_module, "exception.wat");
        std::vector<WasmInstr>& instrs = test_module.funcs[0].body;
        // throw_ref
        Expect(instrs[0].opcode == Opcode::Throw_ref);
        // throw $t0 (tag 0)
        Expect(instrs[1].opcode == Opcode::Throw);
        Expect(as_one(instrs[1]).index == 0);
        // try_table (catch $t0 0) nop end
        Expect(instrs[2].opcode == Opcode::Try_table);
        Expect(as_try(instrs[2]).catches.size() == 1);
        Expect(as_try(instrs[2]).catches[0].kind == Instr::TryCatchEntry::Catch);
        Expect(as_try(instrs[2]).catches[0].tag_idx == 0);
        Expect(as_try(instrs[2]).catches[0].label_idx == 0);
        Expect(instrs[3].opcode == Opcode::Nop);
        Expect(instrs[4].opcode == Opcode::End);
        // try_table (catch_ref $t0 0) (catch_all 0) nop end
        Expect(instrs[5].opcode == Opcode::Try_table);
        Expect(as_try(instrs[5]).catches.size() == 2);
        Expect(as_try(instrs[5]).catches[0].kind == Instr::TryCatchEntry::CatchRef);
        Expect(as_try(instrs[5]).catches[1].kind == Instr::TryCatchEntry::CatchAll);
        Expect(instrs[7].opcode == Opcode::End);
        // try_table (catch_all_ref 0) nop end
        Expect(instrs[8].opcode == Opcode::Try_table);
        Expect(as_try(instrs[8]).catches.size() == 1);
        Expect(as_try(instrs[8]).catches[0].kind == Instr::TryCatchEntry::CatchAllRef);
    })
    Test("gc_struct", {
        ParseFile(test_module, "gc_struct.wat");
        std::vector<WasmInstr>& instrs = test_module.funcs[0].body;
        // struct.new 0
        Expect(instrs[0].opcode == Opcode::Struct_new);
        Expect(as_one(instrs[0]).index == 0);
        // struct.new_default 1
        Expect(instrs[1].opcode == Opcode::Struct_new_default);
        Expect(as_one(instrs[1]).index == 1);
        // struct.get 2 3 (typeidx=a, fieldidx=b)
        Expect(instrs[2].opcode == Opcode::Struct_get);
        Expect(as_two(instrs[2]).a == 2);  // typeidx
        Expect(as_two(instrs[2]).b == 3);  // fieldidx
        // struct.get_s 2 3
        Expect(instrs[3].opcode == Opcode::Struct_get_s);
        Expect(as_two(instrs[3]).a == 2);
        Expect(as_two(instrs[3]).b == 3);
        // struct.get_u 2 3
        Expect(instrs[4].opcode == Opcode::Struct_get_u);
        Expect(as_two(instrs[4]).a == 2);
        Expect(as_two(instrs[4]).b == 3);
        // struct.set 2 3
        Expect(instrs[5].opcode == Opcode::Struct_set);
        Expect(as_two(instrs[5]).a == 2);
        Expect(as_two(instrs[5]).b == 3);
    })
    Test("gc_array", {
        ParseFile(test_module, "gc_array.wat");
        std::vector<WasmInstr>& instrs = test_module.funcs[0].body;
        // array.new 0
        Expect(instrs[0].opcode == Opcode::Array_new);
        Expect(as_one(instrs[0]).index == 0);
        // array.new_default 1
        Expect(instrs[1].opcode == Opcode::Array_new_default);
        Expect(as_one(instrs[1]).index == 1);
        // array.new_fixed 2 5 (typeidx=a, n=b)
        Expect(instrs[2].opcode == Opcode::Array_new_fixed);
        Expect(as_two(instrs[2]).a == 2);  // typeidx
        Expect(as_two(instrs[2]).b == 5);  // n
        // array.new_data 3 0 (typeidx=a, dataidx=b)
        Expect(instrs[3].opcode == Opcode::Array_new_data);
        Expect(as_two(instrs[3]).a == 3);  // typeidx
        Expect(as_two(instrs[3]).b == 0);  // dataidx
        // array.new_elem 4 0 (typeidx=a, elemidx=b)
        Expect(instrs[4].opcode == Opcode::Array_new_elem);
        Expect(as_two(instrs[4]).a == 4);  // typeidx
        Expect(as_two(instrs[4]).b == 0);  // elemidx
        // array.get/get_s/get_u/set 5
        Expect(instrs[5].opcode == Opcode::Array_get);
        Expect(as_one(instrs[5]).index == 5);
        Expect(instrs[6].opcode == Opcode::Array_get_s);
        Expect(as_one(instrs[6]).index == 5);
        Expect(instrs[7].opcode == Opcode::Array_get_u);
        Expect(as_one(instrs[7]).index == 5);
        Expect(instrs[8].opcode == Opcode::Array_set);
        Expect(as_one(instrs[8]).index == 5);
        // array.len
        Expect(instrs[9].opcode == Opcode::Array_len);
        // array.fill 6 (typeidx stored as OneIdx.index)
        Expect(instrs[10].opcode == Opcode::Array_fill);
        Expect(as_one(instrs[10]).index == 6);
        // array.copy 7 8 (dst_typeidx=a, src_typeidx=b)
        Expect(instrs[11].opcode == Opcode::Array_copy);
        Expect(as_two(instrs[11]).a == 7);  // dst_typeidx
        Expect(as_two(instrs[11]).b == 8);  // src_typeidx
        // array.init_data 9 0 (typeidx=a, dataidx=b)
        Expect(instrs[12].opcode == Opcode::Array_init_data);
        Expect(as_two(instrs[12]).a == 9);
        Expect(as_two(instrs[12]).b == 0);
        // array.init_elem 9 0 (typeidx=a, elemidx=b)
        Expect(instrs[13].opcode == Opcode::Array_init_elem);
        Expect(as_two(instrs[13]).a == 9);
        Expect(as_two(instrs[13]).b == 0);
    })
    Test("parametric", {
        ParseFile(test_module, "parametric.wat");
        std::vector<WasmInstr>& instrs = test_module.funcs[0].body;
        Expect(instrs[0].opcode == Opcode::Select);
        Expect(as_sel(instrs[0]).valtypes.empty());
        Expect(instrs[1].opcode == Opcode::Select);
        Expect(as_sel(instrs[1]).valtypes.size() == 2);
        Expect(as_sel(instrs[1]).valtypes[0] == ValueType::i32);
        Expect(as_sel(instrs[1]).valtypes[1] == ValueType::i64);
        Expect(instrs[2].opcode == Opcode::Drop);
    })
    Test("variable", {
        ParseFile(test_module, "variable.wat");
        std::vector<WasmInstr>& instrs = test_module.funcs[0].body;
        Expect(instrs[0].opcode == Opcode::Local_get);
        Expect(as_one(instrs[0]).index == 0);
        Expect(instrs[1].opcode == Opcode::Local_get);
        Expect(as_one(instrs[1]).index == 1);
        Expect(instrs[2].opcode == Opcode::Local_set);
        Expect(as_one(instrs[2]).index == 0);
        Expect(instrs[3].opcode == Opcode::Local_tee);
        Expect(as_one(instrs[3]).index == 0);
        Expect(instrs[4].opcode == Opcode::Global_get);
        Expect(as_one(instrs[4]).index == 0);
        Expect(instrs[5].opcode == Opcode::Global_set);
        Expect(as_one(instrs[5]).index == 0);
    })
    Test("table", {
        ParseFile(test_module, "table.wat");
        std::vector<WasmInstr>& instrs = test_module.funcs[0].body;
        Expect(instrs[0].opcode == Opcode::Table_get);
        Expect(as_one(instrs[0]).index == 1);
        Expect(instrs[1].opcode == Opcode::Table_get);
        Expect(as_one(instrs[1]).index == 0);
        Expect(instrs[2].opcode == Opcode::Table_set);
        Expect(as_one(instrs[2]).index == 1);
        Expect(instrs[3].opcode == Opcode::Table_set);
        Expect(as_one(instrs[3]).index == 0);
        Expect(instrs[4].opcode == Opcode::Table_size);
        Expect(as_one(instrs[4]).index == 1);
        Expect(instrs[5].opcode == Opcode::Table_size);
        Expect(as_one(instrs[5]).index == 0);
        Expect(instrs[6].opcode == Opcode::Table_grow);
        Expect(as_one(instrs[6]).index == 1);
        Expect(instrs[7].opcode == Opcode::Table_grow);
        Expect(as_one(instrs[7]).index == 0);
        Expect(instrs[8].opcode == Opcode::Table_fill);
        Expect(as_one(instrs[8]).index == 1);
        Expect(instrs[9].opcode == Opcode::Table_fill);
        Expect(as_one(instrs[9]).index == 0);
        // table.copy (dstidx=a, srcidx=b)
        Expect(instrs[10].opcode == Opcode::Table_copy);
        Expect(as_two(instrs[10]).a == 1);  // dstidx
        Expect(as_two(instrs[10]).b == 2);  // srcidx
        Expect(instrs[11].opcode == Opcode::Table_copy);
        Expect(as_two(instrs[11]).a == 0);
        Expect(as_two(instrs[11]).b == 0);
        // table.init (tableidx=a, elemidx=b)
        Expect(instrs[12].opcode == Opcode::Table_init);
        Expect(as_two(instrs[12]).a == 1);  // tableidx
        Expect(as_two(instrs[12]).b == 3);  // elemidx
        Expect(instrs[13].opcode == Opcode::Table_init);
        Expect(as_two(instrs[13]).a == 0);
        Expect(as_two(instrs[13]).b == 3);
        Expect(instrs[14].opcode == Opcode::Elem_drop);
        Expect(as_one(instrs[14]).index == 4);
    })
    Test("memory", {
        ParseFile(test_module, "memory.wat");
        std::vector<WasmInstr>& instrs = test_module.funcs[0].body;
        // i32
        Expect(instrs[0].opcode == Opcode::I32_load);
        Expect(as_mem(instrs[0]).memidx == 0);
        Expect(as_mem(instrs[0]).offset == 4);
        Expect(as_mem(instrs[0]).align == 2);
        Expect(instrs[1].opcode == Opcode::I32_load8_s);
        Expect(as_mem(instrs[1]).memidx == 1);
        Expect(as_mem(instrs[1]).offset == 0);
        Expect(as_mem(instrs[1]).align == 0);
        Expect(instrs[2].opcode == Opcode::I32_load8_u);
        Expect(as_mem(instrs[2]).memidx == 0);
        Expect(as_mem(instrs[2]).offset == 0);
        Expect(as_mem(instrs[2]).align == 0);
        Expect(instrs[3].opcode == Opcode::I32_load16_s);
        Expect(as_mem(instrs[3]).memidx == 0);
        Expect(as_mem(instrs[3]).offset == 0);
        Expect(as_mem(instrs[3]).align == 1);
        Expect(instrs[4].opcode == Opcode::I32_load16_u);
        Expect(as_mem(instrs[4]).memidx == 0);
        Expect(as_mem(instrs[4]).offset == 0);
        Expect(as_mem(instrs[4]).align == 1);
        Expect(instrs[5].opcode == Opcode::I32_store);
        Expect(as_mem(instrs[5]).memidx == 0);
        Expect(as_mem(instrs[5]).offset == 0);
        Expect(as_mem(instrs[5]).align == 2);
        Expect(instrs[6].opcode == Opcode::I32_store8);
        Expect(as_mem(instrs[6]).memidx == 0);
        Expect(as_mem(instrs[6]).offset == 0);
        Expect(as_mem(instrs[6]).align == 0);
        Expect(instrs[7].opcode == Opcode::I32_store16);
        Expect(as_mem(instrs[7]).memidx == 0);
        Expect(as_mem(instrs[7]).offset == 0);
        Expect(as_mem(instrs[7]).align == 1);
        // i64
        Expect(instrs[8].opcode == Opcode::I64_load);
        Expect(as_mem(instrs[8]).memidx == 0);
        Expect(as_mem(instrs[8]).offset == 0);
        Expect(as_mem(instrs[8]).align == 3);
        Expect(instrs[9].opcode == Opcode::I64_load8_s);
        Expect(as_mem(instrs[9]).memidx == 0);
        Expect(as_mem(instrs[9]).offset == 0);
        Expect(as_mem(instrs[9]).align == 0);
        Expect(instrs[10].opcode == Opcode::I64_load8_u);
        Expect(as_mem(instrs[10]).memidx == 0);
        Expect(as_mem(instrs[10]).offset == 0);
        Expect(as_mem(instrs[10]).align == 0);
        Expect(instrs[11].opcode == Opcode::I64_load16_s);
        Expect(as_mem(instrs[11]).memidx == 0);
        Expect(as_mem(instrs[11]).offset == 0);
        Expect(as_mem(instrs[11]).align == 1);
        Expect(instrs[12].opcode == Opcode::I64_load16_u);
        Expect(as_mem(instrs[12]).memidx == 0);
        Expect(as_mem(instrs[12]).offset == 0);
        Expect(as_mem(instrs[12]).align == 1);
        Expect(instrs[13].opcode == Opcode::I64_load32_s);
        Expect(as_mem(instrs[13]).memidx == 0);
        Expect(as_mem(instrs[13]).offset == 0);
        Expect(as_mem(instrs[13]).align == 2);
        Expect(instrs[14].opcode == Opcode::I64_load32_u);
        Expect(as_mem(instrs[14]).memidx == 0);
        Expect(as_mem(instrs[14]).offset == 0);
        Expect(as_mem(instrs[14]).align == 2);
        Expect(instrs[15].opcode == Opcode::I64_store);
        Expect(as_mem(instrs[15]).memidx == 0);
        Expect(as_mem(instrs[15]).offset == 0);
        Expect(as_mem(instrs[15]).align == 3);
        Expect(instrs[16].opcode == Opcode::I64_store8);
        Expect(as_mem(instrs[16]).memidx == 0);
        Expect(as_mem(instrs[16]).offset == 0);
        Expect(as_mem(instrs[16]).align == 0);
        Expect(instrs[17].opcode == Opcode::I64_store16);
        Expect(as_mem(instrs[17]).memidx == 0);
        Expect(as_mem(instrs[17]).offset == 0);
        Expect(as_mem(instrs[17]).align == 1);
        Expect(instrs[18].opcode == Opcode::I64_store32);
        Expect(as_mem(instrs[18]).memidx == 0);
        Expect(as_mem(instrs[18]).offset == 0);
        Expect(as_mem(instrs[18]).align == 2);
        // f32
        Expect(instrs[19].opcode == Opcode::F32_load);
        Expect(as_mem(instrs[19]).memidx == 0);
        Expect(as_mem(instrs[19]).offset == 0);
        Expect(as_mem(instrs[19]).align == 2);
        Expect(instrs[20].opcode == Opcode::F32_store);
        Expect(as_mem(instrs[20]).memidx == 0);
        Expect(as_mem(instrs[20]).offset == 0);
        Expect(as_mem(instrs[20]).align == 2);
        // f64
        Expect(instrs[21].opcode == Opcode::F64_load);
        Expect(as_mem(instrs[21]).memidx == 0);
        Expect(as_mem(instrs[21]).offset == 0);
        Expect(as_mem(instrs[21]).align == 3);
        Expect(instrs[22].opcode == Opcode::F64_store);
        Expect(as_mem(instrs[22]).memidx == 0);
        Expect(as_mem(instrs[22]).offset == 0);
        Expect(as_mem(instrs[22]).align == 3);
    })
    Category("numeric", {
        Test("i32", {
            ParseFile(test_module, "i32.wat");
            std::vector<WasmInstr>& instrs = test_module.funcs[0].body;
            Expect(instrs[0].opcode == Opcode::I32_const);
            Expect(as_ci32(instrs[0]).value == 2);
            Expect(instrs[1].opcode == Opcode::I32_clz);
            Expect(instrs[2].opcode == Opcode::I32_ctz);
            Expect(instrs[3].opcode == Opcode::I32_popcnt);
            Expect(instrs[4].opcode == Opcode::I32_add);
            Expect(instrs[5].opcode == Opcode::I32_sub);
            Expect(instrs[6].opcode == Opcode::I32_mul);
            Expect(instrs[7].opcode == Opcode::I32_div_s);
            Expect(instrs[8].opcode == Opcode::I32_div_u);
            Expect(instrs[9].opcode == Opcode::I32_rem_s);
            Expect(instrs[10].opcode == Opcode::I32_rem_u);
            Expect(instrs[11].opcode == Opcode::I32_and);
            Expect(instrs[12].opcode == Opcode::I32_or);
            Expect(instrs[13].opcode == Opcode::I32_xor);
            Expect(instrs[14].opcode == Opcode::I32_shl);
            Expect(instrs[15].opcode == Opcode::I32_shr_s);
            Expect(instrs[16].opcode == Opcode::I32_shr_u);
            Expect(instrs[17].opcode == Opcode::I32_rotl);
            Expect(instrs[18].opcode == Opcode::I32_rotr);
            Expect(instrs[19].opcode == Opcode::I32_eqz);
            Expect(instrs[20].opcode == Opcode::I32_eq);
            Expect(instrs[21].opcode == Opcode::I32_ne);
            Expect(instrs[22].opcode == Opcode::I32_lt_s);
            Expect(instrs[23].opcode == Opcode::I32_lt_u);
            Expect(instrs[24].opcode == Opcode::I32_gt_s);
            Expect(instrs[25].opcode == Opcode::I32_gt_u);
            Expect(instrs[26].opcode == Opcode::I32_le_s);
            Expect(instrs[27].opcode == Opcode::I32_le_u);
            Expect(instrs[28].opcode == Opcode::I32_ge_s);
            Expect(instrs[29].opcode == Opcode::I32_ge_u);
            Expect(instrs[30].opcode == Opcode::I32_wrap_i64);
            Expect(instrs[31].opcode == Opcode::I32_trunc_f32_s);
            Expect(instrs[32].opcode == Opcode::I32_trunc_f32_u);
            Expect(instrs[33].opcode == Opcode::I32_trunc_f64_s);
            Expect(instrs[34].opcode == Opcode::I32_trunc_f64_u);
            Expect(instrs[35].opcode == Opcode::I32_trunc_sat_f32_s);
            Expect(instrs[36].opcode == Opcode::I32_trunc_sat_f32_u);
            Expect(instrs[37].opcode == Opcode::I32_trunc_sat_f64_s);
            Expect(instrs[38].opcode == Opcode::I32_trunc_sat_f64_u);
            Expect(instrs[39].opcode == Opcode::I32_reinterpret_f32);
            Expect(instrs[40].opcode == Opcode::I32_extend8_s);
            Expect(instrs[41].opcode == Opcode::I32_extend16_s);
        })
        Test("i64", {
            ParseFile(test_module, "i64.wat");
            std::vector<WasmInstr>& instrs = test_module.funcs[0].body;
            Expect(instrs[0].opcode == Opcode::I64_const);
            Expect(as_ci64(instrs[0]).value == 2);
            Expect(instrs[1].opcode == Opcode::I64_clz);
            Expect(instrs[2].opcode == Opcode::I64_ctz);
            Expect(instrs[3].opcode == Opcode::I64_popcnt);
            Expect(instrs[4].opcode == Opcode::I64_add);
            Expect(instrs[5].opcode == Opcode::I64_sub);
            Expect(instrs[6].opcode == Opcode::I64_mul);
            Expect(instrs[7].opcode == Opcode::I64_div_s);
            Expect(instrs[8].opcode == Opcode::I64_div_u);
            Expect(instrs[9].opcode == Opcode::I64_rem_s);
            Expect(instrs[10].opcode == Opcode::I64_rem_u);
            Expect(instrs[11].opcode == Opcode::I64_and);
            Expect(instrs[12].opcode == Opcode::I64_or);
            Expect(instrs[13].opcode == Opcode::I64_xor);
            Expect(instrs[14].opcode == Opcode::I64_shl);
            Expect(instrs[15].opcode == Opcode::I64_shr_s);
            Expect(instrs[16].opcode == Opcode::I64_shr_u);
            Expect(instrs[17].opcode == Opcode::I64_rotl);
            Expect(instrs[18].opcode == Opcode::I64_rotr);
            Expect(instrs[19].opcode == Opcode::I64_eqz);
            Expect(instrs[20].opcode == Opcode::I64_eq);
            Expect(instrs[21].opcode == Opcode::I64_ne);
            Expect(instrs[22].opcode == Opcode::I64_lt_s);
            Expect(instrs[23].opcode == Opcode::I64_lt_u);
            Expect(instrs[24].opcode == Opcode::I64_gt_s);
            Expect(instrs[25].opcode == Opcode::I64_gt_u);
            Expect(instrs[26].opcode == Opcode::I64_le_s);
            Expect(instrs[27].opcode == Opcode::I64_le_u);
            Expect(instrs[28].opcode == Opcode::I64_ge_s);
            Expect(instrs[29].opcode == Opcode::I64_ge_u);
            Expect(instrs[30].opcode == Opcode::I64_extend_i32_s);
            Expect(instrs[31].opcode == Opcode::I64_extend_i32_u);
            Expect(instrs[32].opcode == Opcode::I64_trunc_f32_s);
            Expect(instrs[33].opcode == Opcode::I64_trunc_f32_u);
            Expect(instrs[34].opcode == Opcode::I64_trunc_f64_s);
            Expect(instrs[35].opcode == Opcode::I64_trunc_f64_u);
            Expect(instrs[36].opcode == Opcode::I64_trunc_sat_f32_s);
            Expect(instrs[37].opcode == Opcode::I64_trunc_sat_f32_u);
            Expect(instrs[38].opcode == Opcode::I64_trunc_sat_f64_s);
            Expect(instrs[39].opcode == Opcode::I64_trunc_sat_f64_u);
            Expect(instrs[40].opcode == Opcode::I64_extend8_s);
            Expect(instrs[41].opcode == Opcode::I64_extend16_s);
            Expect(instrs[42].opcode == Opcode::I64_extend32_s);
        })
        Test("f32", {
            ParseFile(test_module, "f32.wat");
            std::vector<WasmInstr>& instrs = test_module.funcs[0].body;
            Expect(instrs[0].opcode == Opcode::F32_const);
            Expect(as_cf32(instrs[0]).value == 3.14f);
            Expect(instrs[1].opcode == Opcode::F32_abs);
            Expect(instrs[2].opcode == Opcode::F32_neg);
            Expect(instrs[3].opcode == Opcode::F32_ceil);
            Expect(instrs[4].opcode == Opcode::F32_floor);
            Expect(instrs[5].opcode == Opcode::F32_trunc);
            Expect(instrs[6].opcode == Opcode::F32_nearest);
            Expect(instrs[7].opcode == Opcode::F32_sqrt);
            Expect(instrs[8].opcode == Opcode::F32_add);
            Expect(instrs[9].opcode == Opcode::F32_sub);
            Expect(instrs[10].opcode == Opcode::F32_mul);
            Expect(instrs[11].opcode == Opcode::F32_div);
            Expect(instrs[12].opcode == Opcode::F32_min);
            Expect(instrs[13].opcode == Opcode::F32_max);
            Expect(instrs[14].opcode == Opcode::F32_copysign);
            Expect(instrs[15].opcode == Opcode::F32_eq);
            Expect(instrs[16].opcode == Opcode::F32_ne);
            Expect(instrs[17].opcode == Opcode::F32_lt);
            Expect(instrs[18].opcode == Opcode::F32_gt);
            Expect(instrs[19].opcode == Opcode::F32_le);
            Expect(instrs[20].opcode == Opcode::F32_ge);
            Expect(instrs[21].opcode == Opcode::F32_convert_i32_s);
            Expect(instrs[22].opcode == Opcode::F32_convert_i32_u);
            Expect(instrs[23].opcode == Opcode::F32_convert_i64_s);
            Expect(instrs[24].opcode == Opcode::F32_convert_i64_u);
            Expect(instrs[25].opcode == Opcode::F32_demote_f64);
        })
        Test("f64", {
            ParseFile(test_module, "f64.wat");
            std::vector<WasmInstr>& instrs = test_module.funcs[0].body;
            Expect(instrs[0].opcode == Opcode::F64_const);
            Expect(as_cf64(instrs[0]).value == 3.14);
            Expect(instrs[1].opcode == Opcode::F64_abs);
            Expect(instrs[2].opcode == Opcode::F64_neg);
            Expect(instrs[3].opcode == Opcode::F64_ceil);
            Expect(instrs[4].opcode == Opcode::F64_floor);
            Expect(instrs[5].opcode == Opcode::F64_trunc);
            Expect(instrs[6].opcode == Opcode::F64_nearest);
            Expect(instrs[7].opcode == Opcode::F64_sqrt);
            Expect(instrs[8].opcode == Opcode::F64_add);
            Expect(instrs[9].opcode == Opcode::F64_sub);
            Expect(instrs[10].opcode == Opcode::F64_mul);
            Expect(instrs[11].opcode == Opcode::F64_div);
            Expect(instrs[12].opcode == Opcode::F64_min);
            Expect(instrs[13].opcode == Opcode::F64_max);
            Expect(instrs[14].opcode == Opcode::F64_copysign);
            Expect(instrs[15].opcode == Opcode::F64_eq);
            Expect(instrs[16].opcode == Opcode::F64_ne);
            Expect(instrs[17].opcode == Opcode::F64_lt);
            Expect(instrs[18].opcode == Opcode::F64_gt);
            Expect(instrs[19].opcode == Opcode::F64_le);
            Expect(instrs[20].opcode == Opcode::F64_ge);
            Expect(instrs[21].opcode == Opcode::F64_convert_i32_s);
            Expect(instrs[22].opcode == Opcode::F64_convert_i32_u);
            Expect(instrs[23].opcode == Opcode::F64_convert_i64_s);
            Expect(instrs[24].opcode == Opcode::F64_convert_i64_u);
            Expect(instrs[25].opcode == Opcode::F64_promote_f32);
            Expect(instrs[26].opcode == Opcode::F64_reinterpret_i64);
        })
    })
    Category("invalid", {
        Test("unknown instruction", {
            Throw(Exception::Parse, {
                std::istringstream stream("(module (func i99.nonexistent))");
                module_parse(stream);
            });
        })
        Test("unknown instruction variants", {
            ExpectThrows(Exception::Parse, {
                std::istringstream ss("(module (func bad.instr))");
                module_parse(ss);
            });
            ExpectThrows(Exception::Parse, {
                std::istringstream ss("(module (func i32.bad))");
                module_parse(ss);
            });
            ExpectThrows(Exception::Parse, {
                std::istringstream ss("(module (func f64.bad_op))");
                module_parse(ss);
            });
        })
        Test("duplicate label", {
            Throw(Exception::Parse, {
                std::istringstream stream("(module (func (block $lbl (block $lbl end) end)))");
                module_parse(stream);
            });
        })
        Test("label not found", {
            Throw(Exception::Parse, {
                std::istringstream stream("(module (func (br $missing)))");
                module_parse(stream);
            });
        })
    })
    Test("folded", {
        ParseFile(test_module, "folded.wat");
        std::vector<WasmInstr>& instrs = test_module.funcs[0].body;
        // Single plain
        Expect(instrs[0].opcode == Opcode::Unreachable);
        // plain with folded
        Expect(instrs[1].opcode == Opcode::Return);
        Expect(instrs[2].opcode == Opcode::Nop);
        Expect(instrs[3].opcode == Opcode::Unreachable);
        // block
        Expect(instrs[4].opcode == Opcode::Block);
        Expect(instrs[5].opcode == Opcode::Nop);
        Expect(instrs[6].opcode == Opcode::End);
        // loop
        Expect(instrs[7].opcode == Opcode::Loop);
        Expect(instrs[8].opcode == Opcode::Unreachable);
        Expect(instrs[9].opcode == Opcode::End);
        // if
        Expect(instrs[10].opcode == Opcode::If);
        Expect(instrs[11].opcode == Opcode::Unreachable);
        Expect(instrs[12].opcode == Opcode::Else);
        Expect(instrs[13].opcode == Opcode::End);
        // if then else
        Expect(instrs[14].opcode == Opcode::If);
        Expect(instrs[15].opcode == Opcode::Unreachable);
        Expect(instrs[16].opcode == Opcode::Else);
        Expect(instrs[17].opcode == Opcode::Nop);
        Expect(instrs[18].opcode == Opcode::End);
        // if folded
        Expect(instrs[19].opcode == Opcode::Return);
        Expect(instrs[20].opcode == Opcode::If);
        Expect(instrs[21].opcode == Opcode::Unreachable);
        Expect(instrs[22].opcode == Opcode::Else);
        Expect(instrs[23].opcode == Opcode::Nop);
        Expect(instrs[24].opcode == Opcode::End);
    })
};
