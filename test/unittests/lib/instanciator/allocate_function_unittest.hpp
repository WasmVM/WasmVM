#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <Allocates.h>
#include <core/Store.h>
#include <dataTypes/vector_p.h>
#include <dataTypes/list_p.h>
#include <structures/WasmFunc.h>
#include <structures/instrs/Variable.h>
#include <structures/instrs/Numeric.h>
#include <dataTypes/FuncType.h>
#include <dataTypes/Value.h>
#include <Opcodes.h>
#include <instance/ModuleInst.h>
#include <instance/VariableInstrInst.h>
#include <instance/NumericInstrInst.h>
}
#undef _Bool

SKYPAT_F(allocate_function, valid)
{
    // Prepare
    Store store = new_Store();
    char* moduleName = (char*) malloc(sizeof(char) * 5);
    strcpy(moduleName, "test");
    ModuleInst* moduleInst = new_ModuleInst(moduleName);

    FuncType type = new_FuncType();
    ValueType* param1 = (ValueType*) malloc(sizeof(ValueType));
    *param1 = Value_i32;
    vector_push_back(type->params, param1);
    ValueType* result1 = (ValueType*) malloc(sizeof(ValueType));
    *result1 = Value_i32;
    vector_push_back(type->results, result1);
    vector_push_back(moduleInst->types, type);

    WasmFunc* func = new_WasmFunc();
    ValueType* local1 = (ValueType*) malloc(sizeof(ValueType));
    *local1 = Value_i32;
    vector_push_back(func->locals, local1);
    WasmVariableInstr* getParamInstr = new_WasmVariableInstr(0);
    getParamInstr->parent.opcode = Op_get_local;
    list_push_back(func->body, getParamInstr);
    WasmVariableInstr* getLocalInstr = new_WasmVariableInstr(1);
    getLocalInstr->parent.opcode = Op_get_local;
    list_push_back(func->body, getLocalInstr);
    WasmNumericInstr* addInstr = new_WasmNumericInstr();
    addInstr->parent.opcode = Op_i32_add;
    list_push_back(func->body, addInstr);

    // Test
    uint32_t address = allocate_Function(func, store, moduleInst);
    FuncInst* funcInst = vector_at(FuncInst*, store->funcs, address);
    EXPECT_EQ(vector_size(funcInst->type->params), 1);
    EXPECT_EQ(*vector_at(ValueType*, funcInst->type->params, 0), Value_i32);
    EXPECT_EQ(vector_size(funcInst->type->results), 1);
    EXPECT_EQ(*vector_at(ValueType*, funcInst->type->results, 0), Value_i32);
    EXPECT_EQ(*vector_at(ValueType*, funcInst->locals, 0), Value_i32);
    EXPECT_EQ(funcInst->module, moduleInst);
    EXPECT_EQ((list_at(VariableInstrInst*, funcInst->code, 0))->parent.opcode, Op_get_local);
    EXPECT_EQ((list_at(VariableInstrInst*, funcInst->code, 0))->index, 0);
    EXPECT_EQ((list_at(VariableInstrInst*, funcInst->code, 1))->parent.opcode, Op_get_local);
    EXPECT_EQ((list_at(VariableInstrInst*, funcInst->code, 1))->index, 1);
    EXPECT_EQ((list_at(NumericInstrInst*, funcInst->code, 2))->parent.opcode, Op_i32_add);

    // Clean
    free_Store(store);
    free_WasmFunc(func);
    free_ModuleInst(moduleInst);
}