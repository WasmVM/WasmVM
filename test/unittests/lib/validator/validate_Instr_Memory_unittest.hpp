#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <dataTypes/Value.h>
#include <dataTypes/stack_p.h>
#include <dataTypes/vector_p.h>
#include <dataTypes/FuncType.h>
#include <structures/instrs/Memory.h>
#include <structures/WasmMemory.h>
#include <structures/WasmFunc.h>
#include <structures/WasmModule.h>
#include <Validates.h>
#include <Opcodes.h>
#include <Context.h>
}
#undef _Bool

static void test_storeN_align(stack_p opds, Context* context, stack_p ctrls, WasmMemoryInstr* instr, ValueType expect)
{
    ValueType* address = (ValueType*) malloc(sizeof(ValueType));
    *address = Value_i32;
    stack_push(opds, address);
    EXPECT_EQ(validate_Instr_storeN(instr, context, opds, ctrls), -3);
}

static void test_storeN(stack_p opds, Context* context, stack_p ctrls, WasmMemoryInstr* instr, ValueType expect)
{
    ValueType* address = (ValueType*) malloc(sizeof(ValueType));
    *address = Value_i32;
    stack_push(opds, address);
    ValueType* operand = (ValueType*) malloc(sizeof(ValueType));
    *operand = expect;
    stack_push(opds, operand);
    EXPECT_EQ(validate_Instr_storeN(instr, context, opds, ctrls), 0);
    EXPECT_EQ(stack_size(opds), 0);
}

static void test_store(stack_p opds, Context* context, stack_p ctrls, WasmMemoryInstr* instr, ValueType expect)
{
    ValueType* address = (ValueType*) malloc(sizeof(ValueType));
    *address = Value_i32;
    stack_push(opds, address);
    ValueType* operand = (ValueType*) malloc(sizeof(ValueType));
    *operand = expect;
    stack_push(opds, operand);
    EXPECT_EQ(validate_Instr_store(instr, context, opds, ctrls), 0);
    EXPECT_EQ(stack_size(opds), 0);
}

static void test_load(stack_p opds, Context* context, stack_p ctrls, WasmMemoryInstr* instr, ValueType expect)
{
    ValueType* address = (ValueType*) malloc(sizeof(ValueType));
    *address = Value_i32;
    stack_push(opds, address);
    EXPECT_EQ(validate_Instr_load(instr, context, opds, ctrls), 0);
    EXPECT_EQ(stack_size(opds), 1);
    ValueType* operand = stack_pop(ValueType*, opds);
    EXPECT_EQ(*operand, expect);
}

static void test_loadN(stack_p opds, Context* context, stack_p ctrls, WasmMemoryInstr* instr, ValueType expect)
{
    ValueType* address = (ValueType*) malloc(sizeof(ValueType));
    *address = Value_i32;
    stack_push(opds, address);
    EXPECT_EQ(validate_Instr_loadN(instr, context, opds, ctrls), 0);
    EXPECT_EQ(stack_size(opds), 1);
    ValueType* operand = stack_pop(ValueType*, opds);
    EXPECT_EQ(*operand, expect);
}

static void test_loadN_align(stack_p opds, Context* context, stack_p ctrls, WasmMemoryInstr* instr, ValueType expect)
{
    ValueType* address = (ValueType*) malloc(sizeof(ValueType));
    *address = Value_i32;
    stack_push(opds, address);
    EXPECT_EQ(validate_Instr_loadN(instr, context, opds, ctrls), -3);
}

static void clean(stack_p opds, stack_p ctrls)
{
    free_stack_p(opds);
    free_stack_p(ctrls);
}

SKYPAT_F(validate_Instr_load, valid)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    WasmMemory* memory = (WasmMemory*) malloc(sizeof(WasmMemory));
    memory->min = 1;
    memory->max = 0;
    vector_push_back(module->mems, memory);
    Context* context = new_Context(module, func);
    stack_p opds = new_stack_p(free);
    stack_p ctrls = new_stack_p((void(*)(void*))free_ctrl_frame);
    ctrl_frame* frame = new_ctrl_frame(opds);
    stack_push(ctrls, frame);

    WasmMemoryInstr* instr = new_WasmMemoryInstr(0, 0);

    // Check
    instr->parent.opcode = Op_i32_load;
    test_load(opds, context, ctrls, instr, Value_i32);
    instr->parent.opcode = Op_i64_load;
    test_load(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_f32_load;
    test_load(opds, context, ctrls, instr, Value_f32);
    instr->parent.opcode = Op_f64_load;
    test_load(opds, context, ctrls, instr, Value_f64);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_load, memory_not_exist)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    Context* context = new_Context(module, func);
    stack_p opds = new_stack_p(free);
    stack_p ctrls = new_stack_p((void(*)(void*))free_ctrl_frame);
    ctrl_frame* frame = new_ctrl_frame(opds);
    stack_push(ctrls, frame);

    WasmMemoryInstr* instr = new_WasmMemoryInstr(0, 0);

    // Check
    instr->parent.opcode = Op_i32_load;
    ValueType* address = (ValueType*) malloc(sizeof(ValueType));
    *address = Value_i32;
    stack_push(opds, address);
    EXPECT_EQ(validate_Instr_load(instr, context, opds, ctrls), -1);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_load, align_too_much)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    WasmMemory* memory = (WasmMemory*) malloc(sizeof(WasmMemory));
    memory->min = 1;
    memory->max = 0;
    vector_push_back(module->mems, memory);
    Context* context = new_Context(module, func);
    stack_p opds = new_stack_p(free);
    stack_p ctrls = new_stack_p((void(*)(void*))free_ctrl_frame);
    ctrl_frame* frame = new_ctrl_frame(opds);
    stack_push(ctrls, frame);

    WasmMemoryInstr* instr = new_WasmMemoryInstr(4, 0);

    // Check
    instr->parent.opcode = Op_i32_load;
    ValueType* address = (ValueType*) malloc(sizeof(ValueType));
    *address = Value_i32;
    stack_push(opds, address);
    EXPECT_EQ(validate_Instr_load(instr, context, opds, ctrls), -3);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_load, no_enough_operand)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    WasmMemory* memory = (WasmMemory*) malloc(sizeof(WasmMemory));
    memory->min = 1;
    memory->max = 0;
    vector_push_back(module->mems, memory);
    Context* context = new_Context(module, func);
    stack_p opds = new_stack_p(free);
    stack_p ctrls = new_stack_p((void(*)(void*))free_ctrl_frame);
    ctrl_frame* frame = new_ctrl_frame(opds);
    stack_push(ctrls, frame);

    WasmMemoryInstr* instr = new_WasmMemoryInstr(0, 0);

    // Check
    instr->parent.opcode = Op_i32_load;
    EXPECT_EQ(validate_Instr_load(instr, context, opds, ctrls), -4);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_loadN, valid)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    WasmMemory* memory = (WasmMemory*) malloc(sizeof(WasmMemory));
    memory->min = 1;
    memory->max = 0;
    vector_push_back(module->mems, memory);
    Context* context = new_Context(module, func);
    stack_p opds = new_stack_p(free);
    stack_p ctrls = new_stack_p((void(*)(void*))free_ctrl_frame);
    ctrl_frame* frame = new_ctrl_frame(opds);
    stack_push(ctrls, frame);

    WasmMemoryInstr* instr = new_WasmMemoryInstr(0, 0);

    // Check
    instr->parent.opcode = Op_i32_load8_s;
    test_loadN(opds, context, ctrls, instr, Value_i32);
    instr->parent.opcode = Op_i32_load8_u;
    test_loadN(opds, context, ctrls, instr, Value_i32);
    instr->parent.opcode = Op_i32_load16_s;
    test_loadN(opds, context, ctrls, instr, Value_i32);
    instr->parent.opcode = Op_i32_load16_u;
    test_loadN(opds, context, ctrls, instr, Value_i32);
    instr->parent.opcode = Op_i64_load8_s;
    test_loadN(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_i64_load8_u;
    test_loadN(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_i64_load16_s;
    test_loadN(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_i64_load16_u;
    test_loadN(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_i64_load32_s;
    test_loadN(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_i64_load32_u;
    test_loadN(opds, context, ctrls, instr, Value_i64);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_loadN, memory_not_exist)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    Context* context = new_Context(module, func);
    stack_p opds = new_stack_p(free);
    stack_p ctrls = new_stack_p((void(*)(void*))free_ctrl_frame);
    ctrl_frame* frame = new_ctrl_frame(opds);
    stack_push(ctrls, frame);

    WasmMemoryInstr* instr = new_WasmMemoryInstr(0, 0);

    // Check
    instr->parent.opcode = Op_i32_load8_s;
    ValueType* address = (ValueType*) malloc(sizeof(ValueType));
    *address = Value_i32;
    stack_push(opds, address);
    EXPECT_EQ(validate_Instr_loadN(instr, context, opds, ctrls), -1);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_loadN, no_enough_operand)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    WasmMemory* memory = (WasmMemory*) malloc(sizeof(WasmMemory));
    memory->min = 1;
    memory->max = 0;
    vector_push_back(module->mems, memory);
    Context* context = new_Context(module, func);
    stack_p opds = new_stack_p(free);
    stack_p ctrls = new_stack_p((void(*)(void*))free_ctrl_frame);
    ctrl_frame* frame = new_ctrl_frame(opds);
    stack_push(ctrls, frame);

    WasmMemoryInstr* instr = new_WasmMemoryInstr(0, 0);

    // Check
    instr->parent.opcode = Op_i32_load8_s;
    EXPECT_EQ(validate_Instr_loadN(instr, context, opds, ctrls), -4);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_loadN, align_too_much)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    WasmMemory* memory = (WasmMemory*) malloc(sizeof(WasmMemory));
    memory->min = 1;
    memory->max = 0;
    vector_push_back(module->mems, memory);
    Context* context = new_Context(module, func);
    stack_p opds = new_stack_p(free);
    stack_p ctrls = new_stack_p((void(*)(void*))free_ctrl_frame);
    ctrl_frame* frame = new_ctrl_frame(opds);
    stack_push(ctrls, frame);

    WasmMemoryInstr* instr = new_WasmMemoryInstr(0, 0);

    // Check
    instr->parent.opcode = Op_i32_load8_s;
    instr->align = 1;
    test_loadN_align(opds, context, ctrls, instr, Value_i32);
    instr->parent.opcode = Op_i32_load8_u;
    instr->align = 1;
    test_loadN_align(opds, context, ctrls, instr, Value_i32);
    instr->parent.opcode = Op_i32_load16_s;
    instr->align = 2;
    test_loadN_align(opds, context, ctrls, instr, Value_i32);
    instr->parent.opcode = Op_i32_load16_u;
    instr->align = 2;
    test_loadN_align(opds, context, ctrls, instr, Value_i32);
    instr->parent.opcode = Op_i64_load8_s;
    instr->align = 1;
    test_loadN_align(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_i64_load8_u;
    instr->align = 1;
    test_loadN_align(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_i64_load16_s;
    instr->align = 2;
    test_loadN_align(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_i64_load16_u;
    instr->align = 2;
    test_loadN_align(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_i64_load32_s;
    instr->align = 3;
    test_loadN_align(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_i64_load32_u;
    instr->align = 3;
    test_loadN_align(opds, context, ctrls, instr, Value_i64);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_store, valid)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    WasmMemory* memory = (WasmMemory*) malloc(sizeof(WasmMemory));
    memory->min = 1;
    memory->max = 0;
    vector_push_back(module->mems, memory);
    Context* context = new_Context(module, func);
    stack_p opds = new_stack_p(free);
    stack_p ctrls = new_stack_p((void(*)(void*))free_ctrl_frame);
    ctrl_frame* frame = new_ctrl_frame(opds);
    stack_push(ctrls, frame);

    WasmMemoryInstr* instr = new_WasmMemoryInstr(0, 0);

    // Check
    instr->parent.opcode = Op_i32_store;
    test_store(opds, context, ctrls, instr, Value_i32);
    instr->parent.opcode = Op_i64_store;
    test_store(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_f32_store;
    test_store(opds, context, ctrls, instr, Value_f32);
    instr->parent.opcode = Op_f64_store;
    test_store(opds, context, ctrls, instr, Value_f64);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_store, memory_not_exist)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    Context* context = new_Context(module, func);
    stack_p opds = new_stack_p(free);
    stack_p ctrls = new_stack_p((void(*)(void*))free_ctrl_frame);
    ctrl_frame* frame = new_ctrl_frame(opds);
    stack_push(ctrls, frame);

    WasmMemoryInstr* instr = new_WasmMemoryInstr(0, 0);

    // Check
    instr->parent.opcode = Op_i32_store;
    ValueType* address = (ValueType*) malloc(sizeof(ValueType));
    *address = Value_i32;
    stack_push(opds, address);
    EXPECT_EQ(validate_Instr_store(instr, context, opds, ctrls), -1);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_store, align_too_much)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    WasmMemory* memory = (WasmMemory*) malloc(sizeof(WasmMemory));
    memory->min = 1;
    memory->max = 0;
    vector_push_back(module->mems, memory);
    Context* context = new_Context(module, func);
    stack_p opds = new_stack_p(free);
    stack_p ctrls = new_stack_p((void(*)(void*))free_ctrl_frame);
    ctrl_frame* frame = new_ctrl_frame(opds);
    stack_push(ctrls, frame);

    WasmMemoryInstr* instr = new_WasmMemoryInstr(4, 0);

    // Check
    instr->parent.opcode = Op_i32_store;
    ValueType* address = (ValueType*) malloc(sizeof(ValueType));
    *address = Value_i32;
    stack_push(opds, address);
    EXPECT_EQ(validate_Instr_store(instr, context, opds, ctrls), -3);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_store, no_enough_operand)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    WasmMemory* memory = (WasmMemory*) malloc(sizeof(WasmMemory));
    memory->min = 1;
    memory->max = 0;
    vector_push_back(module->mems, memory);
    Context* context = new_Context(module, func);
    stack_p opds = new_stack_p(free);
    stack_p ctrls = new_stack_p((void(*)(void*))free_ctrl_frame);
    ctrl_frame* frame = new_ctrl_frame(opds);
    stack_push(ctrls, frame);

    WasmMemoryInstr* instr = new_WasmMemoryInstr(0, 0);

    // Check
    instr->parent.opcode = Op_i32_store;
    EXPECT_EQ(validate_Instr_store(instr, context, opds, ctrls), -4);
    ValueType* operand = (ValueType*) malloc(sizeof(ValueType));
    *operand = Value_i32;
    stack_push(opds, operand);
    EXPECT_EQ(validate_Instr_store(instr, context, opds, ctrls), -5);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_storeN, valid)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    WasmMemory* memory = (WasmMemory*) malloc(sizeof(WasmMemory));
    memory->min = 1;
    memory->max = 0;
    vector_push_back(module->mems, memory);
    Context* context = new_Context(module, func);
    stack_p opds = new_stack_p(free);
    stack_p ctrls = new_stack_p((void(*)(void*))free_ctrl_frame);
    ctrl_frame* frame = new_ctrl_frame(opds);
    stack_push(ctrls, frame);

    WasmMemoryInstr* instr = new_WasmMemoryInstr(0, 0);

    // Check
    instr->parent.opcode = Op_i32_store8;
    test_storeN(opds, context, ctrls, instr, Value_i32);
    instr->parent.opcode = Op_i32_store16;
    test_storeN(opds, context, ctrls, instr, Value_i32);
    instr->parent.opcode = Op_i64_store8;
    test_storeN(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_i64_store16;
    test_storeN(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_i64_store32;
    test_storeN(opds, context, ctrls, instr, Value_i64);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_storeN, memory_not_exist)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    Context* context = new_Context(module, func);
    stack_p opds = new_stack_p(free);
    stack_p ctrls = new_stack_p((void(*)(void*))free_ctrl_frame);
    ctrl_frame* frame = new_ctrl_frame(opds);
    stack_push(ctrls, frame);

    WasmMemoryInstr* instr = new_WasmMemoryInstr(0, 0);

    // Check
    instr->parent.opcode = Op_i32_store8;
    ValueType* address = (ValueType*) malloc(sizeof(ValueType));
    *address = Value_i32;
    stack_push(opds, address);
    EXPECT_EQ(validate_Instr_storeN(instr, context, opds, ctrls), -1);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_storeN, no_enough_operand)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    WasmMemory* memory = (WasmMemory*) malloc(sizeof(WasmMemory));
    memory->min = 1;
    memory->max = 0;
    vector_push_back(module->mems, memory);
    Context* context = new_Context(module, func);
    stack_p opds = new_stack_p(free);
    stack_p ctrls = new_stack_p((void(*)(void*))free_ctrl_frame);
    ctrl_frame* frame = new_ctrl_frame(opds);
    stack_push(ctrls, frame);

    WasmMemoryInstr* instr = new_WasmMemoryInstr(0, 0);

    // Check
    instr->parent.opcode = Op_i32_store8;
    EXPECT_EQ(validate_Instr_storeN(instr, context, opds, ctrls), -4);
    ValueType* operand = (ValueType*) malloc(sizeof(ValueType));
    *operand = Value_i32;
    stack_push(opds, operand);
    EXPECT_EQ(validate_Instr_storeN(instr, context, opds, ctrls), -5);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_storeN, align_too_much)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    WasmMemory* memory = (WasmMemory*) malloc(sizeof(WasmMemory));
    memory->min = 1;
    memory->max = 0;
    vector_push_back(module->mems, memory);
    Context* context = new_Context(module, func);
    stack_p opds = new_stack_p(free);
    stack_p ctrls = new_stack_p((void(*)(void*))free_ctrl_frame);
    ctrl_frame* frame = new_ctrl_frame(opds);
    stack_push(ctrls, frame);

    WasmMemoryInstr* instr = new_WasmMemoryInstr(0, 0);


    // Check
    instr->parent.opcode = Op_i32_store8;
    instr->align = 1;
    test_storeN_align(opds, context, ctrls, instr, Value_i32);
    instr->parent.opcode = Op_i32_store16;
    instr->align = 2;
    test_storeN_align(opds, context, ctrls, instr, Value_i32);
    instr->parent.opcode = Op_i64_store8;
    instr->align = 1;
    test_storeN_align(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_i64_store16;
    instr->align = 2;
    test_storeN_align(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_i64_store32;
    instr->align = 3;
    test_storeN_align(opds, context, ctrls, instr, Value_i64);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_memory_size, valid)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    WasmMemory* memory = (WasmMemory*) malloc(sizeof(WasmMemory));
    memory->min = 1;
    memory->max = 0;
    vector_push_back(module->mems, memory);
    Context* context = new_Context(module, func);
    stack_p opds = new_stack_p(free);
    stack_p ctrls = new_stack_p((void(*)(void*))free_ctrl_frame);
    ctrl_frame* frame = new_ctrl_frame(opds);
    stack_push(ctrls, frame);

    WasmMemoryInstr* instr = new_WasmMemoryInstr(0, 0);

    // Check
    instr->parent.opcode = Op_memory_size;

    EXPECT_EQ(validate_Instr_memory_size(instr, context, opds, ctrls), 0);
    EXPECT_EQ(stack_size(opds), 1);
    ValueType* operand = stack_pop(ValueType*, opds);
    EXPECT_EQ(*operand, Value_i32);

    // Clean
    free(operand);
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_memory_size, memory_not_exist)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    Context* context = new_Context(module, func);
    stack_p opds = new_stack_p(free);
    stack_p ctrls = new_stack_p((void(*)(void*))free_ctrl_frame);
    ctrl_frame* frame = new_ctrl_frame(opds);
    stack_push(ctrls, frame);

    WasmMemoryInstr* instr = new_WasmMemoryInstr(0, 0);

    // Check
    instr->parent.opcode = Op_memory_size;

    EXPECT_EQ(validate_Instr_memory_size(instr, context, opds, ctrls), -1);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_memory_grow, valid)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    WasmMemory* memory = (WasmMemory*) malloc(sizeof(WasmMemory));
    memory->min = 1;
    memory->max = 0;
    vector_push_back(module->mems, memory);
    Context* context = new_Context(module, func);
    stack_p opds = new_stack_p(free);
    stack_p ctrls = new_stack_p((void(*)(void*))free_ctrl_frame);
    ctrl_frame* frame = new_ctrl_frame(opds);
    stack_push(ctrls, frame);

    WasmMemoryInstr* instr = new_WasmMemoryInstr(0, 0);

    // Check
    instr->parent.opcode = Op_memory_grow;
    ValueType* operand = (ValueType*) malloc(sizeof(ValueType));
    *operand = Value_i32;
    stack_push(opds, operand);
    EXPECT_EQ(validate_Instr_memory_grow(instr, context, opds, ctrls), 0);
    EXPECT_EQ(stack_size(opds), 1);
    ValueType* result = stack_pop(ValueType*, opds);
    EXPECT_EQ(*result, Value_i32);

    // Clean
    free(result);
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_memory_grow, memory_not_exist)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    Context* context = new_Context(module, func);
    stack_p opds = new_stack_p(free);
    stack_p ctrls = new_stack_p((void(*)(void*))free_ctrl_frame);
    ctrl_frame* frame = new_ctrl_frame(opds);
    stack_push(ctrls, frame);

    WasmMemoryInstr* instr = new_WasmMemoryInstr(0, 0);

    // Check
    instr->parent.opcode = Op_memory_grow;
    ValueType* operand = (ValueType*) malloc(sizeof(ValueType));
    *operand = Value_i32;
    stack_push(opds, operand);
    EXPECT_EQ(validate_Instr_memory_grow(instr, context, opds, ctrls), -1);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_memory_grow, no_enough_operand)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    WasmMemory* memory = (WasmMemory*) malloc(sizeof(WasmMemory));
    memory->min = 1;
    memory->max = 0;
    vector_push_back(module->mems, memory);
    Context* context = new_Context(module, func);
    stack_p opds = new_stack_p(free);
    stack_p ctrls = new_stack_p((void(*)(void*))free_ctrl_frame);
    ctrl_frame* frame = new_ctrl_frame(opds);
    stack_push(ctrls, frame);

    WasmMemoryInstr* instr = new_WasmMemoryInstr(0, 0);

    // Check
    instr->parent.opcode = Op_memory_grow;
    EXPECT_EQ(validate_Instr_memory_grow(instr, context, opds, ctrls), -2);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}